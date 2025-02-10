// If NDEBUG is defined as a macro name at the point in the source code where
// <cassert> or <assert.h> is included, the assertion is disabled: assert does
// nothing.
// https://en.cppreference.com/w/cpp/error/assert
#ifdef NDEBUG
#undefine NDEBUG
#endif
#include <cassert>

#include "Util/FileSystem.hpp"
#include "package.hpp"

// Maybe resolves "undefined reference to SDL_main"
#ifdef WIN32
#include <windows.h>
#endif
// Maybe Resolves "undefined reference to WinMain"
#include <SDL.h>
