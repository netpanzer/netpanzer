inc_dir = [
  '2D',
  'Util'
]

inc_dirs = [include_directories('Lib')]

foreach file : inc_dir
    inc_dirs += include_directories(join_paths('Lib', file))
endforeach

subdir('NetPanzer')


