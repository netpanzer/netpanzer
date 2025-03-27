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

#ifndef TEST_LIB

#include "CachedFontRenderer.hpp"

#include <string>
#include <optional>

#include "Interfaces/MenuConfig.hpp"
#include "Util/FileSystem.hpp"
#include "Util/Log.hpp"

TTF_Font *CachedFontRenderer::font = nullptr;
Uint32 CachedFontRenderer::lastCleanedTick = 0;
std::unordered_map<std::string, RenderedText>
        CachedFontRenderer::rendered_surfaces = {};

void CachedFontRenderer::initFont() {
  if (TTF_Init() < 0) {
    LOGGER.warning("Couldn't initialize SDL TTF: %s\n", SDL_GetError());
    exit(EXIT_FAILURE);
  }

  std::string absFontPath = std::string(filesystem::getRealName((*MenuConfig::menu_font).c_str()));
  LOGGER.warning("font path: %s", absFontPath.c_str());
  font = TTF_OpenFont(absFontPath.c_str(), MenuConfig::menu_font_size);
  if (font == NULL) {
    LOGGER.warning("CachedFontRenderer - cannot load font %s.", absFontPath.c_str());
#ifndef TEST_LIB
    exit(EXIT_FAILURE);
#else
    return;
#endif
  }

  TTF_SetFontStyle(CachedFontRenderer::font, TTF_STYLE_BOLD);
  TTF_SetFontHinting(CachedFontRenderer::font, TTF_HINTING_MONO);
}

std::string CachedFontRenderer::create_cache_key(const char *text,
                                                 SDL_Color color,
                                                 SDL_Color blendColor,
                                                 bool wrapped,
                                                 int wrapLength) {
  std::string result = std::string(text);
  result += std::to_string(color.r) + std::to_string(color.g) + std::to_string(color.b);
  result += std::to_string(blendColor.r) + std::to_string(blendColor.g) + std::to_string(blendColor.b);
  result += std::to_string(wrapped) + std::to_string(wrapLength);
  return result;
}

SDL_Surface *CachedFontRenderer::render(const char *text, SDL_Color color, SDL_Color blendColor, bool wrapped, int wrapLength) {
  std::string key = create_cache_key(text, color, blendColor, wrapped, wrapLength);
  auto it = rendered_surfaces.find(key);
  if (it != rendered_surfaces.end()) {
    // Return the cached surface
    it->second.lastUsedTick = SDL_GetTicks();
    return it->second.sdlSurface;
  }

  // If not found, render the text
  SDL_Surface *rendered_surface = wrapped
          ? TTF_RenderUTF8_Shaded_Wrapped(CachedFontRenderer::font, text, color, blendColor, wrapLength)
          : TTF_RenderUTF8_Shaded(CachedFontRenderer::font, text, color, blendColor);
  if (rendered_surface) {
    // Store the rendered surface in the cache
    RenderedText rendered_text(rendered_surface, SDL_GetTicks());
    rendered_surfaces[key] = rendered_text;
  }

  return rendered_surface;
}

SDL_Surface *CachedFontRenderer::render(const char *text, SDL_Color color, SDL_Color blendColor) {
  return render(text, color, blendColor, false, 0);
}

SDL_Surface *CachedFontRenderer::renderWrapped(const char *text, SDL_Color color, SDL_Color blendColor, int wrapLength) {
  return render(text, color, blendColor, true, wrapLength);
}

void CachedFontRenderer::cleanup() {
  const Uint32 currentTick = SDL_GetTicks();
  const Uint32 cleanupThreshold = 20000;

  if (currentTick - lastCleanedTick < cleanupThreshold) {
    return;
  }

  LOGGER.debug("Cached font cleanup: Begin.");

  // Iterate through the map to remove old RenderedText objects
  for (auto it = rendered_surfaces.begin(); it != rendered_surfaces.end();) {
    Uint32 lastUsedTick = it->second.lastUsedTick;
    if (currentTick - lastUsedTick > cleanupThreshold) {
      SDL_FreeSurface(it->second.sdlSurface);  // Free SDL surface memory
      it = rendered_surfaces.erase(it);        // Remove the entry from the map
    } else {
      ++it;
    }
  }
  lastCleanedTick = currentTick;
  LOGGER.debug("Cached font cleanup: Done.");
}

#else

#include "test.hpp"
#include "CachedFontRenderer.hpp"
#include "Interfaces/MenuConfig.hpp"
#include "Scripts/ScriptManager.hpp"

void CachedFontRenderer::test_openFont(void) {
  CachedFontRenderer::initFont();
  assert(font != NULL);

  return;
}

int main(int argc, char *argv[]) {
  (void)argc;

  filesystem::initialize(argv[0], "test_CachedFontRenderer");
  Package::assignDataDir();
  filesystem::addToSearchPath(Package::getDataDir().c_str());

  ScriptManager::initialize();
  MenuConfig::loadConfig();
  CachedFontRenderer::test_openFont();

  return 0;
}

#endif
