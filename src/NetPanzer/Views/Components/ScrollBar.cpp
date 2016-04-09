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


#include "Views/Components/ScrollBar.hpp"

// ScrollBar
//---------------------------------------------------------------------------
ScrollBar::ScrollBar() : Component()
{
    reset();
    decrease = new Button( "decreaseButton");
    decrease->setLabel("-");

    increase = new Button( "increaseButton");
    increase->setLabel("+");


} // end ScrollBar::ScrollBar

// ScrollBar
//---------------------------------------------------------------------------
ScrollBar::ScrollBar(Orientation orientation)
{
    reset();

    ScrollBar::orientation = orientation;
    decrease = new Button( "decreaseButton");
    decrease->setLabel("-");

    increase = new Button( "increaseButton");
    increase->setLabel("+");

} // end ScrollBar::ScrollBar

// ScrollBar
//---------------------------------------------------------------------------
ScrollBar::ScrollBar(Orientation orientation, int value, int visible, int minimum, int maximum)
{
    reset();

    ScrollBar::orientation = orientation;
    ScrollBar::value       = value;
    ScrollBar::value       = visible;
    ScrollBar::minimum     = minimum;
    ScrollBar::maximum     = maximum;

    decrease = new Button( "decreaseButton");
    decrease->setLabel("-");

    increase = new Button( "increaseButton");
    increase->setLabel("+");

    decrease->setSize(MINOR_AXIS_SIZE, MINOR_AXIS_SIZE);
    increase->setSize(MINOR_AXIS_SIZE, MINOR_AXIS_SIZE);

} // end ScrollBar::ScrollBar

// reset
//---------------------------------------------------------------------------
void ScrollBar::reset()
{
    blockIncrement = DEFAULT_BLOCK_INCREMENT;
    minimum        = DEFAULT_MINIMUM;
    maximum        = DEFAULT_MAXIMUM;
    orientation    = VERTICAL;
    value          = DEFAULT_VALUE;
    unitIncrement  = DEFAULT_UNIT_INCREMENT;
    viewableAmount = 0;
    percent        = 0.0f;

} // end reset

// setLocation
//---------------------------------------------------------------------------
void ScrollBar::setLocation()
{
    if (parent != 0) {
        if (orientation == HORIZONTAL) {
            // MINOR_AXIS_SIZE * 2 is room for the click buttons.
            position.x = ((View *) parent)->getClientRect().min.x + MINOR_AXIS_SIZE;
            position.y = ((View *) parent)->getClientRect().max.y - MINOR_AXIS_SIZE;
        } else if (orientation == VERTICAL) {
            // MINOR_AXIS_SIZE * 2 is room for the click buttons.
            position.x = ((View *) parent)->getClientRect().max.x - MINOR_AXIS_SIZE;
            position.y = ((View *) parent)->getClientRect().min.y + MINOR_AXIS_SIZE;
        } else {
            assert(false);
        }
    }

} // end ScrollBar::setLocation

// setSize
//---------------------------------------------------------------------------
void ScrollBar::setSize()
{
    if (parent != 0) {
        if (orientation == HORIZONTAL) {
            // MINOR_AXIS_SIZE * 2 is room for the click buttons.
            size.x = ((View *) parent)->getClientRect().getSizeX() - MINOR_AXIS_SIZE * 2;
            size.y = MINOR_AXIS_SIZE;
        } else if (orientation == VERTICAL) {
            // MINOR_AXIS_SIZE * 2 is room for the click buttons.
            size.x = MINOR_AXIS_SIZE;
            size.y = ((View *) parent)->getClientRect().getSizeY() - MINOR_AXIS_SIZE * 2;

        } else {
            assert(false);
        }
    }

} // end ScrollBar::setSize

