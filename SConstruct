"""
	Copyright 2010 Greg Tener
	Released under the Lesser General Public License v3.
"""

import os
import checks

env = Environment(tools = ['default', 'packaging', 'boris_scons_tools'])

# add checks for the high resolution timer
conf = Configure(env, checks.hrtime_checks)

# get the environment for high resolution timing
hrtime_env = checks.config_hrtime(env, conf)

conf.Finish()

# tell the sconscripts about the timing environment 
Export('hrtime_env')

debug_env = checks.config_debug(env)
release_env = checks.config_release(env) 
profile_env = checks.config_profile(env)

Export({'env': debug_env, 'libsuffix': '-db'})
debug_env.SConscript('src/SConscript',
					build_dir='build/debug', duplicate=0)
#debug_env.SConscript('test/SConscript',
#					build_dir='build/test/debug', duplicate=0)

#Export({'env': release_env, 'libsuffix': ''})
#release_env.SConscript('src/SConscript', build_dir='build/release', duplicate=0)

#Export({'env': profile_env, 'libsuffix': '-prof'})
#profile_env.SConscript('src/SConscript', build_dir='build/profile', duplicate=0)