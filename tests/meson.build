test_cases = [
  '../src/Lib/package',
  '../src/Lib/2D/CachedFontRenderer',
  '../src/Lib/Util/FileSystem'
  ]

fs = import('fs')
foreach case : test_cases
  test_name = 'test_' + fs.name(case)
  exe = executable(
    'test_' + fs.name(case),
    case + '.cpp',
# On Windows, linking against the shared library results in
# "multiple definition of unwind_Resume.
    dependencies : (platform_is_windows ? netpanzer_dep_static : netpanzer_dep_shared),
#    c_args: '-DSOURCE_DIR="@0@"'.format(meson.current_source_dir())
    cpp_args: [
      '-DTEST_LIB',
      '-DMESON_BUILD_ROOT="@0@"'.format(meson.project_build_root()),
      '-DMESON_SOURCE_ROOT="@0@"'.format(meson.project_source_root())
      ],
    win_subsystem: 'console',
    override_options: ['b_lto=false']
  )

  test(
    'test_' + fs.name(case),
    exe,
    env: [
      # 'NETPANZER_DATADIR="@0@"'.format(meson.project_source_root())
      ]
    )
endforeach
