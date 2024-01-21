## Building NetPanzer

The game depends on some tools and libraries to be present, before you can start
building it. Here's a list of them:

- A compiler that supports C++17
- Python3 (for build scripts)
- SDL 2
  http://www.libsdl.org
- SDL 2 TTF
  https://wiki.libsdl.org/SDL2_ttf/FrontPage
- SDL_mixer 2
  http://www.libsdl.org/projects/SDL_mixer/

Thanks to all the authors of these helpful libraries that made our development
easy and straightforward.

We are in the process of deprecating the [Scons](https://scons.org/) build
system in favor of [Meson](https://mesonbuild.com/). You still may use either
one (or both). Note that our meson build doesn't yet support Windows.

## Meson

Instructions for installing `meson` are on their website (see link above).

Install the `physfs` library (required by NetPanzer).

The general usage for meson can be found in the meson docs
Here are the basics:

    meson setup _build

To see options that can be used with 'setup', use `meson help setup`.

The third argument, the build dir, can be named anything you like. Sometimes
you might want to have more than one, depending on how you configure your
build. For example:

    meson configure -Db_sanitize=none

After meson has configured your build directory, change to it, then run

    meson devenv

You only need to do that once in the terminal you're working in. That sets the
environmental variable `NETPANZER_DATADIR` to the source root. When you build
netpanzer, the binary will be output to your build directory. You can run it
from there, and it will find the data in the parent directory.

To see various build-time configuration options, use:

    meson configure

To build netpanzer:

    ninja

Add '-v' for more output.

Note: `ninja` uses all cores by default. If you wish to set it manually use
`-j n' (where 'n' is the number of desired cores).

To see other meson command line options, use

    meson help

To install netpanzer, use `meson configure -Ddatadir=<data-dest-dir> -Dbindir=<bin-dest-dir>`, where destdir
could be similar to the following:

    /usr/share/netpanzer
    $HOME/.local/share/netpanzer

and bin dir could be one of these:

    /usr/bin
    $HOME/.local/bin

Then use

    ninja install

For packaging, or to test installation, use `DESTDIR`, e.g.:

    DESTDIR=/tmp/pkg ninja install

## Scons

To build:

    scons

Optionally, if you have multiple cores, you can speed up the build:

    scons -j n  (Where 'n' is the number of cores you would like to specify)

When scons finish without errors the game is ready to play, simply run the binary:

    ./netpanzer

### Building on Windows

Building on Windows is a little bit more complicated.

MinGW should be installed in C:\mingw

Setup all SDL2 `lib` and `include`'s for `mingw`.

Python and scons should be installed and working.

To compile netpanzer run scons on the folder where you have the netpanzer source
code:

    C:\Some\Folder\With\Netpanzer> scons

**After scons finish without errors you have to copy all the .DLL files from SDL
and from SDL_mixer to the netpanzer folder.**

To run you can type 'netpanzer' or click on the netpanzer.exe file in explorer.

### Building for distribution on non Windows operating systems

If you plan to distribute NetPanzer for installation (on Linux systems, for example)
there are some extra steps to be taken care.

NetPanzer needs to know where the data files will be stored when the game runs. To
set this data you have to add the parameter "datadir=/some/dir/" at compile time
and NetPanzer will look there for its data files. Example:

    scons datadir=/usr/local/games/netpanzer

Using the sample line, after NetPanzer is built, it will look in /usr/local/games/netpanzer
for its data files.

For versioning, there is a file called "RELEASE_VERSION", the first line on that file will
be the version that NetPanzer will show. You can use that file to change the version of
the game shown. You should modify the file before compilation.

Actually there is no install script, you will have to prepare the script by yourself.
The installation is actually easy, and you only have to copy several files. The following
directories should be copied on the "datadir" destination:
- cache/
- fonts/
- maps/
- pics/
- powerups/
- scripts/
- sound/
- units/
- wads/

If you want to copy some documentation, the "docs" folder has it. There is also the COPYING
 and the README (this file).

Inside "support" folder there are many useful scripts and resources.

"netpanzer.png" and "netpanzer.xpm" are some sample icons to be used with the game.

And of course, the file "netpanzer" is the main binary that should be on the PATH.

## Tests

See [tests/README.md](tests/README.md)

There are no tests configured yet for the NetPanzer scons build.
