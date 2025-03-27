## Building NetPanzer

The game depends on some tools and libraries to be present, before you can start
building it. Here's a list of them:

- A compiler that supports C++17
- [The Meson Build System](https://mesonbuild.com/)
- lua5.1-dev
- physfs-dev
- Python3 (for build scripts)
- SDL2
  http://www.libsdl.org
- SDL2_ttf
  https://wiki.libsdl.org/SDL2_ttf/FrontPage
- SDL2_mixer
  http://www.libsdl.org/projects/SDL_mixer/

- [gettext](https://www.gnu.org/software/gettext/) (If you want to install
translations, otherwise, use '-Dnls=false' when setting up the build).

Note: some dependencies may have different names depending on your OS and
package manager, and commonly have a '-dev' or '-devel' suffix. If the
dependencies aren't available from your package manager, meson will download
and install them when the build starts.

Thanks to all the authors of these helpful libraries that made our development
easy and straightforward.

Instructions for installing `meson` are on their website (see link above).

The general usage for meson can be found in the meson docs. Some options are
needed for this project and we provide convenience scripts to set up the build
directory. Depending on your operating system, use one of the following:

    setup-build.sh
    setup-build.bat

followed by the target build directory, which can be named anything you want.
Example:

    setup-build.sh _build

After meson has configured your build directory, change to it, then run

    meson devenv

You only need to do that once in the terminal you're working in. That sets the
environmental variable `NETPANZER_DATADIR` to the source root. When you build
netpanzer, the binary will be output to your build directory. You can run it
from there, and it will find the data in the parent directory.

To build netpanzer:

    ninja

Add '-v' for more output.

Note: `ninja` uses all cores by default. If you wish to set it manually use
`-j n' (where 'n' is the number of desired cores).

The scripts take the same options as `meson setup`, which you can view by:

    meson help setup

## Cross-Compiling

See [support/cross/README.md](support/cross/README.md)

## Installing

Common options to use before installing netpanzer:

    -Dprefix=/usr

This will install the data to '/usr/share/netpanzer' and the binary to '/usr/bin'.

To package or test installation, use the 'destdir' option', e.g.:

    meson install --destdir=/tmp/pkg --skip-subprojects

Note: If 'destdir' is omitted, the files will be installed to '<prefix>'.

As you gain more understanding of the build system, you might want to see
various build-time configuration options. in the build directory, try:

    meson configure

## Release

Common options to give to 'configure' before creating a release or before
installing:

    -Dbuildtype=release
    -Db_sanitize=none
    -Db_prefix=/usr (on Windows this will be different)

## Tests

See [tests/README.md](tests/README.md)
