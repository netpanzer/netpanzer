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

#ifndef __Button_hpp__
#define __Button_hpp__

#include "Component.hpp"
#include "2D/Color.hpp"
#include "ViewGlobals.hpp"

typedef enum
{
    BNORMAL =   0,
    BOVER   =   1,
    BPRESSED=   2
} ButtonState;

//--------------------------------------------------------------------------
class Button : public Component
{
private:
    int extraBorder;
protected:
    std::string label;

    PIX borders[3][2];
    PIX textColors[3];

    Surface bimage;

    void render();
    ButtonState bstate;

    void resetState()
    {
        bstate = BNORMAL;
        dirty = true;
    }

public:

    Button(const std::string &cname);

    virtual ~Button() {}

    static Button * createTextButton(   std::string cname,
                                        std::string label,
                                        iXY loc,
                                        int bwidth);

    static Button * createSpecialButton(std::string cname,
                                        std::string label,
                                        iXY loc);

    /*static Button * createSpecialUSButton(std::string cname,
                                        std::string label,
                                        iXY loc);
                                        */

    void setTextColors( PIX normal, PIX over, PIX pressed)
    {
        textColors[BNORMAL] = normal;
        textColors[BOVER] = over;
        textColors[BPRESSED] = pressed;
    }

    void setLabel(const std::string& l)
    {
        label = l;
        dirty = true;
    }

    void setImage(const Surface &s)
    {
        if ( s.getNumFrames() ) {
            bimage.copy(s);
            setSize(bimage.getWidth(), bimage.getHeight());
        }
        dirty = true;
    }
    void clearImage()
    {
//        bimage.freeFrames();
    }

    void setUnitSelectionBorder()
    {
        setExtraBorder();
        borders[0][0] = Color::darkGray;
        borders[0][1] = Color::darkGray;
        borders[1][0] = Color::red;
        borders[1][1] = Color::red;
        borders[2][0] = Color::darkGray;
        borders[2][1] = Color::darkGray;
        dirty = true;
    }

    void setNormalBorder()
    {
        setExtraBorder();
        borders[0][0] = topLeftBorderColor;
        borders[0][1] = bottomRightBorderColor;
        borders[1][0] = topLeftBorderColor;
        borders[1][1] = bottomRightBorderColor;
        borders[2][0] = bottomRightBorderColor;
        borders[2][1] = topLeftBorderColor;
        dirty=true;
    }

    void setRedGreenBorder()
    {
        setExtraBorder();
        borders[0][0] = 0;
        borders[0][1] = 0;
        borders[1][0] = Color::red;
        borders[1][1] = Color::darkRed;
        borders[2][0] = Color::green;
        borders[2][1] = Color::darkGreen;
    }

    void clearBorder()
    {
        memset(borders, 0, sizeof(borders));
        dirty = true;
    }

    void setSize(int x, int y)
    {
        Component::setSize(x+(extraBorder*2), y+(extraBorder*2));
    }

    void setTextButtonSize(int xsize)
    {
        Component::setSize(xsize+(extraBorder*2), Surface::getFontHeight() + 4 + (extraBorder*2));
    }

    void setExtraBorder()
    {
        if ( !extraBorder )
        {
            extraBorder = 1;
            setSize( size.x, size.y);
        }
    }

    void clearExtraBorder()
    {
        if ( extraBorder )
        {
            extraBorder = 0;
            setSize( size.x, size.y);
        }
    }

    const std::string& getLabel() const
    {
        return label;
    }

    virtual void actionPerformed(const mMouseEvent &me);
}; // end Button

#endif // end __Button_hpp__
