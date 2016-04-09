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
#ifndef __ViewGlobals_hpp__
#define __ViewGlobals_hpp__

#include "SDL.h"

#define topLeftBorderColor           (Color::lightGray)
#define bottomRightBorderColor       (Color::black)

#define componentBodyColor           (41)
#define componentTextColor           (Color::black)
#define componentInActiveTextColor   (Color::gray)
#define componentActiveTextColor     (Color::lightGray)
#define componentFocusTextColor      (Color::white)

#define activeWindowTitleTextColor   (Color::white)
#define activeWindowTitleBodyColor   (componentBodyColor)
#define inactiveWindowTitleTextColor (Color::gray)
#define inactiveWindowTitleBodyColor (componentBodyColor)

extern Uint8 gForeground;
extern Uint8 gBackground;

void setForeground(Uint8 color);
void setBackground(Uint8 color);

#endif // end __ViewGlobals_hpp__
