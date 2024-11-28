deps_nputil = [physfs_dep, sdl2_dep]
lib_nputil = library (
  'nputil',
  [
    'TimerInterface.cpp',
    'Math.cpp',
    'UtilInterface.cpp',
    'Timer.cpp',
    'TimeStamp.cpp',
    'Log.cpp',
    'FileSystem.cpp',
    'BresenhamLine.cpp',
    'Exception.cpp',
    'StringUtil.cpp'
    ],
  include_directories: inc_dirs,
  dependencies: deps_nputil,
  install: false
  )

nputil_dep = declare_dependency(
  link_with: lib_nputil,
  include_directories: inc_dirs,
  dependencies: deps_nputil
  )

deps += nputil_dep
