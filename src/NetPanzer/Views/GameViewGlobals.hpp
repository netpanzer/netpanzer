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
#ifndef __GameViewGlobals_hpp__
#define __GameViewGlobals_hpp__

#include "2D/Surface.hpp"
#include "Types/iRect.hpp"
#include "Types/iXY.hpp"
#include "Components/ViewGlobals.hpp"

// Globals meter colors.
#define windowTextColor             (Color::black)
#define windowTextColorShadow       (Color::white)
#define meterColor                  (componentBodyColor)
#define meterTextColor              (Color::darkGray)
#define meterTopLeftBorderColor     (Color::gray)
#define meterBottomRightBorderColor (Color::black)
#define viewHeadingColor            (Color::tan) //tan
#define meterWidth                  (280)

#define optionsMeterWidth           (148)
#define optionsMeterStartX          (140)
#define optionsColorMeterStartX     (342)

#define ButonWidth                  (155)
#define ButonHeight                  (30)

// Button placement locations.
extern iXY exitPos;
extern iXY readyPos;
extern iXY playPos;
extern iXY mainTopButtonsStartPos;
extern iXY multiPos;
extern iXY creditsPos;
extern iXY joinPos;
extern iXY hostPos;
extern iXY controlsPos;
extern iXY returnToGamePos;
extern iXY resignPos;

// This is the text area in the menus.
extern iRect bodyTextRect;
extern iRect MenuRect;
extern iRect MenuRectStart;
// These are the possible background colors for the menus.
enum
{
    VIEW_BACKGROUND_DARK_GRAY_BLEND,
    VIEW_BACKGROUND_LIGHT_GRAY_BLEND,
    VIEW_BACKGROUND_SOLID_BLACK,
    VIEW_BACKGROUND_TRANSPARENT,
    VIEW_BACKGROUND_COUNT
};

void bltViewBackground(Surface &dest);
void bltBlendRect(Surface &dest, const iRect &r);

// Is the game help currently open?
extern bool gDrawGameHelp;

#endif // __GameViewGlobals_hpp__
