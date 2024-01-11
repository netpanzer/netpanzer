test_cases = [
  'package'
  ]

foreach case : test_cases
  exe = executable(
    'test_' + case,
    case + '.cpp',
    include_directories : inc_dirs,
    link_with: lib_netpanzer.get_shared_lib(),
    dependencies : deps,
    # c_args: '-DSOURCE_DIR="@0@"'.format(meson.current_source_dir())
  )
  test(case, exe)
endforeach
