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

#include <SDL2/SDL.h>
#include "Util/Log.hpp"
#include <string.h>
#include <unordered_set>
#include <deque>

class KeyboardInterface
{
protected:
    static std::unordered_set<int> key_table;
    static std::unordered_set<int> previous_key_state;
    static bool textmode;
    static std::deque<int> char_buffer;

public:
    static void sampleKeyboard()
    {
        previous_key_state = key_table;
    }

    static void keyPressed(int scancode) { key_table.insert(scancode); }
    static void keyReleased(int scancode){ key_table.erase(scancode);}

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
        return key_table.count(scan_code);
    }

    static inline bool getPrevKeyState(int scan_code)
    {
        return previous_key_state.count(scan_code);
    }

    static inline void flushCharBuffer()
    {
        char_buffer.clear();
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
        if (char_buffer.empty()) {
            return false;
        }

        c = char_buffer.front();
        char_buffer.pop_front();

        return true;
    }

    static inline void putChar(int c)
    {
        char_buffer.push_back(c);
    }
};

#endif // ** _KEYBOARDINTERFACE_HPP
