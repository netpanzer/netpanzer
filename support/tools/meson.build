paktool = ['bmp2pak', 'pak2bmp']

foreach tool : paktool
  executable(
    tool,
    tool + '.cpp',
    win_subsystem: 'console',
    include_directories : inc_dirs,
    dependencies: bin_deps,
    link_with: lib_netpanzer.get_static_lib(),
    link_args: ldflags,
    install : false
  )
endforeach
