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


#include "System/SDLVideo.hpp"
#include "Classes/ScreenSurface.hpp"

ScreenSurface* screen = 0;

ScreenSurface::ScreenSurface(SDLVideo* newdraw, int width, int height)
        : Surface(), draw(newdraw)
{
    myMem = false;
    numFrames = 1;

    twidth =width;
    theight=height;
    tpitch = width;
    
    numFrames = 1;
    doesExist = false;
}

void ScreenSurface::lock()
{
    // XXX HERE HERE 
    assert(doesExist == false);
    frame0 = (PIX*) draw->getSurface()->pixels;
    mem = frame0;
    tpitch = draw->getSurface()->pitch;
    doesExist = true;
}

void ScreenSurface::unlock()
{
    assert(doesExist == true);
    doesExist = false;
}

void ScreenSurface::render() {
    draw->render();
}
