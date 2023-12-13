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


#include "Views/Components/View.hpp"
#include "Views/Components/Desktop.hpp"
#include "Views/Components/Label.hpp"
#include "Interfaces/MouseInterface.hpp"
#include "Interfaces/KeyboardInterface.hpp"
#include "Types/fXY.hpp"
#include "ViewGlobals.hpp"
#include "Util/Exception.hpp"
#include "InputEvent.hpp"

#include <algorithm>

const int RESIZE_WIDTH = 10;
const int RESIZE_XMIN  = RESIZE_WIDTH;
const int RESIZE_XMAX  = RESIZE_WIDTH * 3;
const int RESIZE_YMIN  = RESIZE_WIDTH;
const int RESIZE_YMAX  = RESIZE_WIDTH * 3;

// add
//---------------------------------------------------------------------------
void View::add(Component *component)
{
    if ( component ) {
        components.push_back(component);
        component->setParent(this);
    }
} // end View::add

// addHorizontal
//---------------------------------------------------------------------------
void View::addHorizontal(iXY start_pos, u_int spacing, std::vector<Component*> new_components)
{
    iXY pos = start_pos;
    for (Component* component : new_components) {
        component->setLocation(pos);
        pos = iXY(pos.x + component->getSize().x + spacing, pos.y);
        components.push_back(component);
        component->setParent(this);
    }
} // end View::addHorizontal

// View
//---------------------------------------------------------------------------
View::View()
    : currentscreen(0)
{
    reset();
} // end View::View

// View
//---------------------------------------------------------------------------
View::View(const iXY &pos, const iXY &size, const char *title)
{
    reset();

    // If the button is bordered, then put a close button on it.
    //if (status & STATUS_ISBORDERED)
    //LOG(("About to addButton"));
    //addButton(iXY(getSizeX()-10, getSizeY()-10), "pics/buttons/wclose.raw", "Close the current window", View::toggleWindow);
    //LOG(("Past addButton"));

    moveTo(pos);
    resize(size);
    setTitle(title);

} // end View::View

// ~View
//---------------------------------------------------------------------------
View::~View()
{
    std::vector<cButton*>::iterator i;
    for(i = buttons.begin(); i != buttons.end(); i++) {
        delete *i;
    }
    std::vector<cInputField*>::iterator f;
    for(f = inputFields.begin(); f != inputFields.end(); f++) {
        delete *f;
    }

    free(title);
    free(subTitle);
    free(searchName);
} // end ~View::View

// reset
//---------------------------------------------------------------------------
// Purpose: Reset all the settings for this window.
//---------------------------------------------------------------------------
void View::reset()
{
    min.zero();
    max.zero();
    status                =  0;
    pressedButton         = -1;
    prevPressedButton     = -1;
    highlightedButton     = -1;
    prevHighlightedButton = -1;
    selectedInputField    = -1;
    searchName            = 0;
    title                 = 0;
    subTitle              = 0;
    statusText            = 0;
    focusComponent        = 0;

    setSearchName("donut");
    setTitle("donut");
    setSubTitle("donut");

    //scrollBar             = 0;

    removeComponents();

    moveAreaHeight  = Surface::getFontHeight(); // one row of text plus some padding
    borderSize      = DEFAULT_BORDER_SIZE;
    snapToTolerance = DEFAULT_SNAP_TOLERANCE;

    setAllowResize(true);
    setAllowMove(true);
    setBordered(true);
    setVisible(false);
    setDisplayStatusBar(false);
    setAlwaysOnBottom(false);

    //setScrollBar(false);
} // end reset

// drawBorder
//---------------------------------------------------------------------------
// Purpose: Draws the borders of the window.
//---------------------------------------------------------------------------
void View::drawBorder(Surface &viewArea)
{
    assert(this != 0);

    viewArea.drawWindowsBorder();
} // end drawBorder

