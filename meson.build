project(
  'netpanzer',
  ['c', 'cpp'],
  version: '0.9.0',
  meson_version : '>= 0.59.0',
  default_options: [
    'warning_level=2',
    'cpp_std=c++17',
    'b_sanitize=address,undefined',
    'b_lundef=false',
    ]
  )

# Setup compiler.
cxx = meson.get_compiler('cpp')
cc = meson.get_compiler('c')
extra_flags = [
  '-fno-common',
#  '-fstack-protector-strong',
#  '-Wformat-security',
#  '-Wformat-overflow=2',
  '-Wno-shadow',
  '-Werror=odr',
  '-Werror=lto-type-mismatch',
#  '-Werror=strict-aliasing',
#  '-Wstrict-overflow=5'
# '-Wconversion'
  ]

ldflags = []

mingw_flags = [
    '-static-libgcc',
    '-static-libstdc++',
  ]

host_sys = host_machine.system()
platform_is_windows = (host_sys == 'windows')

add_project_arguments(cc.get_supported_arguments(extra_flags), language: 'c')
add_project_arguments(cxx.get_supported_arguments(extra_flags), language: 'cpp')

if platform_is_windows
  ldflags += mingw_flags
endif

add_project_link_arguments(cxx.get_supported_link_arguments(ldflags), language: 'cpp')

deps = []

if platform_is_windows == true
    deps += cxx.find_library('ws2_32')
    deps += cxx.find_library('winpthread', static: true)
endif

conf = configuration_data()
subdir('po')
localedir = join_paths(get_option('prefix'), get_option('localedir'))
conf.set_quoted('NP_LOCALEDIR', localedir)
conf.set_quoted('PACKAGE_VERSION', meson.project_version())
conf.set_quoted('NP_DATADIR', join_paths(
  get_option('prefix'),
  get_option('datadir'),
  meson.project_name())
  )

config_h = configure_file(output : 'config.h', configuration : conf)

meson.add_devenv({'NETPANZER_DATADIR': join_paths(meson.project_source_root(), 'data')})
meson.add_devenv({'NETPANZER_LOCALEDIR': join_paths(meson.project_build_root(), 'po')})

# this won't use fallback defined in subprojects/lua.wrap
lua_dep = dependency('lua-5.1', required: false)
if not lua_dep.found()
  lua_dep = cc.find_library('lua', has_headers: 'lua.h', required: false)
  if not lua_dep.found()
    # This will use the fallback
    lua_dep = dependency(
      'lua-5.1',
      required: true,
      fallback: 'lua',
      default_options: ['default_library=static']
      )
  endif
endif
deps += lua_dep

phyfs_dep = dependency('physfs', required: false)
if not phyfs_dep.found()
  phyfs_dep = cc.find_library('physfs', has_headers: 'physfs.h', required: false)
  if not phyfs_dep.found()
    # This will use the fallback
    phyfs_dep = dependency(
      'physfs',
      required: true,
      fallback: 'physfs',
      default_options: ['default_library=static']
      )
  endif
endif
deps += phyfs_dep

sdl2_dep = dependency('SDL2', version: '>=2.0.18', required: false)
if not sdl2_dep.found()
  sdl2_dep = cc.find_library('SDL2', has_headers: 'SDL.h', required: false)
  if not sdl2_dep.found()
    # This will use the fallback
    sdl2_dep = dependency(
      'sdl2',
      version: '>=2.0.18',
      required: true,
      fallback: 'sdl2',
      default_options: 'default_library=static')
  endif
endif
deps += sdl2_dep

sdl2main_dep = []
if platform_is_windows
  sdl2main_dep = dependency('SDL2_main', required: false)
  if not sdl2main_dep.found()
    sdl2main_dep = cc.find_library('SDL2_main', has_headers: 'SDL_main.h', required: false)
    if not sdl2main_dep.found()
      # This will use the fallback
      sdl2main_dep = dependency(
        'sdl2main',
        required: true,
        fallback: 'sdl2',
        default_options: ['default_library=static'])
    endif
  endif
endif
# Don't add to deps, but add it to netpanzer_dep when the dependency is declared

sdl2_ttf_dep = dependency('SDL2_ttf', version: '>=2.1', required: false)
if not sdl2_ttf_dep.found()
  sdl2_ttf_dep = cc.find_library('SDL2_ttf', has_headers: 'SDL_ttf.h', required: false)
  if not sdl2_ttf_dep.found()

