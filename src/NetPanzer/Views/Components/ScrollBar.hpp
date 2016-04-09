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
#ifndef __ScrollBar_hpp__
#define __ScrollBar_hpp__

#include "Component.hpp"
#include "Views/Components/View.hpp"
#include "Views/Components/Button.hpp"

//--------------------------------------------------------------------------
class ScrollBar : public Component
{
public:
    enum Orientation {
        HORIZONTAL, VERTICAL
    };                          

private:
    enum { DEFAULT_BLOCK_INCREMENT =   1 };
    enum { DEFAULT_MINIMUM         =   0 };
    enum { DEFAULT_MAXIMUM         = 100 };
    enum { DEFAULT_VALUE           =   0 };
    enum { DEFAULT_UNIT_INCREMENT  =   1 };
    enum { MINOR_AXIS_SIZE         =  12 };

    int   blockIncrement;
    int   minimum;
    int   maximum;
    Orientation orientation;
    int   unitIncrement;
    int   value;
    int   viewableAmount; // The total number of viewable items.
    float percent;

    Button * decrease;
    Button * increase;

protected:
    void clicked(const iXY &pos);
    void setSize();
    void setLocation();
    void reset();

public:
    ScrollBar();
    ScrollBar(Orientation orientation);
    ScrollBar(Orientation orientation, int value, int visible, int minimum, int maximum);
    virtual ~ScrollBar()
    {}

    int getBlockIncrement() const
    {
        return blockIncrement;
    }
    int getMaximum() const
    {
        return maximum;
    }
    int getMinimum() const
    {
        return minimum;
    }
    int getOrientation() const
    {
        return orientation;
    }
    int getUnitIncrement() const
    {
        return unitIncrement;
    }
    int getViewableAmount() const
    {
        return viewableAmount;
    }
    int getValue() const;

    void setBlockIncrement(int blockIncrement)
    {
        ScrollBar::blockIncrement = blockIncrement;
    }
    void setMaximum(int maximum)
    {
        ScrollBar::maximum = maximum;
    }
    void setMinimum(int minimum)
    {
        ScrollBar::minimum = minimum;
    }
    void setOrientation(Orientation orientation)
    {
        ScrollBar::orientation = orientation;
    }
    void setUnitIncrement(int unitIncrement)
    {
        ScrollBar::unitIncrement = unitIncrement;
    }
    void setViewableAmount(int viewableAmount)
    {
        ScrollBar::viewableAmount = viewableAmount;
    }

    virtual void draw(Surface &dest);
    virtual void render()
    {
        
    }
    virtual void actionPerformed(const mMouseEvent& me);
}; // end ScrollBar

#endif // end __ScrollBar_hpp__
