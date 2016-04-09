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
#ifndef __mMouseEvent_hpp__
#define __mMouseEvent_hpp__

#include "Component.hpp"
#include "Types/iXY.hpp"
#include "InputEvent.hpp"

class mMouseEvent : public InputEvent
{
private:
    Component *source;       // The Component that originated the event.
    int        id;           // The integer that identifies the event.
    iXY        pos;          // The coordinate where the event occured.
    int        clickCount;   // The number of mouse clicks associated with the event.
    bool       popupTrigger; // True if this event is a trigger for a popup-menu.

public:
    enum
    {
        MOUSE_EVENT_CLICKED,
        MOUSE_EVENT_DRAGGED,
        MOUSE_EVENT_ENTERED,
        MOUSE_EVENT_EXITED,
        MOUSE_EVENT_MOVED,
        MOUSE_EVENT_PRESSED,
        MOUSE_EVENT_RELEASED
    };

    mMouseEvent(Component *source, int id, TimeStamp when, int modifiers, int x, int y, int clickCount, bool popupTrigger)
    {
        mMouseEvent::source       = source;
        mMouseEvent::id           = id;
        mMouseEvent::when         = when;
        mMouseEvent::modifiers    = modifiers;
        mMouseEvent::pos.x        = x;
        mMouseEvent::pos.y        = y;
        mMouseEvent::clickCount   = clickCount;
        mMouseEvent::popupTrigger = popupTrigger;
    }

    int getID() const
    {
        return id;
    }

    // Returns the horizontal x position of the event relative to the source component.
    int getX() const
    {
        return pos.x;
    }

    // Returns the vertical y position of the event relative to the source component.
    int getY() const
    {
        return pos.y;
    }

    // Returns the x,y position of the event relative to the source component.
    const iXY &getPoint() const
    {
        return pos;
    }

    // Translates the event's coordinates to a new position by adding the specified
    // x (horizontal) and y (vertical) offsets.
    void translatePoint(int x, int y)
    {
        pos.x += x;
        pos.y += y;
    }

    // Returns the number of mouse clicks associated with this event.
    int getClickCount() const
    {
        return clickCount;
    }

    // Returns whether or not this mouse event is a popup-menu trigger event for the platform.
    bool isPopupTrigger() const
    {
        return popupTrigger;
    }

    Component * getSource() const
    {
        return source;
    }

}
; // end mMouseEvent

#endif // __mMouseEvent_hpp__
