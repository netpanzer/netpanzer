/*
Copyright (C) 1998 Pyrosoft Inc. (www.pyrosoftgames.com), Matthew Bogue

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#include "SDLVideo.hpp"

#include <stdlib.h>
#include <time.h>

#include <iostream>
#include <string>

#include "2D/Color.hpp"
#include "Interfaces/ConsoleInterface.hpp"
#include "Util/Exception.hpp"
#include "Util/FileSystem.hpp"
#include "Util/Log.hpp"
#include "Util/NTimer.hpp"
#include "Util/Log.hpp"
#include "package.hpp"

#if defined _WIN32 || defined __MINGW32__

#include "Interfaces/GameConfig.hpp"

#endif

SDLVideo *Screen;  // get rid of this later...

SDLVideo::SDLVideo() : window(0) {
#if defined _WIN32
#if not defined __MINGW32__
  if (GameConfig::video_usedirectx) {
    putenv("SDL_VIDEODRIVER=directx");
  }
#endif
#endif
  this->window = nullptr;
  this->renderer = nullptr;
  this->surface = nullptr;
  this->texture = nullptr;
  this->is_fullscreen = false;
  if (SDL_InitSubSystem(SDL_INIT_VIDEO)) {
    throw Exception("Couldn't initialize SDL_video subsystem: %s",
                    SDL_GetError());
  }
}

SDLVideo::~SDLVideo() {
  if (texture != nullptr) {
    SDL_DestroyTexture(texture);
  }
  if (surface != nullptr) {
    SDL_FreeSurface(surface);
  }
  if (renderer != nullptr) {
    SDL_DestroyRenderer(renderer);
  }
  if (window != nullptr) {
    SDL_DestroyWindow(window);
  }

  SDL_QuitSubSystem(SDL_INIT_VIDEO);
}

bool SDLVideo::setVideoMode(int new_width, int new_height, int bpp,
                            bool fullscreen) {
  const bool was_fullscreen = this->is_fullscreen;
  this->is_fullscreen = fullscreen;

  if (window == nullptr) {
    LOGGER.debug("Creating new window.");
    if (fullscreen) {
      // use the native desktop resolution, and scale linearly later using
      // renderer
      window = SDL_CreateWindow(
          Package::GetFullyQualifiedName().c_str(), SDL_WINDOWPOS_UNDEFINED,
          SDL_WINDOWPOS_UNDEFINED, 0, 0, SDL_WINDOW_FULLSCREEN_DESKTOP);
    } else {
      window = SDL_CreateWindow(
          Package::GetFullyQualifiedName().c_str(), SDL_WINDOWPOS_UNDEFINED,
          SDL_WINDOWPOS_UNDEFINED, new_width, new_height, 0);
    }
    if (window == nullptr) {
        LOGGER.warning("Couldn't create a window: %s", SDL_GetError());
        return false;
    }

    LOGGER.debug("Showing window.");
    SDL_ShowWindow(window); // has to happen before fullscreen switch to fix cursor stuck in region issue
    SDL_RaiseWindow(window);

    LOGGER.debug("Creating new renderer.");
    renderer = SDL_CreateRenderer(window, -1, 0);

    if (renderer == nullptr) {
        LOGGER.warning("Couldn't create renderer: %s", SDL_GetError());
        return false;
    }
  } else {
    LOGGER.debug("Showing window.");
    SDL_ShowWindow(window);  // has to happen before fullscreen switch to fix
    // cursor stuck in region issue
    SDL_RaiseWindow(window);
    if (fullscreen) {
      if (was_fullscreen) {
        // no change
      } else {
        LOGGER.debug("Setting fullscreen.");
        const int setFullscreenResult = SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP);
        if (setFullscreenResult < 0) {
            LOGGER.warning("Could not set fullscreen: %s", SDL_GetError());
        }
      }
    } else {
      if (was_fullscreen) {
        LOGGER.debug("Disabling fullscreen.");
        const int disableFullScreenResult = SDL_SetWindowFullscreen(window, 0);
        if (disableFullScreenResult < 0) {
            LOGGER.warning("Could not disable fullscreen: %s", SDL_GetError());
        }
      }
      LOGGER.debug("Setting window size.");
      SDL_SetWindowSize(window, new_width, new_height);
    }
  }

  if (surface != nullptr) {
    LOGGER.debug("Cleaning up old surface.");
    SDL_FreeSurface(surface);
  }

  LOGGER.debug("Creating surface.");
  surface = SDL_CreateRGBSurfaceWithFormat(0, new_width, new_height, 8,
                                           SDL_PIXELFORMAT_INDEX8);

  if (surface == nullptr) {
    LOGGER.warning("Couldn't create render surface: %s", SDL_GetError());
    return false;
  }

  if (texture != nullptr) {
    LOGGER.debug("Destroying old texture.");
    SDL_DestroyTexture(texture);
  }

  LOGGER.debug("Creating new render texture.");
  texture = SDL_CreateTextureFromSurface(renderer, surface);

  if (texture == nullptr) {
    LOGGER.warning("Couldn't create render texture: %s", SDL_GetError());
    return false;
  }

  // make the scaled rendering look smoother.
  // Note: "linear" made game look blurry when game resolution did not match
  // monitor resolution.
  LOGGER.debug("Setting render hints.");
  SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "nearest");
  LOGGER.debug("Setting render logical size.");
  const int setLogicalSizeResult = SDL_RenderSetLogicalSize(renderer, new_width, new_height);
  if (setLogicalSizeResult < 0) {
    LOGGER.warning("Couldn't set logical resolution: %d %d %s", new_width, new_height, SDL_GetError());
  }

  // let's scare the mouse :)
  // this fixes the mouse cursor stuck to a small region after resolution change
  LOGGER.debug("Showing cursor.");
  const int showCursorResult = SDL_ShowCursor(SDL_DISABLE);
  if (showCursorResult < 0) {
    LOGGER.warning("Couldn't show cursor: %s", SDL_GetError());
    // can still try to continue
  }

  // Center the mouse after changing the resolution - also helps mouse cursor
  // from getting stuck in old region
  int centerX = new_width / 2;
  int centerY = new_height / 2;
  LOGGER.debug("Warping mouse into window...");
  SDL_WarpMouseInWindow(window, centerX, centerY);
  SDL_SetWindowGrab(window, fullscreen ? SDL_TRUE : SDL_FALSE);
  return true;
}

void SDLVideo::setPalette(SDL_Color *color) {
  SDL_SetPaletteColors(surface->format->palette, color, 0, 256);
}

SDL_Surface *SDLVideo::getSurface() { return surface; }
SDL_Window *SDLVideo::getWindow() { return window; }

void SDLVideo::render() {
  // This mechanism is only about 5-10% slower than SDL_BlitSurface &&
  // SDL_UpdateWindowSurface. But, it gets us a lot (simpler code, much nicer
  // rendering and scaling).
  if (texture != nullptr) {
    SDL_DestroyTexture(texture);
  }
  texture = SDL_CreateTextureFromSurface(renderer, surface);
  SDL_RenderClear(renderer);
  SDL_RenderCopy(renderer, texture, nullptr, nullptr);
  SDL_RenderPresent(renderer);
}

void SDLVideo::doScreenshot() {
  // this is called blind faith
  static NTimer timer(1000);

  if (!timer.isTimeOut()) {
    return;
  }

  filesystem::mkdir("screenshots");

  char buf[256];
  time_t curtime = time(0);
  struct tm *loctime = localtime(&curtime);
  strftime(buf, sizeof(buf), "screenshots/%Y%m%d_%H%M%S.bmp", loctime);

  std::string bmpfile = filesystem::getRealWriteName(buf);
  SDL_SaveBMP(surface, bmpfile.c_str());
  ConsoleInterface::postMessage(Color::cyan, false, 0,
                                "Screenshot saved as: %s", buf);
  timer.reset();
}
