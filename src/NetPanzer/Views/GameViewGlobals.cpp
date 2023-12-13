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


#include "2D/Palette.hpp"
#include "Views/GameViewGlobals.hpp"
#include "Interfaces/GameConfig.hpp"

iRect MenuRect(80, 80, 80+640, 80+470);
iRect MenuRectStart(80, 80, 80+640, 80+5);
iRect bodyTextRect(MenuRect.min.x +10, MenuRect.min.y+10, MenuRect.max.x-10, MenuRect.max.y-10);

iXY mainTopButtonsStartPos(MenuRect.min.x+8, MenuRect.min.y-26);
iXY joinPos(mainTopButtonsStartPos.x+54, mainTopButtonsStartPos.y);
iXY hostPos(joinPos.x+54, mainTopButtonsStartPos.y);
iXY exitPos(MenuRect.max.x-125, mainTopButtonsStartPos.y);
iXY playPos(MenuRect.max.x-58, MenuRect.max.y);
iXY returnToGamePos(mainTopButtonsStartPos);
iXY resignPos(MenuRect.min.x+134, mainTopButtonsStartPos.y);
iXY readyPos(480, 32);
iXY backPos(480, 575);

bool gDrawGameHelp = false;

//--------------------------------------------------------------------------
void bltViewBackground(Surface &dest)
{
    bltBlendRect(dest, dest.getRect());
}

//--------------------------------------------------------------------------
void bltBlendRect(Surface &dest, const iRect &r)
{
    if (GameConfig::interface_viewdrawbackgroundmode == VIEW_BACKGROUND_DARK_GRAY_BLEND) {
        dest.BltRoundRect(r, 10, Palette::darkGray256.getColorArray());
    } else if (GameConfig::interface_viewdrawbackgroundmode == VIEW_BACKGROUND_LIGHT_GRAY_BLEND) {
        dest.BltRoundRect(r, 10, Palette::gray256.getColorArray());
    } else if (GameConfig::interface_viewdrawbackgroundmode == VIEW_BACKGROUND_SOLID_BLACK) {
        dest.fillRect(r, Color::black);
    } else if (GameConfig::interface_viewdrawbackgroundmode == VIEW_BACKGROUND_TRANSPARENT) {}
}
