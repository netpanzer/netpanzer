test_cases = [
  '../src/Lib/package'
  ]
fs = import('fs')

foreach case : test_cases
  exe = executable(
    'test_' + fs.name(case),
    case + '.cpp',
    include_directories : inc_dirs,
    link_with: lib_netpanzer.get_shared_lib(),
    dependencies : deps,
#    c_args: '-DSOURCE_DIR="@0@"'.format(meson.current_source_dir())
    cpp_args: '-DTEST_LIB'
  )
  test(case, exe)
endforeach
