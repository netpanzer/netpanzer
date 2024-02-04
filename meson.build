project(
  'netpanzer',
  ['c', 'cpp'],
  version: '0.9.0.07.555',
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
#  '-Wshadow',
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
conf.set_quoted('NP_DATADIR', join_paths(get_option('prefix'), get_option('datadir')))
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
deps += lua_dep

phyfs_dep = dependency('physfs', required: false)
if not phyfs_dep.found()
  phyfs_dep = cc.find_library('physfs', has_headers: 'physfs.h', required: false)
  if not phyfs_dep.found()
    # This will use the fallback
    phyfs_dep = dependency(
      'physfs',
      required: true,
      fallback: 'physfs',
      default_options: ['default_library=static']
      )
  endif
endif
deps += phyfs_dep

sdl2_dep = dependency('SDL2', required: false)
if not sdl2_dep.found()
  sdl2_dep = cc.find_library('SDL2', has_headers: 'SDL.h', required: false)
  if not sdl2_dep.found()
    # This will use the fallback
    sdl2_dep = dependency(
      'sdl2',
      required: true,
      fallback: 'sdl2',
      default_options: ['default_library=static'])
  endif
endif
deps += sdl2_dep

bin_deps = []
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
  bin_deps += sdl2main_dep
endif

# Don't add to deps, only link with the binary and the test programs
# not the shared library.

freetype_use_embedded = false
sdl2_ttf_dep = dependency('SDL2_ttf', required: false)
if not sdl2_ttf_dep.found()
  sdl2_ttf_dep = cc.find_library('SDL2_ttf', has_headers: 'SDL_ttf.h', required: false)
  if not sdl2_ttf_dep.found()
    # This will use the fallback
    sdl2_ttf_dep = dependency(
      'sdl2_ttf',
      required: true,
      fallback: 'sdl2_ttf',
      default_options: ['default_library=static']
      )
    freetype_use_embedded = true
  endif
endif
deps += sdl2_ttf_dep

# These are required by the SDL2_mixer meson wrap, but not by NetPanzer
# If SDL2_mixer was found without using the fallback, chances are that it already
# has all its dependencies.
if freetype_use_embedded
  freetype2_dep = dependency('freetype2', required: false)
  if not freetype2_dep.found()
    freetype2_dep = cc.find_library('freetype2', has_headers: 'freetype.h', required: false)
    if not freetype2_dep.found()
      freetype2_dep = dependency(
        'freetype2',
        required: true,
        fallback: 'freetype2',
        default_options: ['default_library=static'])
    endif
  endif
  deps += freetype2_dep
endif

sdl2_mixer_use_embedded = false
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
    sdl2_mixer_use_embedded = true
  endif
endif
deps += sdl2_mixer_dep

# These will be built as optional dependencies of SDL2_mixer by default
# if found on the system, but they are not required by NetPanzer. If SDL2_mixer was
# found without using the fallback, chances are that it already has all its
# dependencies.
if sdl2_mixer_use_embedded
  libogg_dep = dependency('ogg', required: false)
  if not libogg_dep.found()
    libogg_dep = cc.find_library('ogg', has_headers: 'ogg.h', required: false)
    if not libogg_dep.found()
      libogg_dep = dependency(
        'ogg',
        required: true,
        fallback: 'ogg',
        default_options: ['default_library=static'])
    endif
  endif
  deps += libogg_dep

  vorbis_dep = dependency('vorbis', required: false)
  if not vorbis_dep.found()
    vorbis_dep = cc.find_library('vorbis', has_headers: 'vorbisfile.h', required: false)
    if not vorbis_dep.found()
      vorbis_dep = dependency(
        'vorbis',
        required: true,
        fallback: 'vorbis',
        default_options: ['default_library=static'])
    endif
  endif
  deps += vorbis_dep
endif

inc_dirs = [include_directories('.')]

subdir('src')
bin_deps += deps
subdir('support/tools')
exe = executable(
  'netpanzer',
  'src/NetPanzer/Core/main.cpp',
  win_subsystem: 'windows',
  include_directories : [
    inc_dirs
  ],
  dependencies: [
    bin_deps,
  ],
  link_with: lib_netpanzer.get_static_lib(),
  link_args: ldflags,
  install : true
)

install_subdir(
  'data',
  install_dir : join_paths(get_option('datadir'), meson.project_name()),
  strip_directory : true
  )

if not platform_is_windows
  subdir('tests')
endif

message('\n\n        == Important Note ==\n')
message('In the build directory, you will need to enter "meson devenv"')
message('to set the environmental variable NETPANZER_DATADIR to the absolute')
message('path of the root of the source directory; or you may set the variable')
message('by entering "export NETPANZER_DATADIR=$PWD" while in the source root')
message('directory.\n')
