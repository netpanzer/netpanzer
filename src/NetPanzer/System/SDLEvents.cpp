/*
   Copyright (C) 2004 Matthias Braun <matze@braunis.de>,
 
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


#include <ctype.h>
#include <SDL2/SDL.h>
#include "Interfaces/KeyboardInterface.hpp"
#include "Interfaces/MouseInterface.hpp"
#include "SDLVideo.hpp"
#include "2D/Palette.hpp"
#include "Interfaces/GameConfig.hpp"
#include "Interfaces/GameManager.hpp"
#include "Util/Log.hpp"

bool handleSDLEvents() {
    static SDL_Event event;
    KeyboardInterface::sampleKeyboard();
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
                return true;
                break;
            case SDL_MOUSEBUTTONDOWN:
                MouseInterface::onMouseButtonDown(&event.button);
                break;
            case SDL_MOUSEBUTTONUP:
                MouseInterface::onMouseButtonUp(&event.button);
                break;
            case SDL_MOUSEMOTION:
                MouseInterface::onMouseMoved(&event.motion);
                break;
            case SDL_KEYDOWN: {
//                LOGGER.info("Pressed key : scancode[%d] keycode[%d]", event.key.keysym.scancode, event.key.keysym.sym);
//                printf("Pressed key : scancode[%d] keycode[%d]\n", event.key.keysym.scancode, event.key.keysym.sym);
                KeyboardInterface::keyPressed(event.key.keysym.sym);

                if ((event.key.keysym.sym & 0xFF80) == 0) {
                    SDL_Keycode c = event.key.keysym.sym;
                    if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == ' ') {
                        KeyboardInterface::putChar(c);
                    } else {
                        // extended chars, first push a 0
                        KeyboardInterface::putChar(0);
                        KeyboardInterface::putChar(c);
                    }
                } else {
                    // international character ignored for now
                }

                break;
            }
            case SDL_KEYUP:
//                LOGGER.debug("Released key : scancode[%d] keycode[%d]", event.key.keysym.scancode, event.key.keysym.sym);
                KeyboardInterface::keyReleased(event.key.keysym.sym);
                break;
        }
    }

    return false;
}