// drawTitle
//---------------------------------------------------------------------------
// Purpose: Draws the title of the view and colors the move area background
//          depending on whether the view is active.
//---------------------------------------------------------------------------
void View::drawTitle(Surface &viewArea)
{
    assert(this != 0);

    iRect r(borderSize, borderSize, getSizeX() - borderSize, borderSize + moveAreaHeight - 1);

    Surface s;
    s.setTo(viewArea, r);

    s.fill(getActive() ? activeWindowTitleBodyColor : inactiveWindowTitleBodyColor);

    char strBuf[256];

    sprintf(strBuf, "%s%s", title, subTitle);

    s.bltStringCenter(strBuf, getActive() ? activeWindowTitleTextColor : inactiveWindowTitleTextColor);

    if (getResize()) {
        {
            // Bottom-Left resize.
            iXY verticalPos(borderSize, getSizeY() - borderSize - moveAreaHeight + 1);
            iXY horizontalPos(borderSize + moveAreaHeight - 1, getSizeY() - borderSize);

            viewArea.drawLine(verticalPos, horizontalPos, Color::white);
            verticalPos.y++;
            horizontalPos.x--;
            viewArea.drawLine(verticalPos, horizontalPos, Color::gray224);
            verticalPos.y++;
            horizontalPos.x--;
            viewArea.drawLine(verticalPos, horizontalPos, Color::gray192);
            verticalPos.y++;
            horizontalPos.x--;
            viewArea.drawLine(verticalPos, horizontalPos, Color::gray160);
            verticalPos.y++;
            horizontalPos.x--;
            viewArea.drawLine(verticalPos, horizontalPos, Color::gray160);
            verticalPos.y++;
            horizontalPos.x--;
            viewArea.drawLine(verticalPos, horizontalPos, Color::gray160);
            verticalPos.y++;
            horizontalPos.x--;
            viewArea.drawLine(verticalPos, horizontalPos, Color::gray160);
            verticalPos.y++;
            horizontalPos.x--;
            viewArea.drawLine(verticalPos, horizontalPos, Color::gray160);
            verticalPos.y++;
            horizontalPos.x--;
            viewArea.drawLine(verticalPos, horizontalPos, Color::gray160);
            verticalPos.y++;
            horizontalPos.x--;
            viewArea.drawLine(verticalPos, horizontalPos, Color::gray160);
            verticalPos.y++;
            horizontalPos.x--;
            viewArea.drawLine(verticalPos, horizontalPos, Color::gray160);
            verticalPos.y++;
            horizontalPos.x--;
            viewArea.drawLine(verticalPos, horizontalPos, Color::gray160);
            verticalPos.y++;
            horizontalPos.x--;
        }

        // Top-Left resize.
        {
            iXY verticalPos(borderSize, borderSize + moveAreaHeight - 1);
            iXY horizontalPos(borderSize + moveAreaHeight - 1, borderSize);

            viewArea.drawLine(verticalPos, horizontalPos, Color::white);
            verticalPos.y--;
            horizontalPos.x--;
            viewArea.drawLine(verticalPos, horizontalPos, Color::gray224);
            verticalPos.y--;
            horizontalPos.x--;
            viewArea.drawLine(verticalPos, horizontalPos, Color::gray192);
            verticalPos.y--;
            horizontalPos.x--;
            viewArea.drawLine(verticalPos, horizontalPos, Color::gray160);
            verticalPos.y--;
            horizontalPos.x--;
            viewArea.drawLine(verticalPos, horizontalPos, Color::gray160);
            verticalPos.y--;
            horizontalPos.x--;
            viewArea.drawLine(verticalPos, horizontalPos, Color::gray160);
            verticalPos.y--;
            horizontalPos.x--;
            viewArea.drawLine(verticalPos, horizontalPos, Color::gray160);
            verticalPos.y--;
            horizontalPos.x--;
            viewArea.drawLine(verticalPos, horizontalPos, Color::gray160);
            verticalPos.y--;
            horizontalPos.x--;
            viewArea.drawLine(verticalPos, horizontalPos, Color::gray160);
            verticalPos.y--;
            horizontalPos.x--;
            viewArea.drawLine(verticalPos, horizontalPos, Color::gray160);
            verticalPos.y--;
            horizontalPos.x--;
            viewArea.drawLine(verticalPos, horizontalPos, Color::gray160);
            verticalPos.y--;
            horizontalPos.x--;
            viewArea.drawLine(verticalPos, horizontalPos, Color::gray160);
            verticalPos.y--;
            horizontalPos.x--;
        }

        // Top-Right resize.
        {
            iXY verticalPos(getSizeX() - borderSize, borderSize + moveAreaHeight - 1);
            iXY horizontalPos(getSizeX() - borderSize - moveAreaHeight, borderSize - 1);

            viewArea.drawLine(verticalPos, horizontalPos, Color::white);
            verticalPos.y--;
            horizontalPos.x++;
            viewArea.drawLine(verticalPos, horizontalPos, Color::gray224);
            verticalPos.y--;
            horizontalPos.x++;
            viewArea.drawLine(verticalPos, horizontalPos, Color::gray192);
            verticalPos.y--;
            horizontalPos.x++;
            viewArea.drawLine(verticalPos, horizontalPos, Color::gray160);
            verticalPos.y--;
            horizontalPos.x++;
            viewArea.drawLine(verticalPos, horizontalPos, Color::gray160);
            verticalPos.y--;
            horizontalPos.x++;
            viewArea.drawLine(verticalPos, horizontalPos, Color::gray160);
            verticalPos.y--;
            horizontalPos.x++;
            viewArea.drawLine(verticalPos, horizontalPos, Color::gray160);
            verticalPos.y--;
            horizontalPos.x++;
            viewArea.drawLine(verticalPos, horizontalPos, Color::gray160);
            verticalPos.y--;
            horizontalPos.x++;
            viewArea.drawLine(verticalPos, horizontalPos, Color::gray160);
            verticalPos.y--;
            horizontalPos.x++;
            viewArea.drawLine(verticalPos, horizontalPos, Color::gray160);
            verticalPos.y--;
            horizontalPos.x++;
            viewArea.drawLine(verticalPos, horizontalPos, Color::gray160);
            verticalPos.y--;
            horizontalPos.x++;
            viewArea.drawLine(verticalPos, horizontalPos, Color::gray160);
            verticalPos.y--;
            horizontalPos.x++;
        }

        // Bottom-Right resize.
        {
            iXY verticalPos(getSizeX() - borderSize, getSizeY() - borderSize - moveAreaHeight + 1);
            iXY horizontalPos(getSizeX() - borderSize - moveAreaHeight + 1, getSizeY() - borderSize);

            viewArea.drawLine(verticalPos, horizontalPos, Color::white);
            verticalPos.y++;
            horizontalPos.x++;
            viewArea.drawLine(verticalPos, horizontalPos, Color::gray224);
            verticalPos.y++;
            horizontalPos.x++;
            viewArea.drawLine(verticalPos, horizontalPos, Color::gray192);
            verticalPos.y++;
            horizontalPos.x++;
            viewArea.drawLine(verticalPos, horizontalPos, Color::gray160);
            verticalPos.y++;
            horizontalPos.x++;
            viewArea.drawLine(verticalPos, horizontalPos, Color::gray160);
            verticalPos.y++;
            horizontalPos.x++;
            viewArea.drawLine(verticalPos, horizontalPos, Color::gray160);
            verticalPos.y++;
            horizontalPos.x++;
            viewArea.drawLine(verticalPos, horizontalPos, Color::gray160);
            verticalPos.y++;
            horizontalPos.x++;
            viewArea.drawLine(verticalPos, horizontalPos, Color::gray160);
            verticalPos.y++;
            horizontalPos.x++;
            viewArea.drawLine(verticalPos, horizontalPos, Color::gray160);
            verticalPos.y++;
            horizontalPos.x++;
            viewArea.drawLine(verticalPos, horizontalPos, Color::gray160);
            verticalPos.y++;
            horizontalPos.x++;
            viewArea.drawLine(verticalPos, horizontalPos, Color::gray160);
            verticalPos.y++;
            horizontalPos.x++;
            viewArea.drawLine(verticalPos, horizontalPos, Color::gray160);
            verticalPos.y++;
            horizontalPos.x++;
        }
    }

} // end View::drawTitle

