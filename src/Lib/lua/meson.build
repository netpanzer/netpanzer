src = [
  'print.c',
  'lstring.c',
  'ltablib.c',
  'loadlib.c',
  'lmathlib.c',
  'lstate.c',
  'ldump.c',
  'loslib.c',
  'liolib.c',
  'ldblib.c',
  'lbaselib.c',
  'lapi.c',
  'llex.c',
  'lopcodes.c',
  'lvm.c',
  'ltable.c',
  'linit.c',
  'lobject.c',
  'lstrlib.c',
  'lcode.c',
  'ldebug.c',
  'lauxlib.c',
  'ldo.c',
  'lfunc.c',
  'lzio.c',
  'ltm.c',
  'lgc.c',
  'lmem.c',
  'lparser.c',
  'lundump.c'
]

lib_lua = static_library(
  'embedded_lua',
  src,
)

dep_embedded_lua = declare_dependency(link_with: lib_lua)
