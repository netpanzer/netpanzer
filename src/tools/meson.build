paktool = ['bmp2pak', 'pak2bmp']

foreach tool : paktool
  executable(
    tool,
    tool + '.cpp',
    win_subsystem: 'console',
    dependencies: (platform_is_windows ? netpanzer_dep_static : netpanzer_dep_shared),
    link_args: ldflags,
    install : false
  )
endforeach
