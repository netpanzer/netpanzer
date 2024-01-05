project(
  'netpanzer',
  ['c', 'cpp'],
  version: '0.9.0.06',
  meson_version : '>= 0.56.2',
  default_options: [
    'warning_level=1',
    'cpp_std=c++17',
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

add_project_arguments(cc.get_supported_arguments(extra_flags), language: 'c')
add_project_arguments(cxx.get_supported_arguments(extra_flags), language: 'cpp')

conf = configuration_data()
conf.set_quoted('PACKAGE_VERSION', meson.project_version())
conf.set_quoted('NP_DATADIR', join_paths(get_option('prefix'), get_option('datadir')))
config_h = configure_file(output : 'config.h', configuration : conf)

#devenv = environment()
#devenv.set('NETPANZER_DATADIR', meson.source_root())
#meson.add_devenv(devenv)

deps = [
  dependency('SDL2'),
# SDL2 is case sensitive for the ttf library
  dependency('SDL2_ttf'),
  dependency('SDL2_mixer'),
  dependency('physfs')
]

inc_dirs = [include_directories('.')]

subdir('src')

src = []
core_src = [
  'NetworkGlobals.cpp',
  'main.cpp',
]
foreach file : core_src
  src += [join_paths('src/NetPanzer/Core', file)]
endforeach

exe = executable(
  'netpanzer',
  src,
  include_directories : [
    inc_dirs
  ],
  dependencies: [
    dep_netpanzer,
  ],
  install : true, install_dir : get_option('bindir')
)

subdirs = [
  'cache',
  'fonts',
  'maps',
  'pics',
  'powerups',
  'scripts',
  'sound',
  'units',
  'wads'
  ]

foreach dir : subdirs
  install_subdir(
    dir,
    install_dir : get_option('datadir')
  )
endforeach