// draw
//---------------------------------------------------------------------------
void View::draw(Surface& surface)
{
    assert(this != 0);

    if (!getVisible())
        return;

    currentscreen = &surface; // hack
    Surface* viewarea = getViewArea(surface);
    Surface* clientarea = getClientArea(surface);
    //LOGGER.info("Name: %s, active=%d, visible=%d, status=%d, resize=%d", getSearchName(), getActive(), getVisible(), getShowStatus(), getResize());
    doDraw(*viewarea, *clientarea);
    delete viewarea;
    delete clientarea;
} // end draw

// activate
//---------------------------------------------------------------------------
// Purpose: Default Implementation - Sets the window status to active.
//---------------------------------------------------------------------------
void View::activate()
{
    if (getActive()) {
        return;
    }

    // Set the active bit.
    status |= STATUS_ACTIVE;

    doActivate();

} // end View::activate

// deactivate
//---------------------------------------------------------------------------
// Purpose: Sets the window status to non-active.
//---------------------------------------------------------------------------
void View::deactivate()
{
    if (!getActive()) {
        return;
    }

    setActive(false);

    doDeactivate();

    highlightedButton     = -1;
    prevHighlightedButton = -1;
    selectedInputField    = -1;

} // end View::deactivate

// doDraw
//---------------------------------------------------------------------------
// Purpose: Default implementation - draws all the componentList of the window.
//---------------------------------------------------------------------------
void View::doDraw(Surface &viewArea, Surface &clientArea)
{
    if (getShowStatus()) {
        drawStatus(clientArea);
    }

    drawDefinedButtons(clientArea);
    drawInputFields(clientArea);
    drawHighlightedButton(clientArea);
    drawPressedButton(clientArea);

    // Draw all non-selected components.
    ComponentsIterator i;
    for ( i=components.begin(); i != components.end(); i++) {
        if (*i != focusComponent) {
            (*i)->draw(clientArea);
        }
    }

    // Now draw the selected component.
    if (focusComponent != 0) {
        focusComponent->draw(clientArea);
    }

    if (getBordered()) {
        drawTitle(viewArea);
        drawBorder(viewArea);
    }
} // end View::doDraw

// doActivate
//---------------------------------------------------------------------------
// Purpose: Default Implementation -
//---------------------------------------------------------------------------
void View::doActivate()
{
    // Tell all the components the mouse entered the view.
    ComponentsIterator i;
    for ( i=components.begin(); i != components.end(); i++)
    {
        if ((*i)->contains(MouseInterface::getMousePosition())) {
            mMouseEvent me((*i),
                mMouseEvent::MOUSE_EVENT_ENTERED, now(), 0,
                MouseInterface::getMouseX(), MouseInterface::getMouseY(), 0, false);

            (*i)->actionPerformed(me);

            actionPerformed(me);
        }
    }
} // end View::doActivate

// doDeactivate
//---------------------------------------------------------------------------
// Purpose: Default Implementation -
//---------------------------------------------------------------------------
void View::doDeactivate()
{
    ComponentsIterator i;
    for ( i=components.begin(); i != components.end(); i++)
    {
        mMouseEvent me((*i),
            mMouseEvent::MOUSE_EVENT_EXITED, now(), 0,
            MouseInterface::getMouseX(), MouseInterface::getMouseY(), 0, false);

        (*i)->actionPerformed(me);

        actionPerformed(me);
    }
} // end View::doDeactivate

