deps_npscript = [lua_dep, sdl2_dep,nputil_dep]

lib_npscript = library (
  'npscript',
  [
    'ScriptManager.cpp',
    'NetPanzerModule.cpp',
    'ScriptHelper.cpp'
    ],
  include_directories: inc_dirs,
  dependencies: deps_npscript,
  install: false
  )

npscript_dep = declare_dependency(
  link_with: lib_npscript,
  include_directories: inc_dirs,
  dependencies: deps_npscript
  )

deps += npscript_dep
