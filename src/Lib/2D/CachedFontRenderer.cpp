/*
Copyright (C) 2024 NetPanzer (https://github.com/netpanzer/), Devon Winrick, Et al.

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

#include <string>

#include "CachedFontRenderer.hpp"
#include "Util/Log.hpp"

const Uint32 cleanupThreshold = 20000;

TTF_Font *CachedFontRenderer::font = nullptr;
Uint32 CachedFontRenderer::lastCleanedTick = 0;
std::unordered_map <std::string, RenderedText> CachedFontRenderer::rendered_surfaces = {};

void CachedFontRenderer::initFont() {
    if (TTF_Init() < 0) {
        printf("Couldn't initialize SDL TTF: %s\n", SDL_GetError());
        exit(1);
    }
    // Quantico-Regular looked good too but some issues with some characters.
    CachedFontRenderer::font = TTF_OpenFont("fonts/GNUUnifont9FullHintInstrUCSUR.ttf", FONT_SIZE);
    TTF_SetFontStyle(CachedFontRenderer::font, TTF_STYLE_BOLD);
    TTF_SetFontHinting(CachedFontRenderer::font, TTF_HINTING_MONO);
}

std::string CachedFontRenderer::create_cache_key(const char *text, SDL_Color color) {
    return std::string(text) + std::to_string(color.r) +
           std::to_string(color.g) + std::to_string(color.b);
}

SDL_Surface *CachedFontRenderer::render(const char *text, SDL_Color color) {
    std::string key = create_cache_key(text, color);
    auto it = rendered_surfaces.find(key);
    if (it != rendered_surfaces.end()) {
        // Return the cached surface
        it->second.lastUsedTick = SDL_GetTicks();
        return it->second.sdlSurface;
    }

    // If not found, render the text
    SDL_Surface *rendered_surface = TTF_RenderUTF8_Solid(CachedFontRenderer::font, text, color);
    if (rendered_surface) {
        // Store the rendered surface in the cache
        RenderedText text(rendered_surface, SDL_GetTicks());
        rendered_surfaces[key] = text;
    }

    return rendered_surface;
}

void CachedFontRenderer::cleanup() {
    const Uint32 currentTick = SDL_GetTicks();

    if (currentTick - lastCleanedTick < cleanupThreshold) {
        return;
    }

    LOGGER.debug("Cached font cleanup: Begin.");

    // Iterate through the map to remove old RenderedText objects
    for (auto it = rendered_surfaces.begin(); it != rendered_surfaces.end();) {
        Uint32 lastUsedTick = it->second.lastUsedTick;
        if (currentTick - lastUsedTick > cleanupThreshold) {
            SDL_FreeSurface(it->second.sdlSurface); // Free SDL surface memory
            it = rendered_surfaces.erase(it); // Remove the entry from the map
        } else {
            ++it;
        }
    }
    lastCleanedTick = currentTick;
    LOGGER.debug("Cached font cleanup: Done.");
}