// getMouseActions
//---------------------------------------------------------------------------
// Purpose: Find the actions associated with the position of the mouse arrow
//          cursor in the window.
//---------------------------------------------------------------------------
int View::getMouseActions(const iXY &pos) const
{
    assert(this != 0);

    int actions = 0;

    if (!getVisible() || !getActive()) {
        return actions;
    }

    if (!getBordered()) {
        return actions;
    }

    if (getResize()) {
        // Now lets check for a window resize
        iXY minOff;
        iXY maxOff;

        minOff.x = abs(pos.x); maxOff.x = abs(pos.x - getSizeX());
        minOff.y = abs(pos.y); maxOff.y = abs(pos.y - getSizeY());

        // Check the left,  then the right
        if      (minOff.x < RESIZE_XMIN) actions |= MA_RESIZE_LEFT;
        else if (maxOff.x < RESIZE_XMIN) actions |= MA_RESIZE_RIGHT;

        // Check the top, then the bottom
        if      (minOff.y < RESIZE_YMIN) actions |= MA_RESIZE_TOP;
        else if (maxOff.y < RESIZE_YMIN) actions |= MA_RESIZE_BOTTOM;

        // If a left or right was turned on, then give more area for selecting the
        // resize area
        if (actions & (MA_RESIZE_LEFT | MA_RESIZE_RIGHT)) {
            if      (minOff.y < RESIZE_YMAX) actions |= MA_RESIZE_TOP;
            else if (maxOff.y < RESIZE_YMAX) actions |= MA_RESIZE_BOTTOM;
        }

        if (actions & (MA_RESIZE_TOP | MA_RESIZE_BOTTOM)) {
            if      (minOff.x < RESIZE_XMAX) actions |= MA_RESIZE_LEFT;
            else if (maxOff.x < RESIZE_XMAX) actions |= MA_RESIZE_RIGHT;
        }
    }

    // Check for moving the window via the move area.
    if (getAllowMove()) {
        //iRect titleBar(borderSize, borderSize, getSizeX()-borderSize, borderSize + moveAreaHeight);
        iRect titleBar(borderSize, borderSize, getSizeX() - borderSize, borderSize + moveAreaHeight);
        if (titleBar.contains(pos)) return actions |= MA_MOVE;
    }

    // Check for moving the window via the status bar.
    //if (getAllowMove())
    //{
    //	//iRect titleBar(borderSize, borderSize, getSizeX()-borderSize, borderSize + moveAreaHeight);
    //	iRect titleBar(borderSize, getSizeY() - borderSize - DEFAULT_STATUS_BAR_HEIGHT, getSizeX() - borderSize, getSizeY() - borderSize);
    //	if (titleBar.contains(pos)) return actions |= MA_MOVE;
    //}

    return actions;
} // end getMouseActions

// getScreenToClientPos
//---------------------------------------------------------------------------
// Purpose: Finds the position of the mouse in the client area of a window.
//          If this function gets called, we can assume that the mouse is in
//          client area of the window.  IF the window is borderless, then
//          adjust to the window area.
//---------------------------------------------------------------------------
iXY View::getScreenToClientPos(const iXY &pos)
{
    assert(this != 0);

    if (getBordered()) {
        return iXY( pos.x - (min.x + borderSize),
                    pos.y - (min.y + borderSize + moveAreaHeight));
    }

    return getScreenToViewPos(pos);
} // end View::getScreenToClientPos

// getScreenToViewPos
//---------------------------------------------------------------------------
// Purpose: Finds the position of the mouse in the client area of a window.
//          If this function gets called, we can assume that the mouse is in
//          client area of the window.
//---------------------------------------------------------------------------
iXY View::getScreenToViewPos(const iXY &pos)
{
    assert(this != 0);

    return iXY(pos.x - min.x, pos.y - min.y);
} // end getScreenToViewPos

// getViewArea
//---------------------------------------------------------------------------
// Purpose: Returns a Surface of the view's dimensions.
//---------------------------------------------------------------------------
Surface* View::getViewArea(Surface& dest)
{
    assert(this != 0);

    iRect rect(min, max);
    Surface *ns;
    ns = new Surface();
    ns->setTo(dest,rect);
    return ns;
} // end View::getViewArea

// getClientArea
//---------------------------------------------------------------------------
// Purpose:
//---------------------------------------------------------------------------
Surface* View::getClientArea(Surface& dest)
{
    Surface* viewarea = getViewArea(dest);
    if (getBordered()) {
        iRect rect( borderSize, borderSize+moveAreaHeight,
                    getSizeX() - borderSize, getSizeY() - borderSize );
        Surface *ns = new Surface();
        ns->setTo(*viewarea, rect);
        delete viewarea;
        return ns;
    }

    return viewarea;
} // end View::getClientArea

// getClientRect
//---------------------------------------------------------------------------
// Purpose:
//---------------------------------------------------------------------------
iRect View::getClientRect() const
{
    if (getBordered()) {
        return iRect( borderSize,
                      borderSize + moveAreaHeight,
                      getSizeX() - borderSize,
                      getSizeY() - borderSize);
    }

    return iRect(0, 0, getSizeX(), getSizeY());

} // end View::getClientRect

// mouseMove
//---------------------------------------------------------------------------
// Purpse: Changes the current highlighted button and checks for mouseEnter
//         and mouseExit status.
//---------------------------------------------------------------------------
void View::mouseMove(const iXY & prevPos, const iXY &newPos)
{
    prevHighlightedButton = highlightedButton;
    highlightedButton     = findButtonContaining(newPos);

    // Check all components for a clicked event.
    ComponentsIterator i;
    for ( i=components.begin(); i != components.end(); i++)
    {
        bool containsPrev=(*i)->contains(prevPos);
        bool containsNew=(*i)->contains(newPos);
        if (containsPrev && !containsNew) {
            mMouseEvent me((*i), mMouseEvent::MOUSE_EVENT_EXITED, now(), 0, newPos.x, newPos.y, 0, false);

            (*i)->actionPerformed(me);

            actionPerformed(me);
        } else if (!containsPrev && containsNew) {
            mMouseEvent me((*i), mMouseEvent::MOUSE_EVENT_ENTERED, now(), 0, newPos.x, newPos.y, 0, false);

            (*i)->actionPerformed(me);

            actionPerformed(me);
        } else if (containsNew && newPos!=prevPos) {
            mMouseEvent me((*i), mMouseEvent::MOUSE_EVENT_MOVED, now(), 0, newPos.x, newPos.y, 0, false);

            (*i)->actionPerformed(me);

            actionPerformed(me);
        }
    }

    /*  // THIS DOES NOT WORK!!!!!!!!!!
    	bool flagNewPos  = contains(newPos);
    	bool flagPrevPos = contains(prevPos);

    	// Check for status of mouseEnter and mouseExit.
    	if (flagNewPos && !flagPrevPos)
    	{
    		mouseEnter(newPos);
    	}
    	else if (!flagNewPos && flagPrevPos)
    	{
    		mouseExit(prevPos);
    	}
    */
} // end mouseMove

