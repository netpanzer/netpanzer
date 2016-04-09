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
#ifndef _KEYBOARDINTERFACE_HPP
#define _KEYBOARDINTERFACE_HPP

#include "SDL.h"
#include "Util/Log.hpp"
#include <string.h>

#define _CHAR_BUFFER_SIZE 256
#define _CHAR_BUFFER_MOD  255

class KeyboardInterface
{
protected:
    static bool key_table[SDLK_LAST];
    static bool previous_key_state[SDLK_LAST];
    static bool textmode;
    static int char_buffer[ _CHAR_BUFFER_SIZE ];
    static unsigned long char_buffer_front;
    static unsigned long char_buffer_rear;

public:
    static void clearKeyTable()
    {
        memset(key_table, 0, sizeof(key_table));
        memset(previous_key_state, 0, sizeof(previous_key_state));
    }

    static void sampleKeyboard()
    {
        memcpy(previous_key_state, key_table, sizeof(key_table));
    }

    static void keyPressed(int scancode) { key_table[scancode] = true; }
    static void keyReleased(int scancode){ key_table[scancode] = false;}

    static bool getKeyPressed(int scanCode)
    {
        return ! textmode
               && KeyboardInterface::getKeyState(scanCode)
               && ! KeyboardInterface::getPrevKeyState(scanCode);
    }

    static bool getKeyReleased(int scanCode)
    {
        return ! textmode
                && ! KeyboardInterface::getKeyState(scanCode)
                && KeyboardInterface::getPrevKeyState(scanCode);
    }

    static inline bool getKeyState(int scan_code)
    {
        return key_table[scan_code];
    }

    static inline bool getPrevKeyState(int scan_code)
    {
        return previous_key_state[scan_code];
    }

    static inline void flushCharBuffer()
    {
        char_buffer_front = 0;
        char_buffer_rear = 0;
    }

    /** enable text input mode, which has the effect that getKeyPressed
     * returns always false, as long as it is activated. Only getChar returns
     * the really pressed keys.
     */
    static inline void setTextMode(bool enabled)
    {
	textmode = enabled;
    }

    static inline bool getChar(int &c)
    {
        if ( char_buffer_front == char_buffer_rear )
            return false;

        char_buffer_front = ( char_buffer_front + 1 ) & _CHAR_BUFFER_MOD;
        c = char_buffer[ char_buffer_front ];

        return true;
    }
   
    static inline void putChar(int c)
    {
        char_buffer[ (char_buffer_rear + 1) & _CHAR_BUFFER_MOD ] = c;
        char_buffer_rear = (char_buffer_rear + 1) & _CHAR_BUFFER_MOD;
    }
};

#endif // ** _KEYBOARDINTERFACE_HPP
