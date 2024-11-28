project(
  'netpanzer',
  ['c', 'cpp'],
  version: '0.9.0-rc8-dev',
  meson_version : '>= 0.59.0',
  default_options: [
    'warning_level=2',
    'cpp_std=c++17',
    'b_sanitize=address,undefined',
    'b_lundef=false',
    ]
  )

# Setup compiler.
cxx = meson.get_compiler('cpp')
cc = meson.get_compiler('c')
extra_flags = [
  '-fno-common',
#  '-fstack-protector-strong',
#  '-Wformat-security',
#  '-Wformat-overflow=2',
  '-Wno-shadow',
  '-Werror=odr',
  '-Werror=lto-type-mismatch',
  '-Werror=strict-aliasing',
#  '-Wstrict-overflow=5'
# '-Wconversion'
  ]

ldflags = []

mingw_flags = [
    '-static-libgcc',
    '-static-libstdc++',
  ]

host_sys = host_machine.system()
platform_is_windows = (host_sys == 'windows')

add_project_arguments(cc.get_supported_arguments(extra_flags), language: 'c')
add_project_arguments(cxx.get_supported_arguments(extra_flags), language: 'cpp')

if platform_is_windows
  ldflags += mingw_flags
endif

add_project_link_arguments(cxx.get_supported_link_arguments(ldflags), language: 'cpp')

deps = []

if platform_is_windows == true
    deps += cxx.find_library('ws2_32')
    deps += cxx.find_library('winpthread', static: true)
endif

conf = configuration_data()
subdir('po')
localedir = join_paths(get_option('prefix'), get_option('localedir'))
conf.set_quoted('NP_LOCALEDIR', localedir)
conf.set_quoted('PACKAGE_VERSION', meson.project_version())
conf.set_quoted('NP_DATADIR', join_paths(
  get_option('prefix'),
  get_option('datadir'),
  meson.project_name())
  )

config_h = configure_file(output : 'config.h', configuration : conf)

meson.add_devenv({'NETPANZER_DATADIR': join_paths(meson.project_source_root(), 'data')})
meson.add_devenv({'NETPANZER_LOCALEDIR': join_paths(meson.project_build_root(), 'po')})

# this won't use fallback defined in subprojects/lua.wrap
lua_dep = dependency('lua-5.1', required: false)
if not lua_dep.found()
  lua_dep = cc.find_library('lua', has_headers: 'lua.h', required: false)
  if not lua_dep.found()
    # This will use the fallback
    lua_dep = dependency(
      'lua-5.1',
      required: true,
      fallback: 'lua',
      default_options: ['default_library=static']
      )
  endif
endif

# Included by np libraries where needed
# If added to the global 'deps' var, all tests fail when building with 'sanitize' and
# using the lua wrap instead linking to system lua.
# deps += lua_dep

physfs_dep = dependency('physfs', required: false)
if not physfs_dep.found()
  physfs_dep = cc.find_library('physfs', has_headers: 'physfs.h', required: false)
  if not physfs_dep.found()
    # This will use the fallback
    physfs_dep = dependency(
      'physfs',
      required: true,
      fallback: 'physfs',
      default_options: ['default_library=static']
      )
  endif
endif
deps += physfs_dep

sdl2_dep = dependency('SDL2', version: '>=2.0.18', required: false)
if not sdl2_dep.found()
  sdl2_dep = cc.find_library('SDL2', has_headers: 'SDL.h', required: false)
  if not sdl2_dep.found()
    # This will use the fallback
    sdl2_dep = dependency(
      'sdl2',
      version: '>=2.0.18',
      required: true,
      fallback: 'sdl2',
      default_options: 'default_library=static')
  endif
endif
deps += sdl2_dep

sdl2main_dep = []
if platform_is_windows
  sdl2main_dep = dependency('SDL2_main', required: false)
  if not sdl2main_dep.found()
    sdl2main_dep = cc.find_library('SDL2_main', has_headers: 'SDL_main.h', required: false)
    if not sdl2main_dep.found()
      # This will use the fallback
      sdl2main_dep = dependency(
        'sdl2main',
        required: true,
        fallback: 'sdl2',
        default_options: ['default_library=static'])
    endif
  endif
endif
# Don't add to deps, but add it to netpanzer_dep when the dependency is declared

sdl2_ttf_dep = dependency('SDL2_ttf', version: '>=2.1', required: false)
if not sdl2_ttf_dep.found()
  sdl2_ttf_dep = cc.find_library('SDL2_ttf', has_headers: 'SDL_ttf.h', required: false)
  if not sdl2_ttf_dep.found()

# Freetype configuration must happen first, otherwise using the freetype2 default_options
# won't work because it will have been configured by sdl2_ttf.
# zlib and png are enabled by default by the SDL2_ttf meson wrap, but not required
# by NetPanzer.
    freetype2_dep = dependency('freetype2', required: false)
    if not freetype2_dep.found()
      freetype2_dep = cc.find_library(
        'freetype2',
        has_headers: 'freetype.h',
        required: false
        )
      if not freetype2_dep.found()
        freetype2_dep = dependency(
          'freetype2',
          required: true,
          fallback: 'freetype2',
          default_options: ['default_library=static', 'png=disabled', 'zlib=disabled'])
      endif
    endif
    deps += freetype2_dep

    sdl2_ttf_dep = dependency(
      'sdl2_ttf',
      version: '>=2.1',
      required: true,
      fallback: 'sdl2_ttf',
      default_options: ['default_library=static']
      )
  endif
endif
deps += sdl2_ttf_dep

sdl2_mixer_dep = dependency('SDL2_mixer', required: false)
if not sdl2_mixer_dep.found()
  sdl2_mixer_dep = cc.find_library('SDL2_mixer', has_headers: 'SDL_mixer.h', required: false)
  if not sdl2_mixer_dep.found()
    # This will use the fallback
    sdl2_mixer_dep = dependency(
      'sdl2_mixer',
      required: true,
      fallback: 'sdl2_mixer',
      default_options: ['default_library=static']
      )
  endif
endif
deps += sdl2_mixer_dep

inc_dirs = [include_directories('.')]

subdir('src')

subdir('src/tools')
exe = executable(
  'netpanzer',
  'src/NetPanzer/Core/main.cpp',
  win_subsystem: 'windows',
  dependencies: netpanzer_dep_static,
  link_args: ldflags,
  install : true
)

test(
  'test_' + meson.project_name(),
  exe,
  args: '--help',
  )

install_subdir(
  'data',
  install_dir : join_paths(get_option('datadir'), meson.project_name()),
  strip_directory : true
  )

# Check the Dockerfile when updating this
install_data(
  files(
    'README.md',
    'CONTRIBUTING.md',
    'COPYING.txt',
    'docs/SERVER-HOWTO.md',
    ),
  install_dir : get_option('docdir')
  )

if get_option('build_tests')
  subdir('tests')
endif

message('\n\n        == Important Note ==\n')
message('In the build directory, you will need to enter "meson devenv"')
message('to set the environmental variable NETPANZER_DATADIR to the absolute')
message('path of the root of the source directory; or you may set the variable')
message('by entering "export NETPANZER_DATADIR=$PWD" while in the source root')
message('directory.\n')
