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


#include "Views/Components/CheckBox.hpp"
#include "MouseEvent.hpp"
#include "Views/Components/StateChangedCallback.hpp"
#include "ViewGlobals.hpp"



void
CheckBox::render()
{

    surface.fill(componentBodyColor);
    Surface text;
    text.renderText( label.c_str(), forcedtextColor, 0);
    surface.FillRoundRect( iRect(0,0,14,14),2,  componentBodyColor);
    surface.RoundRect( iRect(1,1,12,12), 2, forcedtextColor);
    if (state) {
        surface.drawLine( 3, 6, 6, 9, forcedtextColor);
        surface.drawLine( 3, 7, 6, 10, forcedtextColor);
        surface.drawLine( 6, 9, 10, 3, forcedtextColor);
        surface.drawLine( 6, 10, 10, 4, forcedtextColor);
    }

    surface.bltString(14+4, (surface.getHeight()/2) - (text.getHeight()/2),label.c_str(), forcedtextColor);
    //text.blt( surface, 14+2, (surface.getHeight()/2) - (text.getHeight()/2) );
    dirty = false;
}

// actionPerformed
//---------------------------------------------------------------------------
void CheckBox::actionPerformed(const mMouseEvent &me)
{
    if (	me.getID() == mMouseEvent::MOUSE_EVENT_CLICKED &&
            (me.getModifiers() & InputEvent::BUTTON1_MASK)) {
        state = !state;
        if(callback)
            callback->stateChanged(this);
        dirty = true;
    } else if (me.getID() == mMouseEvent::MOUSE_EVENT_ENTERED) {
        forcedtextColor = componentFocusTextColor;
        dirty = true; // draw text in red
    } else if (me.getID() == mMouseEvent::MOUSE_EVENT_EXITED) {
        forcedtextColor = Color::darkGray; // instead of global
        dirty = true; // draw defaults;
    }


} // end CheckBox::actionPerformed
