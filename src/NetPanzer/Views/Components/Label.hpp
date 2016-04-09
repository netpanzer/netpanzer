/*
 Copyright (C) 2008 by Aaron Perez <aaronps@gmail.com>
 
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

#ifndef _LABEL_HPP_
#define _LABEL_HPP_

#include "Component.hpp"
#include "Core/CoreTypes.hpp"

class Label:public Component
{
public:
    Label(int x, int y, const NPString& t, PIX color, PIX bgcolor=Color::black, bool shadow=false)
    {
        text = t;
        background = bgcolor;
        foreground = color;
        shadowed = shadow;
        position.x = x;
        position.y = y;
    }
    
    void draw(Surface &dest);
    
    virtual void render()
    {
        // nothing
    }
    
    void actionPerformed(const mMouseEvent &me)
    {
        // nothing
    }
    
private:
    NPString text;
    bool shadowed;
};

#endif
