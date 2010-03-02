from SCons.Script import *

def generate(env, **kw):
	# append common latex locations and the environment's path
	env.PrependENVPath('PATH', os.environ['PATH'])
	env.Tool('default')

def exists(env):
	return True