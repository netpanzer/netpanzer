# Cross Compiling

General documentation for [cross-compiling with the Meson build
system](https://mesonbuild.com/Cross-compilation.html) can be read on their
site.

Remember to disable 'sanitize' when building for Windows (at some point we may
figure out how to efficiently add the correct libraries to add ASAN to Windows
builds).

## x86_64

### Windows

On Linux (or any other system but Windows, since it's a cross-build)

    meson setup --cross-file support/cross/x86_64-w64-mingw32.txt -Db_sanitize=none build-mingw

## aarch64 Linux

    meson setup --cross-file=support/cross/aarch64-linux-gcc.txt build-aarch64
