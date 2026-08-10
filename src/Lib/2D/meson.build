deps_np2D = [sdl2_dep, sdl2_ttf_dep, nputil_dep, npscript_dep]

lib_np2D = library (
  'np2D',
  [
    'CachedFontRenderer.cpp',
    'Color.cpp',
    'ColorTable.cpp',
    'PackedSurface.cpp',
    'Palette.cpp',
    'Span.cpp',
    'Surface.cpp'
    ],
  include_directories: inc_dirs,
  dependencies: deps_np2D,
  install: false
  )

np2D_dep = declare_dependency(
  link_with: lib_np2D,
  include_directories: inc_dirs,
  dependencies: deps_np2D
  )

deps += np2D_dep
