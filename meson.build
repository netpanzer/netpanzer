project(
  'netpanzer',
  ['c', 'cpp'],
  version: '0.9.0.06',
  meson_version : '>= 0.56.2',
  default_options: [
    'warning_level=1',
    'cpp_std=c++17'
    ]
  )

# Setup compiler.
cxx = meson.get_compiler('cpp')
cc = meson.get_compiler('c')

extra_flags = [
  '-fno-common',
  '-DPACKAGE_VERSION="@0@"'.format(meson.project_version()),
#  '-fstack-protector-strong',
#  '-Wformat-security',
#  '-Wformat-overflow=2',
#  '-Wshadow',
#  '-Wstrict-overflow=5'
# '-Wconversion'
  ]

add_project_arguments(cc.get_supported_arguments(extra_flags), language: 'c')
add_project_arguments(cxx.get_supported_arguments(extra_flags), language: 'cpp')
subdir('src')
