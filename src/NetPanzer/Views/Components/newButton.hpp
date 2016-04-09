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

#ifndef __newButton_hpp__
#define __newButton_hpp__

#include "Component.hpp"
#include "Button.hpp"
#include "2D/Color.hpp"
#include "ViewGlobals.hpp"

//--------------------------------------------------------------------------
class newButton : public Button
{
protected:
    void render();
public:
    newButton(std::string cname, std::string Title, iXY loc, int down)
        : Button(cname)
    {
        setLocation(loc);
        setSize(Surface::getTextLength(Title)+20, 25);
        label = Title;
        setTextColors(Color::gray, Color::lightGray, Color::black);
        setExtraBorder();
        borders[0][0] = Color::gray;
        borders[0][1] = Color::black;
        borders[1][0] = Color::gray;
        borders[1][1] = Color::gray;
        borders[2][0] = Color::gray;
        borders[2][1] = Color::darkGray;
        Surface bnormal;
        bnormal.loadBMP("pics/backgrounds/menus/buttons/default/page.bmp");
        Surface spbutton(Surface::getTextLength(Title)+20, 25, 1);
        //spbutton.setFrame(0);
        if (down) bnormal.flipVertical();
        bnormal.blt(spbutton,0,0);
        setImage(spbutton);
    }
};

#endif // end __Button_hpp__
  