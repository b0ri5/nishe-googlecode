"""
  Copyright 2010 Greg Tener
  Released under the Lesser General Public License v3.
"""

import os
import checks

env = Environment(tools = ['default', 'packaging', 'boris_scons_tools'])

# begin checks
conf = Configure(env, checks.all_checks)

# check for gtest
has_gtest = conf.CheckLibWithHeader('gtest', 'gtest/gtest.h', 'C++', autoadd=0)

# only check for cmake if needed
if not has_gtest:
  print '  gtest not installed, will try to use local copy'
  has_cmake = conf.CheckExecutable('cmake')
  if not has_cmake:
    print '  cmake not found, download it at http://www.cmake.org/'

# get the environment for high resolution timing
hrtime_env = checks.config_hrtime(env, conf)
Export('hrtime_env')

env = conf.Finish()  # get our environment back!

# set up for using multiple configurations, using debug as the default
configs = ARGUMENTS.get('config', 'debug') 
config_libsuffixes = {'debug': '-db', 'release': '', 'profile': '-prof'}

for config in configs.split(','):
  print '***Building for %s***' % (config)
  config_env = checks.config(env, config)
  libsuffix = config_libsuffixes[config]
  config_env['CONFIGURATION'] = config
  
  # export the environment and libsuffix to the SConscripts
  Export({'env': config_env, 'libsuffix': libsuffix})
  
  # build the nishe library
  config_env.SConscript('src/SConscript',
          build_dir='build/%s/src' % (config), duplicate=0)
  
  config_env['GTEST_LIB'] = ''
  config_env['GTEST_INCLUDE'] = ''
  libgtest = None

  if not has_gtest and has_cmake:
    # build the copy of gtest in third-party
    
    config_env['GTEST_INCLUDE'] = '#/third-party/gtest-1.5.0/include'
    # the SConscript will set GTEST_LIB appropriately
    config_env.SConscript('third-party/SConscript')
    Import('libgtest')
    has_gtest = True
 
  if has_gtest:
    Export('libgtest')
    config_env.SConscript('test/SConscript',
          build_dir='build/%s/test' % (config), duplicate=0)
    has_gtest = False # falsify this for future configs that might need it

