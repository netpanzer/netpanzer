[![Linux](https://github.com/netpanzer/netpanzer/actions/workflows/c-cpp.yml/badge.svg)](https://github.com/netpanzer/netpanzer/actions/workflows/c-cpp.yml)
[![MacOS](https://github.com/netpanzer/netpanzer/actions/workflows/macos.yml/badge.svg)](https://github.com/netpanzer/netpanzer/actions/workflows/macos.yml)
[![CodeQL](https://github.com/netpanzer/netpanzer/actions/workflows/codeql.yml/badge.svg)](https://github.com/netpanzer/netpanzer/actions/workflows/codeql.yml)
# NetPanzer
### Open Source RTS

## Introduction

NetPanzer is an online multiplayer tactical warfare game designed for FAST
ACTION combat. Gameplay concentrates on the core - no resource management is
needed. The game is based on quick tactical action and unit management in
real-time. Battles progress quickly and constantly as destroyed players respawn
with a set of new units. Players can join or leave multiplayer games at any
time.

## 20+ Years of Development

The original development of NetPanzer was done from PyroSoft inc. In
1999 they stopped development and released the game 2002 in GPL and it was nearly
forgotten, until Ingo announced it at happypenguin.org. Soon after this
announcement a group of coders did the linux port in roughly 2 weeks, with
another 2 weeks polishing for the first release. Since then development has picked up and slowed
down, with various different teams and maintainers. The current maintainer of the game and masterserver
is "winrid" on GitHub.

## To Play

Go to the Releases tab in GitHub and download the version for your OS. The version in Linux repos as of 2024
is very (8+yrs) old, so we suggest simply downloading the latest release directly.

Note: If you only are able to get v0.8.7 from your distribution, change the
'server.masterservers' value in your client.cfg and server.cfg to

    server.masterservers = "netpanzer.io"

One we release our next official version, this will be done automatically and the
newer version will be more accessible to you. **Do not leave the old master server in your config** or else the game will
crash, as 0.8.7 has a bug causing it to segfault if any servers are not available. We've fixed this in 0.9.0.

### Latest Download

[You can download the latest version here.](https://github.com/netpanzer/netpanzer/releases)

[Weekly Linux AppImage is available here.](https://github.com/netpanzer/netpanzer/releases/tag/appimage-weekly)

Most current servers (as of 2024) are running 0.9.0+, so this is what you want if you want to play on a public server.

## Contributing and Reporting Bugs

See [CONTRIBUTING.md](CONTRIBUTING.md)

## Gameplay

An extensive list of commands is available in the Help section of NetPanzer binary,
together with some useful tips and guides.

The same list is available while in game too - just press F1 key.

## Dedicated Server and Configuration Files

See
[SERVER-HOWTO.md](docs/SERVER-HOWTO.md)

## Building NetPanzer

See [BUILD.md](BUILD.md)

## Music

Netpanzer doesn't come with own musicfiles yet. However, you can place your
favourite .mp3, .ogg or .mod files into the sound/music directory and netpanzer
will play these in random order while you play.

## Contact

* [Discord](https://discord.gg/kCAB2CMswd)

It's important to remember that any discussions about bugs or desired features
in a chat room usually scroll away and become forgotten, so it's best to
create a ticket or discussion on GitHub (mentioned above) if you really want a
bug fix or new feature.

* [NetPanzer Discussions](https://github.com/netpanzer/netpanzer/discussions) on GitHub
* [Issues/Bug Tracker](https://github.com/netpanzer/netpanzer/issues)

### Unofficial

These areas are not moderated or maintained by the NetPanzer team. If looking
for information about the game, always be sure to verify with *official*
sources.

* [NetPanzer on Instagram](https://www.instagram.com/netpanzer/)
* [ChatPanzer on WhatsApp](https://chat.whatsapp.com/8CJHyZAzc4XKgbm4CkVpWJ)
* [Reddit](https://www.reddit.com/r/netpanzer)
