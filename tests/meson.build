# In some cases, we will want separate arrays for test_cases because some will
# have different dependencies and we want to avoid unnecessarily linking every
# test binary to unneeded dependencies. # # It may be possible to use one
# foreach loop and specify the 'dependencies: ...' conditionally

test_cases = [
  '../src/Lib/package'
  ]

fs = import('fs')
foreach case : test_cases
  test_name = 'test_' + fs.name(case)
  exe = executable(
    'test_' + fs.name(case),
    case + '.cpp',
    include_directories : inc_dirs,
    link_with: lib_netpanzer.get_shared_lib(),
#    dependencies : deps,
#    c_args: '-DSOURCE_DIR="@0@"'.format(meson.current_source_dir())
    cpp_args: '-DTEST_LIB'
  )
  test(test_name, exe)
endforeach