# Freetype configuration must happen first, otherwise using the freetype2 default_options
# won't work because it will have been configured by sdl2_ttf.
# zlib and png are enabled by default by the SDL2_ttf meson wrap, but not required
# by NetPanzer.
    freetype2_dep = dependency('freetype2', required: false)
    if not freetype2_dep.found()
      freetype2_dep = cc.find_library(
        'freetype2',
        has_headers: 'freetype.h',
        required: false
        )
      if not freetype2_dep.found()
        freetype2_dep = dependency(
          'freetype2',
          required: true,
          fallback: 'freetype2',
          default_options: ['default_library=static', 'png=disabled', 'zlib=disabled'])
      endif
    endif
    deps += freetype2_dep

    sdl2_ttf_dep = dependency(
      'sdl2_ttf',
      version: '>=2.1',
      required: true,
      fallback: 'sdl2_ttf',
      default_options: ['default_library=static']
      )
  endif
endif
deps += sdl2_ttf_dep

sdl2_mixer_dep = dependency('SDL2_mixer', required: false)
if not sdl2_mixer_dep.found()
  sdl2_mixer_dep = cc.find_library('SDL2_mixer', has_headers: 'SDL_mixer.h', required: false)
  if not sdl2_mixer_dep.found()
    # This will use the fallback
    sdl2_mixer_dep = dependency(
      'sdl2_mixer',
      required: true,
      fallback: 'sdl2_mixer',
      default_options: ['default_library=static']
      )
  endif
endif
deps += sdl2_mixer_dep

inc_dirs = [include_directories('.')]

subdir('src')

if host_sys == 'emscripten'
    if get_option('b_staticpic')
        warning('b_staticpic is not compatible with the LLVM WebAssembly backend')
    endif

    # Asyncify instrumentation is very overzealous by default.
    # This crazy list is not exhaustive, but still helps us reduce code size quite a bit.
    # Link with -s ASYNCIFY_ADVISE to see what gets instrumented.
    asyncify_ignore = [
        '*printf',
        'AlphaApplyFilter',
        'ApplyInverseTransforms',
        'Bezier_*',
        'DecodeImageData',
        'DoUVTransform',
        'EmitFancyRGB',
        'EmitRescaledAlphaRGB',
        'EmitRescaledRGB',
        'EmitSampledRGB',
        'Emscripten_*',
        'FT_*',
        'FlushRenderCommands',
        'ReconstructRow',
        'SDL_*',
        'T1_*',
        'TT_*',
        'VP8*',
        'VP8ExitCritical',
        'WebP*',
        'ZSTD_*',
        '__*',
        '__ftello_unlocked',
        '__fwritex',
        '__shgetc',
        '__toread',
        '__vfprintf_internal',
        '_dynarray_*',
        '_ht_*',
        '_netpanzer_log*',
        '_text_draw',
        'af_*',
        'afm_*',
        'apply_kerning',
        'astream_*',
        'basist*',
        'camera3d_*',
        'cf2_*',
        'cff_*',
        'deflate*',
        'fbmgr_*',
        'fclose',
        'fread',
        'ft_*',
        'gamepad_*',
        'get_glyph',
        'gl33_*',
        'glcommon_*',
        'gles30_*',
        'hash_bucket',
        'ht_*',
        'inflate*',
        'log_*',
        'log_internal',
        'op_get_next_page',
        'op_seek_helper',
        'pfr_*',
        'pixmap_*',
        'png_*',
        'printf_core',
        'px_*',
        'qsort',
        'sfnt_*',
        'sift',
        'stdio_*',
        'strtox',
        't1_*',
        't42_*',
        'tt_*',
        'vfs_decref',
        'wrapper_cmp',
    ]

    em_debug = get_option('debug')
    em_link_outputs = []
    em_link_output_suffixes = ['html', 'wasm', 'js']  # first element is significant
    em_data_dir = get_option('datadir')
    em_common_args = []
    em_link_args = [
        '-O@0@'.format(get_option('optimization')),
        '-s', 'ALLOW_MEMORY_GROWTH=1',
        '-s', 'ASYNCIFY_REMOVE=@0@'.format(','.join(asyncify_ignore)),
        '-s', 'AUTO_ARCHIVE_INDEXES=0',
        '-s', 'AUTO_JS_LIBRARIES=0',
        '-s', 'AUTO_NATIVE_LIBRARIES=0',
        '-s', 'DEFAULT_LIBRARY_FUNCS_TO_INCLUDE=["$autoResumeAudioContext"]',
        '-s', 'DYNAMIC_EXECUTION=0',
        '-s', 'ENVIRONMENT=web',
        '-s', 'EXIT_RUNTIME=0',
        '-s', 'EXPORTED_FUNCTIONS=["_main", "_vfs_sync_callback"]',
        '-s', 'EXPORTED_RUNTIME_METHODS=["ccall"]',
        '-s', 'FETCH',
        '-s', 'FETCH_SUPPORT_INDEXEDDB=0',
        '-s', 'FILESYSTEM=1',
        '-s', 'FORCE_FILESYSTEM=1',
        '-s', 'GL_ENABLE_GET_PROC_ADDRESS',
        '-s', 'GL_POOL_TEMP_BUFFERS=0',
        '-s', 'GL_PREINITIALIZED_CONTEXT=1',
        '-s', 'GL_SUPPORT_AUTOMATIC_ENABLE_EXTENSIONS=0',
        '-s', 'GL_SUPPORT_SIMPLE_ENABLE_EXTENSIONS=0',
        '-s', 'IGNORE_MISSING_MAIN=0',
        '-s', 'INITIAL_MEMORY=268435456',
        '-s', 'LLD_REPORT_UNDEFINED',
        '-s', 'MAX_WEBGL_VERSION=2',
        '-s', 'MIN_WEBGL_VERSION=2',
        '-s', 'MODULARIZE=0',
        '-s', 'STACK_SIZE=1MB',
        '-s', 'STRICT_JS=1',
        '-s', 'SUPPORT_BIG_ENDIAN=1',
        '-s', 'WASM=1',
        '-lGL',
        '-legl.js',
        '-lidbfs.js',
        '-lwebgl.js',
        '-lhtml5',   # NOTE: SDL dependency; should be fixed there
    ]

