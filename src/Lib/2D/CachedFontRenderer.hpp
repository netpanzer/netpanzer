/*
Copyright (C) 2024 NetPanzer (https://github.com/netpanzer/), Devon Winrick, Et
al.

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

#ifndef NETPANZER_FONTPOOL_HPP
#define NETPANZER_FONTPOOL_HPP

#include <SDL.h>
#include <SDL_ttf.h>

#include <string>
#include <unordered_map>

#define FONT_WIDTH 14
class RenderedText {
 public:
  SDL_Surface* sdlSurface;
  Uint32 lastUsedTick;

  RenderedText() : sdlSurface(nullptr), lastUsedTick(0) {}
  RenderedText(SDL_Surface* surface, Uint32 tick)
      : sdlSurface(surface), lastUsedTick(tick) {}
};

class CachedFontRenderer {
 private:
  static Uint32 lastCleanedTick;
  static std::unordered_map<std::string, RenderedText> rendered_surfaces;
  static std::string create_cache_key(const char* text, SDL_Color color);
  static TTF_Font* font;

 public:
  static void initFont();
  static SDL_Surface* render(const char* text, SDL_Color color);
  static void cleanup();

  static void test_openFont(void);
};

#endif  // NETPANZER_FONTPOOL_HPP
