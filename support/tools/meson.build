paktool = ['bmp2pak', 'pak2bmp']

foreach tool : paktool
  executable(
    tool,
    tool + '.cpp',
    win_subsystem: 'console',
    dependencies: netpanzer_dep_static,
    link_args: ldflags,
    install : false
  )
endforeach
