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


#include <algorithm>
#include "Views/Components/Choice.hpp"
#include "Views/Components/View.hpp"
#include "Views/Components/StateChangedCallback.hpp"
#include "ViewGlobals.hpp"

//---------------------------------------------------------------------------
void Choice::reset()
{
    index     = 0;
    minWidth  = 0;
    size.x    = 10;
    size.y    = Surface::getFontHeight();
    isOpen    = 0;
    mouseover = 0;
    adjustedY = 0;
    choiceItemHeight = Surface::getFontHeight();
}

//---------------------------------------------------------------------------
void Choice::addItem(const std::string& item)
{
    choiceList.push_back(item);

    int borderSpace = borderSize * 2;

    size.x = std::max((((int) Surface::getTextLength(item)) + borderSpace), size.y);
    size.y = choiceItemHeight;
}

//---------------------------------------------------------------------------
void Choice::select(const std::string& item)
{
    for(size_t i = 0; i < choiceList.size(); i++) {
        if(choiceList[i] == item) {
            if(index == i)
                return;

            mouseover = index = i;
            if(callback)
                callback->stateChanged(this);

            return;
        }
    }
}

//---------------------------------------------------------------------------
void Choice::select(size_t index)
{
    assert(index < choiceList.size());

    if(index == Choice::index)
        return;

    Choice::index = mouseover = index;
    if(callback)
        callback->stateChanged(this);
}

//---------------------------------------------------------------------------
void Choice::actionPerformed(const mMouseEvent &me)
{
    iXY parentDimensions(((View *)parent)->getClientRect().getSize());

    if (me.getID() == mMouseEvent::MOUSE_EVENT_PRESSED &&
            (me.getModifiers() & InputEvent::BUTTON1_MASK)) {
        //assert(isOpen == false);
        isOpen = true;

        // Set the size to accomodate all items.
        size.y = choiceList.size() * choiceItemHeight;

        // Make sure the choice fits on the screen.
        if (position.y + size.y >= parentDimensions.y) {
            // Check to see if it will fit above the base position.
            if ((position.y + choiceItemHeight) - size.y > 0) {
                // Since it fits above, put it there.
                adjustedY =  size.y - choiceItemHeight;
                position.y     -= adjustedY;

            } else {
                // Since it does not fit above, just put it below and keep it on the screen.
                adjustedY =  position.y + size.y - parentDimensions.y;
                position.y     -= adjustedY + 1;
            }


            // Make sure the choice is still on the screen.
            assert (position.y >= 0);
        }
    } else if (me.getID() == mMouseEvent::MOUSE_EVENT_DRAGGED &&
                (me.getModifiers() & InputEvent::BUTTON1_MASK)) {
        isOpen = true;
        size.y = choiceList.size() * choiceItemHeight;

        iRect r(position.x, position.y, position.x + size.x, position.y + choiceItemHeight);

        for (size_t i = 0; i < choiceList.size(); i++) {
            // Find the selected item.
            if (r.contains(iXY(me.getX(), me.getY()))) {
                mouseover = i;
                break;
            }

            r.translate(iXY(0, choiceItemHeight));
        }
    } else if (	me.getID() == mMouseEvent::MOUSE_EVENT_CLICKED &&
                (me.getModifiers() & InputEvent::BUTTON1_MASK)) {
        //assert(isOpen);
        isOpen = false;

        // Set the size back down to a single item.
        size.y = choiceItemHeight;

        // Move the choice back to its original location.
        position.y += adjustedY;
        adjustedY = 0;

        // set new element
        if(mouseover == index)
            return;

        index = mouseover;
        if(callback)
            callback->stateChanged(this);

        // Since an item was selected, find which item was selected.
    } else if (	me.getID() == mMouseEvent::MOUSE_EVENT_EXITED) {
        //assert(!isOpen);
        isOpen = false;

        // Set the size back down to a single item.
        size.y = choiceItemHeight;

        // Move the choice back to its original location.
        position.y += adjustedY;
        adjustedY = 0;

        // set new element
//        if(mouseover == index) return;
//        index = mouseover;
//        if(callback) callback->stateChanged(this);
    }
}

// draw
//---------------------------------------------------------------------------
void Choice::draw(Surface &dest)
{
    Surface s;
    iRect   r;

    // Draw the label.
    iXY pos;

    //pos.x = min.x - strlen((const char *) label) * CHAR_XPIX - CHAR_XPIX;
    pos.x = position.x;
    pos.y = position.y - Surface::getFontHeight() - 4;

    // Draw the name of the choice.
    //dest.bltStringShadowed( position.x, pos.y + adjustedY,
                            //componentName.c_str(), componentActiveTextColor, Color::black);

    dest.bltString( position.x, pos.y + adjustedY + (choiceItemHeight / 3),
                            componentName.c_str(), Color::black);

    getBounds(r);

    s.setTo(dest, r);

    // Draw the border.
    s.RoundRect(iRect(0, 0, s.getWidth() - 2, s.getHeight() - 2), 4, Color::gray96);
    s.RoundRect(iRect(1, 1, s.getWidth() - 1, s.getHeight() - 1), 4, componentActiveTextColor);
    s.FillRoundRect(iRect(1, 1, s.getWidth() - 2, s.getHeight() - 2), 4, componentBodyColor);

    if (!isOpen)	{
        //s.bltStringShadowedCenter(choiceList[index].c_str(), componentActiveTextColor, Color::black);
        s.bltStringCenter(choiceList[index].c_str(), Color::black);
    } else {
        r = iRect(position.x, position.y, position.x + size.x, position.y + choiceItemHeight);

        size_t count = choiceList.size();

        for (size_t i = 0; i < count; i++) {
            s.setTo(dest, r);

            if (i == mouseover) {
                // Highlight the selected item.
                s.fill(componentActiveTextColor);
                s.bltStringCenter(choiceList[i].c_str(), Color::black);

            } else {
                //s.bltStringShadowedCenter(choiceList[i].c_str(), componentActiveTextColor, Color::black);
                s.bltStringCenter(choiceList[i].c_str(), Color::black);
            }

            r.translate(iXY(0, choiceItemHeight));
        }
    }
    //isOpen = 0;
} // end Choice::draw

// add
//---------------------------------------------------------------------------
void Choice::add(const std::string& item)
{
    choiceList.push_back(item);

    int borderSpace = borderSize * 2;

    size.x = std::max(((int) Surface::getTextLength(item)) + borderSpace, size.y);
    size.x = std::max(minWidth, size.x);
} // end Choice::add

// setMinWidth
//---------------------------------------------------------------------------
void Choice::setMinWidth(int minWidth)
{
    Choice::size.x   = minWidth;
    Choice::minWidth = minWidth;

} // end Choice::setMinWidth

// copyItems
//---------------------------------------------------------------------------
void Choice::copyItems(const Choice &choice)
{
    choiceList = choice.choiceList;

    select(0);
} // end Choice::copyItems
