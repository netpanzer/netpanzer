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
#ifndef _MOUSEINTERFACE_HPP
#define _MOUSEINTERFACE_HPP

#include <string>
#include <map>
#include <deque>

#include <SDL2/SDL.h>
#include "2D/Surface.hpp"
#include "Util/NTimer.hpp"

class MouseEvent
{
public:
    typedef enum {
        EVENT_DOWN = SDL_MOUSEBUTTONDOWN,
        EVENT_UP = SDL_MOUSEBUTTONUP
    } MouseEventType;
    unsigned char button;
    MouseEventType event;
    iXY pos;
};

typedef std::deque<MouseEvent> MouseEventQueue;

class MouseInterface
{
private:
    static Surface * cursor;
    static unsigned char cursor_x_size;
    static unsigned char cursor_y_size;
    static NTimer clicktimer;
    static int clickcount;
    static int releasecount;

    typedef std::map<std::string,Surface*> cursors_t;
    static cursors_t cursors;

protected:
    static iXY mouse_pos;

    static unsigned char button_mask;

public:
    enum {
        left_button   = SDL_BUTTON_LEFT,
        middle_button = SDL_BUTTON_MIDDLE,
        right_button  = SDL_BUTTON_RIGHT,
    };

    enum {
        left_button_mask   = SDL_BUTTON_LMASK,
        middle_button_mask = SDL_BUTTON_MMASK,
        right_button_mask  = SDL_BUTTON_RMASK
    };

    enum CursorType {
        defaultcursor, noentry, move, select, target, allie, break_allie
    };

    static MouseEventQueue event_queue;

    static void initialize();
    static void shutdown();

    static void draw(Surface &dest)
    {
        if (cursor) {
            cursor->nextFrame();
            cursor->bltTrans(dest, mouse_pos.x, mouse_pos.y);
        }
    }

    static inline void getMousePosition( int *x, int *y )
    {
        *x = mouse_pos.x;
        *y = mouse_pos.y;
    }

    static inline unsigned char getButtonMask()
    {
        return( button_mask );
    }

    static void onMouseButtonDown(SDL_MouseButtonEvent *e);
    static void onMouseButtonUp(SDL_MouseButtonEvent *e);
    static void manageClickTimer();

    static void setCursor(const char* cursorname);
    static inline void onMouseMoved(SDL_MouseMotionEvent *e)
    {
        mouse_pos.x = e->x;
        mouse_pos.y = e->y;
    }

    static inline iXY getMousePosition()   { return mouse_pos; }

    static inline unsigned int getMouseX() { return mouse_pos.x; }
    static inline unsigned int getMouseY() { return mouse_pos.y; }

};

#endif // ** _MOUSEINTERFACE_HPP
