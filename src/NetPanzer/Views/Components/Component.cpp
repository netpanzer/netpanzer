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


#include "Component.hpp"
#include "ViewGlobals.hpp"
#include "Views/Components/View.hpp"

int Component::borderSize = 12;

// reset
//---------------------------------------------------------------------------
void Component::reset()
{
    foreground = gForeground;
    background = gBackground;
    size.zero();
    position.zero();
    enabled    = true;
    visible    = true;
    parent     = 0;
    surface.free();

} // end Component::reset

// contains
//---------------------------------------------------------------------------
bool Component::contains(int x, int y) const
{
    return (x >= position.x &&
            x < position.x + size.x &&
            y >= position.y &&
            y < position.y + size.y);

} // end Component::contains

//---------------------------------------------------------------------------
void Component::setParent(void *parent)
{
    assert(parent != 0);

    Component::parent = parent;
}

// setLocation
//---------------------------------------------------------------------------
void Component::setLocation(int x, int y)
{
    position.x = x;
    position.y = y;

} // end Component::setLocation

