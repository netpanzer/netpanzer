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


#include "Views/Components/Button.hpp"
#include "Views/Components/View.hpp"
#include "Util/Log.hpp"

#include "MouseEvent.hpp"

Button::Button(const std::string &cname)
        : Component("Button." + cname)
{
    setTextColors(Color::white, Color::red, Color::yellow);
    position.zero();
    label.clear();
    bstate = BNORMAL;
    memset(borders, 0, sizeof(borders));
    extraBorder = 0;
    dirty = true;
}

// render
void
Button::render()
{
    surface.fill(0);
    if ( bimage.getNumFrames() == 1 )
    {
        bimage.bltTrans(surface, extraBorder, extraBorder);
    }
    else if ( bimage.getNumFrames() == 3 )
    {
        bimage.setFrame(bstate);
        bimage.bltTrans(surface, extraBorder, extraBorder);
    }
    else
    {
        surface.fill(componentBodyColor);        
    }

    if ( borders[bstate][0]|extraBorder ) // only 1 | (binary or)
    {
        surface.drawButtonBorder(borders[bstate][0], borders[bstate][1]);
    }
   
    if ( label.length() )
    {
        Surface text;
        text.renderText( label.c_str(), textColors[bstate], 0);
        // blit centered and transparent
        text.bltTrans(surface, (surface.getWidth()/2) - (text.getWidth()/2),
                      (surface.getHeight()/2) - (text.getHeight()/2));        
    }
   
    dirty = false;
}

// actionPerformed
//---------------------------------------------------------------------------
void
Button::actionPerformed(const mMouseEvent &me)
{
    if (me.getID() == mMouseEvent::MOUSE_EVENT_ENTERED
                || me.getID() == mMouseEvent::MOUSE_EVENT_RELEASED)
    {
        bstate = BOVER;
        dirty = true; // draw text in red
    }
    else if (me.getID() == mMouseEvent::MOUSE_EVENT_EXITED)
    {
        bstate = BNORMAL;
        dirty = true; // draw defaults;
    }
    else if (me.getID() == mMouseEvent::MOUSE_EVENT_PRESSED)
    {
        bstate = BPRESSED;
        dirty = true;
    }
    else if (me.getID() == mMouseEvent::MOUSE_EVENT_CLICKED)
    {
        ((View *)parent)->onComponentClicked(this);
    }
} // end Button::actionPerformed

Button *
Button::createTextButton(   std::string cname,
                            std::string label,
                            iXY loc,
                            int bwidth)
{
    Button * b = new Button(cname);
    b->setLabel(label);
    b->setLocation(loc);
    b->setTextButtonSize(bwidth);
    b->setNormalBorder();
    return b;
}

Button *
Button::createSpecialButton( std::string cname,
                             std::string label,
                             iXY loc)
{
    Surface bnormal;
    bnormal.loadBMP("pics/backgrounds/menus/buttons/default/button.bmp");
    
    Surface bhover;
    bhover.loadBMP("pics/backgrounds/menus/buttons/default/button1.bmp");
    
    Surface bclick;
    bclick.loadBMP("pics/backgrounds/menus/buttons/default/button2.bmp");
    
    Surface spbutton(bnormal.getWidth(), bnormal.getHeight(), 3);

    spbutton.setFrame(0);
    bnormal.blt(spbutton,0,0);
    spbutton.setFrame(1);
    bhover.blt(spbutton,0,0);
    spbutton.setFrame(2);
    bclick.blt(spbutton,0,0);
    spbutton.setFrame(0);

    Button *b = new Button(cname);
    b->setImage(spbutton);
    b->setLabel(label);
    b->setLocation(loc);
    b->setTextColors(Color::yellow, Color::white, Color::white);

    return b;
}