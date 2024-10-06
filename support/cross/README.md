# Cross Compiling

General documentation for [cross-compiling with the Meson build
system](https://mesonbuild.com/Cross-compilation.html) can be read on their
site.

Remember to disable 'sanitize' unless you have the corresponding [ASAN
libraries](https://github.com/google/sanitizers/wiki/AddressSanitizer)
installed.

Consult your OS distribution documentation for the compiler packages required
to cross-compile. Normally the Netpanzer dependencies won't be required
because the build will use the embedded sources if they are not found
installed on your system.

## x86_64

### Windows

If you are compiling on Ubuntu and the SDL2 build fails, you can try the
workaround we are using in [our
workflow](https://github.com/netpanzer/netpanzer/blob/master/.github/workflows/c-cpp.yml).

    meson setup --cross-file support/cross/x86_64-w64-mingw32.txt -Db_sanitize=none build-mingw

## aarch64

### Linux

    meson setup --cross-file=support/cross/aarch64-linux-gcc.txt -Db_sanitize=none build-aarch64
