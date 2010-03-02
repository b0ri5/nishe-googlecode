#!/usr/bin/python

import os
import sys
import string
import math
import re

from SCons.Builder import Builder
from SCons.Action import Action

"""
	Custom checks for use in scons. Like high resolution clocks, etc.
"""

all_checks = {}

def Checkgettimeofday(context):
	context.Message('Checking for gettimeofday... ')

	result = context.TryLink("""
		#include <sys/time.h>

		int main(int argc, char **argv)
		{
			timeval tv;
			
			gettimeofday(&tv, 0);

			return 0;
		}
		""", '.cpp')

	context.Result(result)

	return result


def CheckQueryPerformanceCounter(context):
	context.Message('Checking for QueryPerformanceCounter... ')

	result = context.TryLink("""
		#include <windows.h>

		int main(int argc, char **argv)
		{
			LARGE_INTEGER n;

			QueryPerformanceCounter(&n);
			n.QuadPart;

			return 0;
		}
		""", '.cpp')

	context.Result(result)

	return result


def Checkrusage(context):
	context.Message('Checking for rusage... ')

	result = context.TryLink("""
		#include <sys/time.h>
		#include <sys/resource.h>

		int main(int argc, char **argv)
		{
			struct rusage ruse;
			
			getrusage(RUSAGE_SELF, &ruse);
			
			return 0;
		}
		""", '.cpp')

	context.Result(result)
	
	return result


hrtime_checks = { 'CheckQueryPerformanceCounter': CheckQueryPerformanceCounter,
	'Checkgettimeofday': Checkgettimeofday , 'Checkrusage':Checkrusage }

# update the list of all checks
all_checks.update(hrtime_checks)


def CheckMPICXX(context, mpi_cxx):
	context.Message('Checking for mpicxx... ')

	# don't want to change this environment right now
	prev_cxx = context.env['CXX']
	context.env.Replace(CXX = mpi_cxx)

	result = context.TryLink("""
		#include <mpi.h>
		
		int main(int argc, char **argv)
		{
			MPI::Init(argc, argv);
			MPI::Finalize();
			
			return 0;
		}
		""", '.cpp')

	# so once we're done testing, revert
	context.env.Replace(CXX = prev_cxx)

	context.Result(result)

	return result

mpi_checks = { 'CheckMPICXX': CheckMPICXX }

# update all of the checks
all_checks.update(mpi_checks)


"""
	Personal configure things to always make sure they are enabled.
"""
def config_general(env):
	
	# Clone the environment and append the local path
	env = env.Clone()
	env.AppendENVPath('PATH', os.environ['PATH'])

	# I SHOULDN'T HAVE TO ADD THIS BUT I DO IN ORDER TO CLEAN?!?! WHY!?!?!
	# otherwise it dies in CONFIG_HRTIME
	env['CPPDEFINES'] = []
	
	# add /EHsc to get rid of annoying cl warnings, some apps need this too
	if (env['CC'] == 'cl'):
		env.AppendUnique(CPPFLAGS = ['/EHsc'] )

	return env



"""
	Configure an environment for debugging.
"""
def config_debug(env):
	env = config_general(env)
		
	if (not env['CC'] == 'cl'):
		env.AppendUnique(CCFLAGS = ['-g'] )
	else:
		# these were garned through MSVS's stuff
		env.AppendUnique(CCFLAGS = ['/Zi'] )
		env.AppendUnique(LINKFLAGS = ['/DEBUG'] )
		
		# I consider this a bug in SCons, but using mutliple jobs
		# in conjunction with /Zi flag for cl tries to jump the gun in 
		# reading the vc70.pdb file, fixed by using only 1 job to
		# serialize this
		env.SetOption('num_jobs', 1)
		
	return env

"""
	Add release/optimization flags
"""
def config_release(env):
	env = config_general(env)
	
	env.AppendUnique(CPPDEFINES = ['NDEBUG'] )
	
	if (not env['CC'] == 'cl'):
		env.AppendUnique(CCFLAGS = ['-O3'] )
	else:
		env.AppendUnique(CCFLAGS = ['/O2'] )
		
	return env;

"""
	Add profiling flags. So far this only applies with gprof
"""
def config_profile(env):
	env = config_release(env)
	
	if (env['CC'] == 'gcc'):
		env.AppendUnique(CCFLAGS = ['-pg'] )
		env.AppendUnique(LINKFLAGS = ['-pg'] )

	return env

"""
	Configure the best choice for a high resolution timer (in hrtimer.cpp).
"""
def config_hrtime(env, conf):
	env = config_general(env)

	# defines for for calendar_time() (not sure about QueryPerformanceCounter
	if conf.Checkgettimeofday():
		env.AppendUnique(CPPDEFINES = ['HAS_GETTIMEOFDAY'])	
	elif conf.CheckQueryPerformanceCounter():
		env.AppendUnique(CPPDEFINES = ['HAS_QUERY_PERFORMANCE_COUNTER'])
	
	#defines for cpu_time()
	if (conf.Checkrusage()):
		env.AppendUnique(CPPDEFINES = ['HAS_RUSAGE'])

	return env

def merge_hrtime(env, hrtime_env):
	env = env.Clone()
	
	env.AppendUnique(CPPDEFINES = hrtime_env['CPPDEFINES'])

	return env

def config_mpi(env): 
	env = config_general(env)
	
	env.Replace(CC = 'mpicc', CXX = 'mpicxx')
	 
	return env

if __name__ == "__main__":
	main()