// lMouseDown
//---------------------------------------------------------------------------
void View::lMouseDown(const iXY &pos)
{
    if (pressedButton < 0) {
        prevPressedButton = pressedButton;
        pressedButton     = findButtonContaining(pos);
    }

    if (selectedInputField < 0) {
        selectedInputField = findInputFieldContaining(pos);
    }

    focusComponent = 0;

    // Check all components for a pressed event.
    ComponentsIterator i;
    for ( i=components.begin(); i != components.end(); i++)
    {
        if ((*i)->contains(pos)) {
            mMouseEvent me((*i), mMouseEvent::MOUSE_EVENT_PRESSED, now(), InputEvent::BUTTON1_MASK, pos.x, pos.y, 0, false);

            (*i)->actionPerformed(me);

            focusComponent = (*i);

            actionPerformed(me);
        }
    }

} // end View::lMouseDown

// lMouseUp
//---------------------------------------------------------------------------
int View::lMouseUp(const iXY &downPos, const iXY &upPos)
{
    Desktop::resetMouseActionOffset();

    if (pressedButton == findButtonContaining(upPos)) {
        if (pressedButton >= 0) {
            //if (mouse.getCurButton() & 0)
            if (buttons[pressedButton]->leftClickFunc != 0)
                buttons[pressedButton]->leftClickFunc();
            else
            if (buttons[pressedButton]->rightClickFunc != 0)
                buttons[pressedButton]->rightClickFunc();
        }
    }

    prevPressedButton = pressedButton;
    pressedButton     = -1;

    // Check all components for a clicked event.
    {
        ComponentsIterator i;
        for ( i=components.begin(); ! components.empty() && i != components.end(); i++ )
        {
            if ((*i)->contains(downPos) && (*i)->contains(upPos)) {
                mMouseEvent me((*i), mMouseEvent::MOUSE_EVENT_CLICKED, now(), InputEvent::BUTTON1_MASK, upPos.x, upPos.y, 0, false);

                (*i)->actionPerformed(me);

                actionPerformed(me);
            }
        }
    }

    // Report a mouse release to all components except for a clicked one.
    {
        ComponentsIterator i;
        for ( i=components.begin(); ! components.empty() && i != components.end(); i++)
        {
            if ((*i)->contains(upPos)) {
                mMouseEvent me((*i), mMouseEvent::MOUSE_EVENT_RELEASED, now(), InputEvent::BUTTON1_MASK, upPos.x, upPos.y, 0, false);

                (*i)->actionPerformed(me);

                actionPerformed(me);
            }
        }}

    // I added this so I can tell if a button was pressed when
    // the mouse button is let up.
    return prevPressedButton;

} // end View::lMouseUp

// lMouseDrag
//---------------------------------------------------------------------------
void View::lMouseDrag(const iXY &downPos, const iXY &prevPos, const iXY &newPos)
{
    //lMouseDown(newPos);
    //mouseMove(prevPos, newPos);

    // Check all components for a dragged event.
    ComponentsIterator i;
    for ( i=components.begin(); ! components.empty() && i != components.end(); i++)
    {
        if ((*i)->contains(newPos) && (*i)->contains(downPos) && newPos!=prevPos) {
            mMouseEvent me((*i), mMouseEvent::MOUSE_EVENT_DRAGGED, now(), InputEvent::BUTTON1_MASK, newPos.x, newPos.y, 0, false);

            (*i)->actionPerformed(me);

            focusComponent = (*i);

            actionPerformed(me);
        }
    }

} // end View::lMouseDrag

// lMouseDouble
//---------------------------------------------------------------------------
void View::lMouseDouble(const iXY &pos)
{
    lMouseDown(pos);

} // end View::lMouseDouble

// rMouseDown
//---------------------------------------------------------------------------
void View::rMouseDown(const iXY &pos)
{
    focusComponent = 0;

    // Check all components for a pressed event.
    ComponentsIterator i;
    for ( i=components.begin(); ! components.empty() && i != components.end(); i++)
    {
        if ((*i)->contains(pos)) {
            mMouseEvent me((*i), mMouseEvent::MOUSE_EVENT_PRESSED, now(), InputEvent::BUTTON2_MASK, pos.x, pos.y, 0, false);

            (*i)->actionPerformed(me);

            focusComponent = (*i);

            actionPerformed(me);
        }
    }

} // end View::rMouseDown

// rMouseUp
//---------------------------------------------------------------------------
void View::rMouseUp(const iXY &downPos, const iXY &upPos)
{
    int reportRelease = true;

    // Check all components for a clicked event.
    ComponentsIterator i;
    for ( i=components.begin(); ! components.empty() && i != components.end(); i++)
    {
        if ((*i)->contains(downPos) && (*i)->contains(upPos)) {
            mMouseEvent me((*i), mMouseEvent::MOUSE_EVENT_CLICKED, now(), InputEvent::BUTTON2_MASK, upPos.x, upPos.y, 0, false);

            (*i)->actionPerformed(me);

            reportRelease = false;

            actionPerformed(me);
        }
    }

    // If we are on the last component and there was no component containing both
    // mouse press positions, then report a release to an
    if (reportRelease) {
        // Check all components for a release event.
        ComponentsIterator i;
        for ( i=components.begin(); ! components.empty() && i != components.end(); i++)
        {
            if ((*i)->contains(upPos)) {
                mMouseEvent me((*i), mMouseEvent::MOUSE_EVENT_RELEASED, now(), InputEvent::BUTTON2_MASK, upPos.x, upPos.y, 0, false);

                (*i)->actionPerformed(me);

                reportRelease = false;

                actionPerformed(me);
            }
        }
    }

} // end View::rMouseUp

