import SCons.Builder
import subprocess as sp
import os
import checks
import SCons.Node.FS
import SCons.Node.Python
from SCons.Script import *

"""
  Add some files that CMake creates to the targets
"""
def cmake_emitter(target, source, env):
  target_dir = target[0]
   
  # need to remove the source-tree directory to avoid a circular dependency
  new_targets = []
  
  # if we're using NMake we need to have different cmake binary-trees for
  # debug and release configurations
  if env['CMAKE_GENERATOR'] == 'NMake Makefiles':
    target_dir = target_dir.Dir(env['CONFIGURATION'])
  elif not env['CMAKE_GENERATOR'].startswith('Visual Studio'):
    target_dir = target_dir.Dir(env['CONFIGURATION'])
    
  new_targets.append(target_dir.Dir('CMakeFiles'))
  new_targets.append(target_dir.File('CMakeCache.txt'))
  new_targets.append(target_dir.File('cmake_install.cmake'))

  return new_targets, source

"""
  Make the binary-tree directory if needed and then run cmake.
    target == binary-tree/<cmake_files>
    source == source-tree directory
"""
def cmake_generator(target, source, env, for_signature):
  target_dir = str(target[0].dir)
  source_dir = str(source[0])
  
  cmd = 'cd ${TARGET.dir} && cmake'
  
  # if we're using a single-configuration generator, specify the configuration
  # when creating the binary tree
  if env['CMAKE_GENERATOR'] == 'NMake Makefiles':
    cmd += ' -D CMAKE_BUILD_TYPE=%s' % (env['CONFIGURATION'])

  relpath = os.path.relpath(source_dir, target_dir)
  cmd += ' %s' % (relpath)
  
  return [Mkdir('${TARGET.dir}'), cmd]


"""
  Add whatever files we know to be in the source folder to the sources,
  and add the .lib or .a files to the targets so we know where they will be as
  well.
"""
def cmake_lib_emitter(target, source, env):
  source_dir = source[0]
  new_target = []
  new_source = []
  libnames = list(target)
  
  target_dir = source_dir
  
  # if we're using visual studios, add a source for each
  # library specified as a .vcproj
  if env['CMAKE_GENERATOR'].startswith('Visual Studio'):
    for libname in libnames:
      new_source.append(source_dir.File('%s.vcproj' % (libname)))
      
    target_dir = source_dir.Dir(env['CONFIGURATION'])
  elif env['CMAKE_GENERATOR'] == 'NMake Makefiles':
    new_source = [source_dir.Dir(env['CONFIGURATION']).File('Makefile')]
    target_dir = source_dir.Dir(env['CONFIGURATION'])
  else:
    new_source = [source_dir.Dir(env['CONFIGURATION']).File('Makefile')]
    target_dir = source_dir.Dir(env['CONFIGURATION'])
  
  # add the .lib or .a's to the targets
  for libname in libnames:
    libpath = '%s%s%s' % (env['LIBPREFIX'], libname, env['LIBSUFFIX'])
    new_target.append(target_dir.File(libpath))
  
  return new_target, new_source

"""
  Builds the cmake binary-tree
"""
def cmake_lib_generator(target, source, env, for_signature):
  return ['cmake --build ${SOURCE.dir} --config %s' % env['CONFIGURATION']]


def set_cmake_generator(env):
  # TODO: determine the actual way of doing this...
  # use a cached file so build's don't always take forever since
  # system-information outputs a LOT
  if not os.path.isfile('.cmake_generator'):
    try:
      p = sp.Popen(['cmake', '--system-information'], stdout=sp.PIPE,
                   stderr=sp.PIPE)
    except:
      return  # the config will detect if we don't have cmake
    
    firstline = p.stdout.readline()
    f = open('.cmake_generator', 'w')
    f.write(firstline)
    f.close()
    p.terminate()
  else:
    f = open('.cmake_generator')
    firstline = f.read()
    f.close()
    
  prefix = '-- Building for: '
  
  env['CMAKE_GENERATOR'] = firstline[len(prefix):].strip()
  

""" Add builders for CMake"""
def generate(env):
  env.PrependENVPath('PATH', os.environ['PATH'])
  env.Tool('default')
  
  # append the builders for using CMake
  bld = SCons.Builder.Builder(target_factory=SCons.Node.FS.Dir,
                              source_factory=SCons.Node.FS.Dir,
                              emitter = cmake_emitter,
                              generator=cmake_generator)
  env.Append(BUILDERS = {'CMake' : bld})
  
  # find and set the CMAKE_GENERATOR
  set_cmake_generator(env)
  
  # append the builders for using CMakeLib
  bld = SCons.Builder.Builder(source_factory=SCons.Node.FS.Dir,
                              target_factory=SCons.Node.Python.Value,
                              emitter=cmake_lib_emitter,
                              generator=cmake_lib_generator)
  env.Append(BUILDERS = {'CMakeLib' : bld})


""" See if we can call CMake"""
def exists(env):
  conf = env.Configure(checks.all_checks)  
  rv = conf.TryAction('cmake')  
  conf.Finish()
  
  return rv[0]