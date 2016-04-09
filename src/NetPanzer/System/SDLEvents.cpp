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
#include "SDL.h"
#include "Interfaces/KeyboardInterface.hpp"
#include "Interfaces/MouseInterface.hpp"
#include "SDLVideo.hpp"
#include "2D/Palette.hpp"
#include "Interfaces/GameConfig.hpp"
#include "Interfaces/GameManager.hpp"
#include "Util/Log.hpp"

static SDLKey key_ascii_conversion[] =
{
    SDLK_a, SDLK_b, SDLK_c, SDLK_d, SDLK_e, SDLK_f, SDLK_g, SDLK_h, SDLK_i,
    SDLK_j, SDLK_k, SDLK_l, SDLK_m, SDLK_n, SDLK_o, SDLK_p, SDLK_q, SDLK_r,
    SDLK_s, SDLK_t, SDLK_u, SDLK_v, SDLK_w, SDLK_x, SDLK_y, SDLK_z
};

static SDLKey translate_key(SDL_keysym *keysym)
{
    SDLKey key = keysym->sym;
    switch( key )
    {
        case SDLK_AMPERSAND:  return SDLK_1;
        case SDLK_WORLD_73:   return SDLK_2;
        case SDLK_QUOTEDBL:   return SDLK_3;
        case SDLK_QUOTE:      return SDLK_4;
        case SDLK_LEFTPAREN:  return SDLK_5;
        case SDLK_MINUS:      return SDLK_6;
        case SDLK_WORLD_72:   return SDLK_7;
        case SDLK_UNDERSCORE: return SDLK_8;
        case SDLK_WORLD_71:   return SDLK_9;
        case SDLK_WORLD_64:   return SDLK_0;
        default:;
    }

    if ( key >= SDLK_a && key <= SDLK_z && (keysym->unicode & 0xFF80) == 0 )
    {
        SDLKey unicode = (SDLKey)(keysym->unicode & 0x7F);
        if ( unicode >= 'a' && unicode <= 'z' )
        {
            key_ascii_conversion[key-SDLK_a] = unicode;
            return unicode;
        }
        else if ( unicode >= 'A' && unicode <= 'Z' )
        {
            unicode = (SDLKey)(unicode + ('a' - 'A'));
            key_ascii_conversion[key-SDLK_a] = unicode;
            return unicode;
        }
        else if ( unicode >= 1 && unicode <= 26 )
        {
            unicode = (SDLKey)(unicode + ('a' - 1));
            key_ascii_conversion[key-SDLK_a] = unicode;
            return unicode;
        }

        return key_ascii_conversion[key-SDLK_a];
    }

    return key;
}

bool handleSDLEvents()
{
    static SDL_Event event;

    KeyboardInterface::sampleKeyboard();
    while(SDL_PollEvent(&event)) {
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
//                LOGGER.debug("Pressed key : scancode[%d] unicode[%d]", event.key.keysym.sym, event.key.keysym.unicode);
                KeyboardInterface::keyPressed(translate_key(&event.key.keysym));

                if ( (event.key.keysym.unicode & 0xFF80) == 0 )
                {
                  char c = event.key.keysym.unicode & 0x7F;
                  if ( isprint(c) )
                  {
                      KeyboardInterface::putChar(c);
                  }
                  else
                  {
                      // extended chars, first push a 0
                      KeyboardInterface::putChar(0);
                      KeyboardInterface::putChar(event.key.keysym.sym);
                  }
                }
                else
                {
                    // international character ignored for now
                }

                break;
            }
        case SDL_KEYUP:
//            LOGGER.debug("Released key: scancode[%d] unicode[%d]", event.key.keysym.sym, event.key.keysym.unicode);
            KeyboardInterface::keyReleased(translate_key(&event.key.keysym));
            break;
       
        case SDL_ACTIVEEVENT:
            if ( (event.active.state&SDL_APPACTIVE)
                 && (event.active.gain==1)
                 && GameConfig::video_fullscreen )
                Screen->setPalette(Palette::color);
             break;
        case SDL_VIDEORESIZE:
            GameConfig::video_width = event.resize.w;
            GameConfig::video_height = event.resize.h;
            GameManager::setVideoMode();
            break;
        }
    }

    return false;
}
