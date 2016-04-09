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
#ifndef __SDLVideo_hpp__
#define __SDLVideo_hpp__

#include "SDL.h"

// DirectDraw class declarations
//---------------------------------------------------------------------------
class SDLVideo
{
private:
    SDL_Surface* frontBuffer;
    SDL_Surface* backBuffer;

public:
    SDLVideo();
    virtual ~SDLVideo();

    void setVideoMode(int width, int height, int bpp, Uint32 flags);
    bool isDisplayModeAvailable(int width, int height, int bpp, Uint32 flags);
    void lockDoubleBuffer(unsigned char **DoubleBuffer);
    void unlockDoubleBuffer();
    void copyDoubleBufferandFlip();
    void setPalette(SDL_Color *color);
    void doScreenshoot();

    SDL_Surface* getSurface();
}; // end DirectDraw

extern SDLVideo* Screen;

#endif // end __UIDraw_hpp__
