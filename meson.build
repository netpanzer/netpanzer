project(
  'netpanzer',
  ['c', 'cpp'],
  version: '0.9.0.06',
  meson_version : '>= 0.56.2',
  default_options: [
    'warning_level=1',
    'cpp_std=c++17',
    'b_sanitize=address,undefined',
    'b_lundef=false'
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
config_h = configure_file(output : 'config.h', configuration : conf)

deps = [
  dependency('SDL2'),
# SDL2 is case sensitive for the ttf library
  dependency('SDL2_ttf'),
  dependency('SDL2_mixer'),
  dependency('physfs')
]

inc_dirs = [include_directories('.')]

subdir('src')

exe = executable(
  'netpanzer',
  'src/NetPanzer/Core/main.cpp',
  include_directories : [
    inc_dirs
  ],
  link_with: lib_netpanzer.get_static_lib(),
  dependencies: [
    deps,
  ]
)

subdir('tests')