// rMouseUp
//---------------------------------------------------------------------------
void View::rMouseUp()
{} // end View::rMouseUp

// rMouseDrag
//---------------------------------------------------------------------------
void View::rMouseDrag(const iXY &downPos, const iXY &prevPos, const iXY &newPos)
{
    rMouseDown(newPos);

    focusComponent = 0;

    // Check all components for a moved event.
    ComponentsIterator i;
    for ( i=components.begin(); i != components.end(); i++)
    {
        if ((*i)->contains(newPos)  && (*i)->contains(downPos) && newPos!=prevPos) {
            mMouseEvent me((*i), mMouseEvent::MOUSE_EVENT_DRAGGED, now(), InputEvent::BUTTON2_MASK, newPos.x, newPos.y, 0, false);

            (*i)->actionPerformed(me);

            focusComponent = (*i);

            actionPerformed(me);
        }
    }

} // end View::rMouseDrag

// rMouseDouble
//---------------------------------------------------------------------------
void View::rMouseDouble(const iXY &pos)
{
    rMouseDown(pos);

} // end View::rMouseDouble

// mouseEnter
//---------------------------------------------------------------------------
// Purpose: Is triggered when the mouse enters the window.
//---------------------------------------------------------------------------
void View::mouseEnter(const iXY &pos)
{
    // Check all components for a entered event.
    ComponentsIterator i;
    for ( i=components.begin(); i != components.end(); i++)
    {
        if ((*i)->contains(pos)) {
            mMouseEvent me((*i), mMouseEvent::MOUSE_EVENT_ENTERED, now(), 0, pos.x, pos.y, 0, false);

            (*i)->actionPerformed(me);

            actionPerformed(me);
        }
    }

} // end View::mouseEnter

// mouseExit
//---------------------------------------------------------------------------
// Purpose: Is triggered when the mouse leaves the window.
//---------------------------------------------------------------------------
void View::mouseExit(const iXY &pos)
{
    assert(this != 0);

    // Check all components for a exited event.
    ComponentsIterator i;
    for ( i=components.begin(); i != components.end(); i++)
    {
        if ((*i)->contains(pos)) {
            mMouseEvent me((*i), mMouseEvent::MOUSE_EVENT_EXITED, now(), 0, pos.x, pos.y, 0, false);

            (*i)->actionPerformed(me);

            actionPerformed(me);
        }
    }

} // end View::mouseExit

// scrollBarMove
//---------------------------------------------------------------------------
// Purpose:
//---------------------------------------------------------------------------
void View::scrollBarMove(const iXY &prevPos, const iXY &newPos)
{
    (void) prevPos;
    (void) newPos;
    //if (scrollBar != 0)
    //{
    //	scrollBar->actionPerformed(newPos);
    //}
} // end scrollBarMove


// drawDefinedButtons
//---------------------------------------------------------------------------
// Purpose:
//---------------------------------------------------------------------------
void View::drawDefinedButtons(Surface &clientArea)
{
    std::vector<cButton*>::iterator i;
    for(i = buttons.begin(); i != buttons.end(); i++) {
        cButton* button = *i;
        button->topSurface.blt(clientArea, button->getBounds().min);
    }
} // end drawDefinedButtons

// drawInputFields
//---------------------------------------------------------------------------
// Purpose:
//---------------------------------------------------------------------------
void View::drawInputFields(Surface &clientArea)
{
    for (size_t num = 0; num < inputFields.size(); num++) {
        if (num == (size_t) selectedInputField) {
            inputFields[num]->drawHighlighted(clientArea);
        } else {
            inputFields[num]->draw(clientArea);
        }
    }
} // end drawInputFields

// drawHighlightedButton
//---------------------------------------------------------------------------
// Purpose: Draws the correct image frame for the currently highlighted button.
//---------------------------------------------------------------------------
void View::drawHighlightedButton(Surface &clientArea)
{
    assert(this != 0);

    if (highlightedButton < 0) {
        return;
    } else if (buttons[highlightedButton]->topSurface.getFrameCount() < 2) {
        cButton* button = buttons[highlightedButton];
        clientArea.drawRect(iRect(button->getBounds().min.x,
                                  button->getBounds().min.y,
                                  button->getBounds().max.x,
                                  button->getBounds().max.y),
                            Color::red);
        return;
    }

    if (pressedButton == highlightedButton) {
        return;
    }

    if (highlightedButton > (int) buttons.size()) {
        throw Exception("ERROR: highlightedButton > butons.getCount()");
    }

    // Change to the highlight button frame.
    buttons[highlightedButton]->topSurface.setFrame(1);
    buttons[highlightedButton]->topSurface.blt(clientArea,
            iXY(buttons[highlightedButton]->getBounds().min.x,
                buttons[highlightedButton]->getBounds().min.y));
    buttons[highlightedButton]->topSurface.setFrame(0);

} // end drawHighlightedButton

// addButtonCenterText
//---------------------------------------------------------------------------
// Purpose: Adds a centered type button to the list of buttons.
//---------------------------------------------------------------------------
void View::addButtonCenterText(const iXY &pos,
                               const int &xSize,
                               const char *name,
                               const char *toolTip,
                               ITEM_FUNC leftClickFunc)
{
    cButton* button = new cButton;
    button->createCenterText(pos, xSize, name, toolTip, leftClickFunc);
    buttons.push_back(button);
} // end addButtonCenterText

