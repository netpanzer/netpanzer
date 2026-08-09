# NetPanzer testing

Unit tests are added to the source files where the functions are defined. For
an example, see
[src/Lib/package.cpp](https://github.com/netpanzer/netpanzer/blob/master/src/Lib/package.cpp)

If the source file to which you are adding a test doesn't already include a
test, add the filename to tests/meson.build.

For detailed information about testing with meson, refer to the [Meson unit
tests documentions](https://mesonbuild.com/Unit-tests.html#unit-tests).

The tests are run from the build directory.

To run all tests:

    meson test --suite=netpanzer

Add `-v` to get more output.

View available options:

    meson help test
