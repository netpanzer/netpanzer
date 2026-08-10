paktool_deps = [
  nputil_dep,
  np2D_dep,
  ]
paktool = ['bmp2pak', 'pak2bmp']

foreach tool : paktool
  executable(
    tool,
    tool + '.cpp',
    win_subsystem: 'console',
    dependencies: paktool_deps,
    link_args: ldflags,
    install : false
  )
endforeach
