src = []

src_basename = [
  'Bot.cpp',
  'BotPlayer.cpp',
  'BotTaskList.cpp'
]

foreach file : src_basename
    src += [join_paths('Bot', file)]
endforeach

src_basename = [
  'ObjectiveInterface.cpp',
  'Objective.cpp',
]
foreach file : src_basename
    src += [join_paths('Objectives', file)]
endforeach

src_basename = [
  'ProxyServer.cpp',
  'ClientSocket.cpp'
]
foreach file : src_basename
    src += [join_paths('Network', file)]
endforeach

src_basename = [
  'ClientConnectDaemon.cpp',
  'ServerMessageRouter.cpp',
  'NetworkClient.cpp',
  'NetworkState.cpp',
  'NetworkServer.cpp',
  'NetworkInterface.cpp',
  'NetMessageEncoder.cpp',
  'NetPacketDebugger.cpp',
  'NetMessageDecoder.cpp',
  'ConnectNetMessage.cpp',
  'NetPacketQueues.cpp',
  'ClientMessageRouter.cpp',
  'ServerConnectDaemon.cpp',
]
foreach file : src_basename
    src += [join_paths('Classes/Network', file)]
endforeach

dep_sdl2 = dependency('SDL2')

# SDL2 is case sensitive for the ttf library
dep_sdl2_ttf = dependency('SDL2_ttf', required: false)

inc_shared_lib_misc_basename = [
  'Bot',
]

inc_shared_lib_misc = []

#foreach file : inc_shared_lib_misc_basename
#    inc_shared_lib_misc += [join_paths('include', file)]
#endforeach

lib_netpanzer = library(
  'netpanzer',
  src,
  include_directories : [
    inc_shared_lib_misc_basename,
    inc_dirs
  ],
  dependencies: [
    dep_sdl2,
    dep_sdl2_ttf
  ]
)

dep_netpanzer = declare_dependency(link_with: lib_netpanzer)