#    em_link_args += subproject('???').get_variable('???_external_link_args')

    if em_debug
        em_link_output_suffixes += ['wasm.map']
        em_link_args += [
            '--profiling',
            '-g3',
            '-gsource-map',
            '--source-map-base', meson.get_external_property('source_map_base', 'http://0.0.0.0:6931/'),
            '-s', 'ASSERTIONS=2',
            '-s', 'GL_ASSERTIONS=1',
            '-s', 'GL_DEBUG=1',
            '-s', 'GL_TRACK_ERRORS=1',
        ]
    else
        em_link_args += [
            '-g0',
            '-s', 'ASSERTIONS=0',
            '-s', 'GL_TRACK_ERRORS=0',
        ]

        if get_option('optimization') != '0'
            em_link_args += ['--closure', '1']
        endif
    endif

    foreach suffix : em_link_output_suffixes
        em_link_outputs += ['@0@.@1@'.format('netpanzer', suffix)]
    endforeach

    meson_link_whole_is_broken = meson.version().version_compare('>1.1.0')

    if meson_link_whole_is_broken
        warning('Installing private static library to work around a Meson bug: https://github.com/mesonbuild/meson/issues/12638')
    endif

    version_deps = []
    libnetpanzer = static_library('netpanzer', src, version_deps,
        dependencies : netpanzer_dep_static,
#        c_pch : 'pch/netpanzer_pch.h',
        c_args : [em_common_args, get_option('c_args')],
        install : meson_link_whole_is_broken,
        install_dir : '.',
        install_tag : 'trash',
        # Explicitly give it a predictable name, in case we have to remove it via install script
        name_prefix : 'lib',
        name_suffix : 'a',
    )

    if meson_link_whole_is_broken
        meson.add_install_script(
            post_install_remove_command, 'lib' + libnetpanzer.name() + '.a',
            install_tag : 'trash',
        )
    endif

    netpanzer = static_library('netpanzer-full',
        link_whole : libnetpanzer,
    )

    netpanzer_html = custom_target(em_link_outputs[0],
        command : [
            meson.get_compiler('cpp').cmd_array(),
            netpanzer,
            '--pre-js', em_preamble,
            '--shell-file', em_shell,
            get_option('c_args'),
            get_option('c_link_args'),
            em_common_args,
            em_link_args,
            '-o', '@OUTPUT0@',
        ],
        build_by_default : true,
        output : em_link_outputs,
        install : true,
        install_dir : bindir,
        install_tag : 'runtime',
        console : true,
    )

    bindist_deps += netpanzer_html
endif

subdir('src/tools')
exe = executable(
  'netpanzer',
  'src/NetPanzer/Core/main.cpp',
  win_subsystem: 'windows',
  dependencies: netpanzer_dep_static,
  link_args: ldflags,
  install : true
)

test(
  'test_' + meson.project_name(),
  exe,
  args: '--help',
  )

install_subdir(
  'data',
  install_dir : join_paths(get_option('datadir'), meson.project_name()),
  strip_directory : true
  )

# Check the Dockerfile when updating this
install_data(
  files(
    'README.md',
    'CONTRIBUTING.md',
    'COPYING.txt',
    'docs/SERVER-HOWTO.md',
    ),
  install_dir : get_option('docdir')
  )

if get_option('build_tests')
  subdir('tests')
endif

message('\n\n        == Important Note ==\n')
message('In the build directory, you will need to enter "meson devenv"')
message('to set the environmental variable NETPANZER_DATADIR to the absolute')
message('path of the root of the source directory; or you may set the variable')
message('by entering "export NETPANZER_DATADIR=$PWD" while in the source root')
message('directory.\n')
