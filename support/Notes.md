## Testing data installation

    meson configure -Dprefix=$HOME/.local

That will rewrite config.h in the build directory. NP_DATADIR will be defined
there. Check it if desired.

    ninja (Rebuild)
    meson install --skip-subprojects

At this point, you should be able to run 1./netpanzer` from the build
directory without using `meson devenv`. In fact, you may want to make sure
that the environmental variable NETPANZER_DATADIR is not set, otherwise that
will interfere with the test results.

You should also be able to go to `$HOME/.local/bin` and run `./netpanzer` from
there, or if `$HOME/.local/bin` is first in your PATH, you can run
`netpanzer'.

    ninja uninstall

Uninstall everything so there won't be anything left over the next time you
need to test installation.
