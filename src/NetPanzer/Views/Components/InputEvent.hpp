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

#ifndef __InputEvent_hpp__
#define __InputEvent_hpp__

#include "Util/TimeStamp.hpp"

class InputEvent
{
protected:
    TimeStamp  when;         // The time the event occured.
    int        modifiers;    // The modifier keys down during the event (shift, ctrl, alt, meta).

public:
    enum { ALT_GRAPH_MASK = (1U << 0) };
    enum { ALT_MASK       = (1U << 1) };
    enum { BUTTON1_MASK   = (1U << 2) };
    enum { BUTTON2_MASK   = (1U << 3) };
    enum { BUTTON3_MASK   = (1U << 4) };
    enum { CTRL_MASK      = (1U << 5) };
    enum { META_MASK      = (1U << 6) };
    enum { SHIFT_MASK     = (1U << 7) };

    InputEvent()
    {}
    ~InputEvent()
    {}

    inline int       isShiftDown()
    {
        return modifiers & SHIFT_MASK;
    }
    inline int       isControlDown()
    {
        return modifiers & CTRL_MASK;
    }
    inline int       isMetaDown()
    {
        return modifiers & META_MASK;
    }
    inline int       isAltDown()
    {
        return modifiers & ALT_MASK;
    }
    inline int       isAltGraphDown()
    {
        return modifiers & ALT_GRAPH_MASK;
    }
    inline TimeStamp getWhen() const
    {
        return when;
    }
    inline int       getModifiers() const
    {
        return modifiers;
    }

}
; // end InputEvent

#endif // __InputEvent_hpp__
