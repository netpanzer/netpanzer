intl_dep = []
if get_option('nls') and not platform_is_windows
  intl_args = []
  intl_dep = dependency('intl', required: false)

  # Works when libintl lives in the compiler's default search path, e.g. when
  # it's part of libc, or on cygwin.
  if not intl_dep.found()
    intl_dep = cc.find_library(
      'intl',
      has_headers: ['libintl.h'],
      required: false
      )
  endif

  # Otherwise look in the prefixes the local package manager installs to. Those
  # aren't searched by default, which is why the build used to fail on macOS
  # with Homebrew on arm64 (/opt/homebrew) or with MacPorts (/opt/local).
  if not intl_dep.found()
    intl_prefixes = []
    if host_sys == 'freebsd' or host_sys == 'openbsd' or host_sys == 'dragonfly'
      intl_prefixes = ['/usr/local']
    elif host_sys == 'netbsd'
      intl_prefixes = ['/usr/pkg']
    elif host_sys == 'darwin'
      # Homebrew keeps gettext keg-only, so it isn't symlinked into the
      # Homebrew prefix unless it was force-linked. Ask brew where it is.
      brew = find_program('brew', required: false)
      if brew.found()
        brew_gettext = run_command(brew, '--prefix', 'gettext', check: false)
        if brew_gettext.returncode() == 0
          intl_prefixes += brew_gettext.stdout().strip()
        endif
      endif
      intl_prefixes += [
        '/opt/homebrew/opt/gettext',
        '/usr/local/opt/gettext',
        '/opt/homebrew',
        '/opt/local', # MacPorts
        '/usr/local',
        ]
    endif

    foreach prefix : intl_prefixes
      args = ['-isystem' + prefix / 'include']
      candidate = cc.find_library(
        'intl',
        has_headers: ['libintl.h'],
        dirs: [prefix / 'lib'],
        header_args: args,
        required: false
        )
      if candidate.found()
        intl_dep = candidate
        intl_args = args
        break
      endif
    endforeach
  endif

  if intl_dep.found()
    # The include path has to reach the C++ sources too; libintl.h is pulled in
    # by src/Lib/gettext.h. Attaching it to the dependency covers every target
    # that uses it, in both languages.
    if intl_args.length() > 0
      intl_dep = declare_dependency(
        dependencies: intl_dep,
        compile_args: intl_args
        )
    endif
    deps += intl_dep
    conf.set('ENABLE_NLS', 1)
    if not find_program('msgfmt', required: false).found()
      warning('libintl was found but msgfmt was not, so the translations ' +
        'will not be built. Add the "bin" directory of your gettext ' +
        'installation to PATH, or add "-Dnls=false" to the setup options.')
    endif
    i18n = import('i18n')
    i18n.gettext(meson.project_name(),
      args: ['--directory=' + meson.project_source_root()])
  else
    message('libintl not found. You will either need to install the appropriate')
    message('package or add "-Dnls=false" to the setup/configure options.')
    error('')
  endif
endif