// setSearchName
//---------------------------------------------------------------------------
// Purpose: Sets the title of the window.
//---------------------------------------------------------------------------
void View::setSearchName(const char *searchName)
{
    free(View::searchName);
    View::searchName = 0;

    if (searchName != 0) {
        View::searchName = strdup(searchName);
        if (View::searchName == 0) {
            throw Exception("ERROR: Unable to allocate searchName: %s", searchName);
        }
    }
} // end View::setSearchName

// setTitle
//---------------------------------------------------------------------------
// Purpose: Sets the title of the window.
//---------------------------------------------------------------------------
void View::setTitle(const char *title)
{
    if (View::title != 0) {
        free(View::title);
        View::title = 0;
    }

    if (title != 0) {
        View::title = strdup(title);
        if (View::title == 0) {
            throw Exception("ERROR: Unable to allocate title: %s", title);
        }
    }
} // end View::setTitle

// setSubTitle
//---------------------------------------------------------------------------
// Purpose: Sets the subTitle of the window.
//---------------------------------------------------------------------------
void View::setSubTitle(const char *subTitle)
{
    if (View::subTitle != 0) {
        free(View::subTitle);
        View::subTitle = 0;
    }

    if (subTitle != 0) {
        View::subTitle = strdup(subTitle);
        if (View::subTitle == 0) {
            throw Exception("ERROR: Unable to allocate subTitle: %s", subTitle);
        }
    }
} // end View::setSubTitle

// showStatus
//---------------------------------------------------------------------------
// Purpose: Sets the status bar text.
//---------------------------------------------------------------------------
void View::showStatus(const char *string)
{
    if (statusText != 0) {
        free(statusText);
        statusText = 0;
    }

    if (string != 0) {
        statusText = strdup(string);
        if(statusText == 0) {
            throw Exception("ERROR: statusText == 0");
        }
    }

} // end View::showStatus

// drawStatus
//---------------------------------------------------------------------------
void View::drawStatus(Surface &dest)
{
    // Draw the status bar.
    iRect clientRect(getClientRect());

    iRect r(0,
            clientRect.getSizeY() - DEFAULT_STATUS_BAR_HEIGHT,
            clientRect.getSizeX(),
            clientRect.getSizeY());

    Surface s;
    s.setTo(dest, r);
    s.fill(Color::gray192);

    // Draw the status text.
    if (statusText != 0) {
        int pos = (DEFAULT_STATUS_BAR_HEIGHT - Surface::getFontHeight()) >> 1;

        s.bltString(pos, pos, statusText, Color::black);
    }
} // end View::drawStatus

// findButtonContaining
//---------------------------------------------------------------------------
// Purpose: Finds the button containing the position of the mouse.  The
//          button are bounded relative to the window, so make sure to make
//          the mouse position relative to the window before calling this.
//---------------------------------------------------------------------------
int View::findButtonContaining(const iXY &pos)
{
    assert(this != 0);

    for (size_t num = 0; num < buttons.size(); num++) {
        if (buttons[num]->contains(pos)) {
            //LOG(("pressed button: %u", num));
            return num;
        }
    }
    return -1;
} // end findButtonContaining

// findInputFieldContaining
//---------------------------------------------------------------------------
// Purpose:
//---------------------------------------------------------------------------
int View::findInputFieldContaining(const iXY &pos)
{
    assert(this != 0);

    for(size_t num = 0; num < inputFields.size(); num++) {
        if (inputFields[num]->contains(pos)) {
            return num;
        }
    }
    return -1;
} // end findInputFieldContaining

// drawPressedButton
//---------------------------------------------------------------------------
// Purpose: Draws the selected button.
//---------------------------------------------------------------------------
void View::drawPressedButton(Surface &clientArea)
{
    assert(this != 0);

    if (pressedButton < 0 || buttons[pressedButton]->topSurface.getFrameCount() < 2) return;
    if (highlightedButton != pressedButton) return;
    assert(pressedButton < (int) buttons.size());

    // Chage to the highlight button frame.
    buttons[pressedButton]->topSurface.setFrame(2);
    buttons[pressedButton]->topSurface.blt(clientArea,
            iXY(buttons[pressedButton]->getBounds().min.x,
                buttons[pressedButton]->getBounds().min.y));
    buttons[pressedButton]->topSurface.setFrame(0);
} // drawPressedButton

// checkResolution
//---------------------------------------------------------------------------
// Purpose: Check position after resolution change
//---------------------------------------------------------------------------
void View::checkResolution(iXY oldResolution, iXY newResolution)
{
        iXY size = getSize();

        min.x += (newResolution.x - oldResolution.x)/2;
        min.y += (newResolution.y - oldResolution.y)/2;
        max = min + size;
} // end checkResolution

void View::checkArea(iXY viewarea)
{
    if(min.x < 0)
        moveTo(0, min.y);
    if(min.y < 0)
        moveTo(min.x, 0);

    if(max.x > viewarea.x)
        moveTo(viewarea.x - getSize().x, min.y);
    if(max.y > viewarea.y)
        moveTo(min.x, viewarea.y - getSize().y);
}

// RESIZE CLIENT AREA
//---------------------------------------------------------------------------
// Purpose: Resize the client area of the window.  The window area will be
//          resized accordingly.
//---------------------------------------------------------------------------
void View::resizeClientArea(const iXY &size)
{
    // These variables constitute the gap space needed for the window borders and
    // move area.
    int xExtra = borderSize * 2;
    int yExtra = xExtra + moveAreaHeight;

    iXY destSize(size);

    // Add the area for the borders and move area.
    destSize.x += xExtra;
    destSize.y += yExtra;

    resize(destSize);
} // end View::resizeClientArea

