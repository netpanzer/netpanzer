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


#include "UnitColorView.hpp"
#include "Views/Game/GameView.hpp"
#include "Views/GameViewGlobals.hpp"
#include "2D/Palette.hpp"
#include "2D/ColorTable.hpp"

static float maxPercent       = 3.5f;
static float minPercent       = 0.5f;
static float incrementPercent = 0.025f;
static float grayPercent      = 2.0f;
static ColorTable *colorTable;
ColorTable gray256;

static void setGrayColorTable()
{
    colorTable = &gray256;
}

static void increasePercent(float &color)
{
    if ((color += incrementPercent) > maxPercent) {
        color = maxPercent;
    }
}

static void decreasePercent(float &color)
{
    if ((color -= incrementPercent) < minPercent) {
        color = minPercent;
    }
}

static void rebuildGrayColorTable()
{
    // 256 shades of gray.
    gray256.init(256);
    for (int num = 0; num < 256; num++) {
        int c = int(Palette::color[num].r+Palette::color[num].g+Palette::color[num].b)/3; //brightness

        int nearestColor = Palette::findNearestColor(
                        int(c * grayPercent),
                        int(c * grayPercent),
                        int(c * grayPercent));
        gray256.setColor(num, nearestColor);
    }
    gray256.setColor(255, 0);
}

static void bIncreaseBrightness()
{
    increasePercent(grayPercent);
    rebuildGrayColorTable();
}

static void bDecreaseBrightness()
{
    decreasePercent(grayPercent);
    rebuildGrayColorTable();
}

// UnitColorView
//---------------------------------------------------------------------------
UnitColorView::UnitColorView() : View()
{
    setSearchName("UnitColorView");
    setTitle("Selects Your Unit Color");
    setSubTitle("");

    setAllowResize(false);
    setAllowMove(false);
    setVisible(false);

    Surface unitSurface;

    moveTo(400, 200);

    packedBody.load("units/pics/pak/original/TitaHNSD.pak");
    packedBody.setFPS(12);

    packedTurret.load("units/pics/pak/original/TitaTNSD.pak");
    packedTurret.setFPS(8);

    grassSurface.loadBMP("pics/grass.bmp");

    fuckingSurface.create(packedTurret.getWidth(), packedTurret.getHeight(), 1);
    fuckingSurface.fill(0);

    resizeClientArea(packedTurret.getWidth(), packedTurret.getHeight());

    int size = packedTurret.getWidth() / 2;
    addButtonCenterText(iXY(0, 0), size, "Up", "", bIncreaseBrightness);
    addButtonCenterText(iXY(size, 0), size, "Down", "", bDecreaseBrightness);

    setGrayColorTable();
    rebuildGrayColorTable();

} // end UnitColorView::UnitColorView

// doDraw
//---------------------------------------------------------------------------
void UnitColorView::doDraw(Surface &viewArea, Surface &clientArea)
{
    //unitSurface.fill(0);

    packedBody.nextFrame();
    packedTurret.nextFrame();

    fuckingSurface.fill(Color::white);
    packedBody.blt(fuckingSurface, 0, 0);
    packedTurret.blt(fuckingSurface, 0, 0);
    fuckingSurface.bltLookup(fuckingSurface.getRect(), colorTable->getColorArray());

    grassSurface.blt(clientArea, 0, 0);
    fuckingSurface.bltTrans(clientArea, 0, 0);

    char strBuf[256];
    sprintf(strBuf, "%1.3f of %1.3f", grayPercent, maxPercent);
    clientArea.bltString(2, fuckingSurface.getHeight() - Surface::getFontHeight() - 1, strBuf, Color::white);

    View::doDraw(viewArea, clientArea);

} // end UnitColorView::doDraw

//---------------------------------------------------------------------------
void UnitColorView::rMouseDown(const iXY &pos)
{
    View::lMouseDown(pos);
}

//---------------------------------------------------------------------------
void UnitColorView::rMouseUp(const iXY &downPos, const iXY &upPos)
{
    View::lMouseUp(downPos, upPos);
}

//---------------------------------------------------------------------------
void UnitColorView::rMouseDrag(const iXY &downPos, const iXY&, const iXY &newPos)
{
    View::lMouseUp(downPos, newPos);
    View::lMouseDown(newPos);
}
