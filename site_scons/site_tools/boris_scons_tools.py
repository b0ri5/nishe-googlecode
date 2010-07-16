from SCons.Script import *

def generate(env, **kw):
  env.PrependENVPath('PATH', os.environ['PATH'])
  env.Tool('default')
  env.Tool('cmake')
  
def exists(env):
  return True