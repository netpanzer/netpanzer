deps_npclassnet = [sdl2_dep]

lib_npclassnet = library (
  'npclassnet',
  [
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
    'ServerConnectDaemon.cpp'
    ],
  include_directories: inc_dirs,
  dependencies: deps_npclassnet,
  install: false
  )

npclassnet_dep = declare_dependency(
  link_with: lib_npclassnet,
  include_directories: inc_dirs,
  dependencies: deps_npclassnet
  )

deps += npclassnet_dep
