// If NDEBUG is defined as a macro name at the point in the source code where
// <cassert> or <assert.h> is included, the assertion is disabled: assert does
// nothing.
// https://en.cppreference.com/w/cpp/error/assert
#ifdef NDEBUG
#undefine NDEBUG
#endif

#include <cassert>
