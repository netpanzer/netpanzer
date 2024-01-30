executable(
  'bmp2pak',
  'bmp2pak.cpp',
  win_subsystem: 'console',
  include_directories : [
    inc_dirs
  ],
  dependencies: [ bin_deps
      ],
  link_with: lib_netpanzer.get_shared_lib(),
  link_args: ldflags,
  install : false
)

executable(
  'pak2bmp',
  'pak2bmp.cpp',
  win_subsystem: 'console',
  include_directories : [
    inc_dirs
  ],
  dependencies: [ bin_deps
      ],
  link_with: lib_netpanzer.get_shared_lib(),
  link_args: ldflags,
  install : false
)