// RESIZE
//---------------------------------------------------------------------------
// Purpose: Resizes the current window to the specified size.
//---------------------------------------------------------------------------
void View::resize(const iXY &size)
{
    iXY destSize(size);

    max = min + destSize;
} // end View::resize

// setPressedButton
//---------------------------------------------------------------------------
void View::setPressedButton(const int &button)
{
    if (button >= (int) buttons.size()) {
        throw Exception("ERROR: pressedButton >= numButtons");
    }

    prevPressedButton = pressedButton;
    pressedButton     = button;
} // end setPressedButton

// setHighlightedButton
//---------------------------------------------------------------------------
void View::setHighlightedButton(const int &button)
{
    if (button >= (int) buttons.size()) {
        throw Exception("ERROR: highlightedButton >= numButtons");
    }

    prevHighlightedButton = highlightedButton;
    highlightedButton     = button;
} // end setHighlightedButton

// moveTo
//---------------------------------------------------------------------------
// Purpose: Move the window to the specified position, while making sure the
//          window is on the screen.
//---------------------------------------------------------------------------
void View::moveTo(iXY destMin)
{
    iXY size = getSize();
    min = destMin;
    max = min + size;
} // end moveTo

// toggleView
//---------------------------------------------------------------------------
// Purpose: Toggle the visibility of this window.
//---------------------------------------------------------------------------
void View::toggleView()
{
    Desktop::toggleVisibility(searchName);
} // end toggleView

// setAllowResize
//---------------------------------------------------------------------------
// Purpose: Allows this view to be resized or not.
//---------------------------------------------------------------------------
void View::setAllowResize(const bool &newStatus)
{
    if (newStatus == true) status |=  STATUS_ALLOW_RESIZE;
    else                   status &= ~STATUS_ALLOW_RESIZE;
} // end setAllowResize

// setDisplayStatusBar
//---------------------------------------------------------------------------
// Purpose: Displays a status bar in the view or not.
//---------------------------------------------------------------------------
void View::setDisplayStatusBar(const bool &newStatus)
{
    if (newStatus == true) status |=  STATUS_DISPLAY_STATUS_BAR;
    else                   status &= ~STATUS_DISPLAY_STATUS_BAR;
} // setDisplayStatusBar

// setAlwaysOnBottom
//---------------------------------------------------------------------------
// Purpose: Makes this view always drawn on the bottom or not.
//---------------------------------------------------------------------------
void View::setAlwaysOnBottom(const bool &newStatus)
{
    if (newStatus == true) status |=  STATUS_ALWAYS_ON_BOTTOM;
    else                   status &= ~STATUS_ALWAYS_ON_BOTTOM;
} // setAlwaysOnBottom

// setBordered
//---------------------------------------------------------------------------
// Purpose: Turns on the borders for this view or not.
//---------------------------------------------------------------------------
void View::setBordered(const bool &newStatus)
{
    if (newStatus == true) status |=  STATUS_BORDERED;
    else                   status &= ~STATUS_BORDERED;
} // end setBordered

// setAllowMove
//---------------------------------------------------------------------------
void View::setAllowMove(const bool &newStatus)
{
    if (newStatus == true) status |=  STATUS_ALLOW_MOVE;
    else                   status &= ~STATUS_ALLOW_MOVE;
} // end setAllowMove

// setVisible
//---------------------------------------------------------------------------
// Purpose: Makes this view visible or not.
//---------------------------------------------------------------------------
void View::setVisible(const bool &newStatus)
{
    if (newStatus == true) status |=  STATUS_VISIBLE;
    else                   status &= ~STATUS_VISIBLE;
} // end setVisible

// setActive
//---------------------------------------------------------------------------
// Purpose: Sets this view as an active view or not.
//---------------------------------------------------------------------------
void View::setActive(const bool &newStatus)
{
    if (newStatus == true) status |=  STATUS_ACTIVE;
    else                   status &= ~STATUS_ACTIVE;
} // end setActive

// processEvents
//---------------------------------------------------------------------------
void View::processEvents(void)
{
    if (selectedInputField >= 0) {
        int key_char;
        while (KeyboardInterface::getChar(key_char)) {
            // Check for extended code.
            if (key_char == 0) {
                if (KeyboardInterface::getChar(key_char)) {
                    inputFields[selectedInputField]->addExtendedChar(key_char);
                } else {
                    throw Exception("ERROR: Expecting extended char code.");
                }
            } else {
                inputFields[selectedInputField]->addChar(key_char);
            }
        }
    }
} // end processEvents

// addInputField
//---------------------------------------------------------------------------
cInputField* View::addInputField(
    const iXY &pos,
    cInputFieldString *string,
    const char *excludedCharacters,
    const bool &isSelected,
    const int maxCharCount)
{
    cInputField* inputfield = new cInputField;

    inputfield->setPos(pos);
    inputfield->setInputFieldString(string);
    inputfield->setExcludedCharacters(excludedCharacters);
    inputfield->setmaxChar(maxCharCount);
    inputFields.push_back(inputfield);

    if (isSelected) {
        selectedInputField = inputFields.size() - 1;
    }

    return inputfield;
} // end addInputField

void View::removeInputField(cInputField *cif)
{
    if ( cif )
    {
        inputFields.erase(std::remove(inputFields.begin(), inputFields.end(), cif), inputFields.end());
        delete cif;
    }
}

//---------------------------------------------------------------------------
void View::add(DEFAULT_VIEW_BUTTON button)
{
    if (button == CLOSE_VIEW_BUTTON) {}
    else if (button == MINMAX_VIEW_BUTTON) {}
    else {
        assert(false);
    }
}