// draw
//---------------------------------------------------------------------------
void ScrollBar::draw(Surface &dest)
{
    setLocation();
    setSize();

    // Set the button locations.
    if (orientation == VERTICAL) {
        decrease->setLocation(position.x, position.y - MINOR_AXIS_SIZE);
        increase->setLocation(position.x, position.y + MINOR_AXIS_SIZE);
    } else if (orientation == HORIZONTAL) {
        decrease->setLocation(position.x - MINOR_AXIS_SIZE, position.y);
        increase->setLocation(position.x + MINOR_AXIS_SIZE, position.y);
    } else {
        assert(false);
    }

    // Draw the value indicator bar.
    if (enabled) {
        iXY drawMin;

        if (orientation == VERTICAL) {
            drawMin.x = dest.getWidth() - size.x;
            drawMin.y = MINOR_AXIS_SIZE;

        } else if (orientation == HORIZONTAL) {
            drawMin.x = MINOR_AXIS_SIZE;
            drawMin.y = dest.getHeight() - size.y;
        } else {
            assert(false);
        }

        iRect r(drawMin, drawMin + size);

        surface.setTo(dest, r);
        //surface.fill(Color::gray128);
        //surface.shade(Color::white);

        //surface.drawButtonBorder(Color::gray192, Color::gray64);
        if (orientation == VERTICAL) {
            int midHandlePos = (int) (float(size.y) * percent);

            r = iRect(0, midHandlePos, size.x, midHandlePos);
            surface.fillRect(r, Color::gray128);
            surface.drawButtonBorder(r, Color::gray64, Color::gray192);

        } else if (orientation == HORIZONTAL) {
            int midHandlePos = (int) (float(size.x) * percent);

            r = iRect(midHandlePos, 0, midHandlePos, size.y);
            surface.fillRect(r, Color::gray128);
            surface.drawButtonBorder(r, Color::gray64, Color::gray192);

        } else {
            assert(false);
        }

        // Draw the top click button.
        //Surface shit;
        //
        //r = iRect(0, 0, MINOR_AXIS_SIZE, MINOR_AXIS_SIZE);
        //shit.setTo(surface, r);
        //shit.fill(Color::gray128);
        //shit.drawButtonBorder(Color::gray64, Color::gray192);
        //iXY shitCenter(shit.getPix() / 2);
        //
        //if (orientation == VERTICAL)
        //{
        //	shit.drawHLine(shitCenter.x - 3, shitCenter.y + 0, shitCenter.x + 3, Color::black);
        //	shit.drawHLine(shitCenter.x - 2, shitCenter.y - 1, shitCenter.x + 2, Color::black);
        //	shit.drawHLine(shitCenter.x - 1, shitCenter.y - 2, shitCenter.x + 1, Color::black);
        //	shit.drawHLine(shitCenter.x - 0, shitCenter.y - 3, shitCenter.x + 0, Color::black);
        //}
        //else if (orientation== HORIZONTAL)
        //{
        //	shit.drawVLine(shitCenter.x - 0, shitCenter.y - 0, shitCenter.y + 0, Color::black);
        //	shit.drawVLine(shitCenter.x - 1, shitCenter.y - 1, shitCenter.y + 1, Color::black);
        //	shit.drawVLine(shitCenter.x - 2, shitCenter.y - 2, shitCenter.y + 2, Color::black);
        //	shit.drawVLine(shitCenter.x - 3, shitCenter.y - 3, shitCenter.y + 3, Color::black);
        //} else
        //{
        //	assert(false);
        //}
        //
        //// Draw the bottom click button.
        //if (orientation == VERTICAL)
        //{
        //	r = iRect(0, size.y, size.x, size.y + MINOR_AXIS_SIZE);
        //}
        //else if (orientation == HORIZONTAL)
        //{
        //	r = iRect(size.x, 0, size.x + MINOR_AXIS_SIZE, MINOR_AXIS_SIZE);
        //} else
        //{
        //	assert(false);
        //}
        //
        //
        //shit.setTo(surface, r);
        //shit.fill(Color::gray128);
        //shit.drawButtonBorder(Color::gray64, Color::gray192);
        //
        //if (orientation == VERTICAL)
        //{
        //	shit.drawHLine(shitCenter.x - 3, shitCenter.y + 0, shitCenter.x + 3, Color::black);
        //	shit.drawHLine(shitCenter.x - 2, shitCenter.y + 1, shitCenter.x + 2, Color::black);
        //	shit.drawHLine(shitCenter.x - 1, shitCenter.y + 2, shitCenter.x + 1, Color::black);
        //	shit.drawHLine(shitCenter.x - 0, shitCenter.y + 3, shitCenter.x + 0, Color::black);
        //}
        //else if (orientation == HORIZONTAL)
        //{
        //	shit.drawVLine(shitCenter.x + 0, shitCenter.y - 0, shitCenter.y + 0, Color::black);
        //	shit.drawVLine(shitCenter.x + 1, shitCenter.y - 1, shitCenter.y + 1, Color::black);
        //	shit.drawVLine(shitCenter.x + 2, shitCenter.y - 2, shitCenter.y + 2, Color::black);
        //	shit.drawVLine(shitCenter.x + 3, shitCenter.y - 3, shitCenter.y + 3, Color::black);
        //}

    } else {
        //surface.fill(background);
        //surface.shade(Color::red);
    }

} // end ScrollBar::draw

// clicked
//---------------------------------------------------------------------------
void ScrollBar::actionPerformed(const mMouseEvent& me)
{
    iXY pos = me.getPoint();
    if (parent != 0) {
        if (orientation == HORIZONTAL) {
            percent = float(pos.x) / float(size.x);
        } else if (orientation == VERTICAL) {
            percent = float(pos.y) / float(size.y);
        } else {
            assert(false);
        }

        int newValue = int(percent * float(maximum - minimum -
		       viewableAmount));

        int difference = newValue - value;

        // Move the bar only by the block increment.
        if (difference > blockIncrement) {
            value += blockIncrement;
        } else if(difference < -blockIncrement) {
            value -= blockIncrement;
        }

        // Check lower bounds.
        if (value > maximum) {
            value = maximum;
        }

        // Check upper bounds.
        if (value < minimum) {
            value = minimum;
        }

        enabled = true;

        // This is the case where there are not enough items to fill up the scroll area.
        if (viewableAmount > (maximum - minimum)) {
            value   = 0;
            enabled = false;
        }

        // Recalculate the percent.
        //percent = float(value) / float(maximum - minimum - viewableAmount);
    }

} // end ScrollBar::clicked

// getValue
//---------------------------------------------------------------------------
int ScrollBar::getValue() const
{
    //float scale = ((TEXT_GAP_SPACE + CHAR_YPIX) * (maximum - minimum)) / viewableAmount;

    return value;//int(percent * float(maximum - minimum - viewableAmount));

} // end ScrollBar::getValue
