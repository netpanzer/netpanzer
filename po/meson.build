intl_dep = []
if get_option('nls') and not platform_is_windows
  inc_args = []
  intl_dep = dependency('intl', required: false)
  if not intl_dep.found()
    if host_sys == 'freebsd' or host_sys == 'openbsd' or host_sys == 'dragonfly'
      inc_args = ['-isystem/usr/local/include']
      intl_dep = cc.find_library(
        'intl',
        has_headers: ['libintl.h'],
        dirs: ['/usr/local/lib'],
        header_args: inc_args,
        required: false
        )
    elif host_sys == 'netbsd'
      inc_args = ['-isystem/usr/pkg/include']
      intl_dep = cc.find_library(
        'intl',
        has_headers: ['libintl.h'],
        dirs: ['/usr/pkg/lib'],
        header_args: inc_args,
        required: false
        )
    else # darwin, cygwin
      intl_dep = cc.find_library(
        'intl',
        has_headers: ['libintl.h'],
        required: false
        )
    endif
  endif
  if intl_dep.found()
    deps += intl_dep
    add_project_arguments(inc_args, language : 'c')
    conf.set('ENABLE_NLS', 1)
    i18n = import('i18n')
    i18n.gettext(meson.project_name(),
      args: ['--directory=' + meson.project_source_root()])
  else
    message('libintl not found. You will either need to install the appropriate')
    message('package or add "-Dnls=false" to the setup/configure options.')
    error('')
  endif
endif

