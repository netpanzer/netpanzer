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
            case SDL_TEXTINPUT: {
                size_t text_length = strlen(event.text.text);

                for (size_t i = 0; i < text_length; i++) {
                    KeyboardInterface::putChar(event.text.text[i]);
                }
                break;
            }
            case SDL_KEYDOWN: {
//                LOGGER.info("Pressed key : scancode[%d] keycode[%d]", event.key.keysym.scancode, event.key.keysym.sym);
                KeyboardInterface::keyPressed(event.key.keysym.sym);

                SDL_Keycode c = event.key.keysym.sym;
                switch (c) {
                    // see cInputField
                    case SDLK_HOME:
                    case SDLK_LEFT:
                    case SDLK_RIGHT:
                    case SDLK_END:
                    case SDLK_INSERT:
                    case SDLK_DELETE:
                    case SDLK_BACKSPACE:
                    case SDLK_KP_ENTER:
                    case SDLK_RETURN:
                        // extended chars, first push a 0
                        KeyboardInterface::putChar(0);
                        KeyboardInterface::putChar(c);
                        break;
                    default:
                        // international character ignored for now
                        break;
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
