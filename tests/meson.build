test_cases = [
  '../src/Lib/package',
  '../src/Lib/2D/CachedFontRenderer',
  '../src/Lib/Util/FileSystem'
  ]

# Tests fail to compile if the shared library is used on Windows
# R:\winlibs64-11.3.0ucrt\2604.gcc\gcc-12.2.0\build_mingw\x86_64-w64-mingw32\libgcc/../../../libgcc/unwind-seh.c:347: multiple definition of `_Unwind_Resume'; src/NetPanzer/libnetpanzer.dll.a(libnetpanzer_dll_d003040.o):(.text+0x0): first defined here
np_lib = []
if not platform_is_windows
  np_lib = lib_netpanzer.get_shared_lib()
else
  np_lib = lib_netpanzer.get_static_lib()
endif

fs = import('fs')
foreach case : test_cases
  test_name = 'test_' + fs.name(case)
  exe = executable(
    'test_' + fs.name(case),
    case + '.cpp',
    include_directories : inc_dirs,
    dependencies : bin_deps,
    link_with: np_lib,
#    c_args: '-DSOURCE_DIR="@0@"'.format(meson.current_source_dir())
    cpp_args: [
      '-DTEST_LIB',
      '-DMESON_BUILD_ROOT="@0@"'.format(meson.project_build_root()),
      '-DMESON_SOURCE_ROOT="@0@"'.format(meson.project_source_root())
      ],
    win_subsystem: 'console',
  )

  test(
    'test_' + fs.name(case),
    exe,
    env: [
      # 'NETPANZER_DATADIR="@0@"'.format(meson.project_source_root())
      ],
    depends: lib_netpanzer.get_shared_lib()
    )
endforeach
