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
#ifndef __Palette_hpp__
#define __Palette_hpp__

#include <string>
#include <stdio.h>

#include "Color.hpp"
#include "ColorTable.hpp"

const size_t PALETTE_LENGTH = 256;

//--------------------------------------------------------------------------
class Palette
{
private:
    static float brightness;
    static std::string name;

public:
    Palette();

    // Best color match blending tables.
    static ColorTable	colorTable2080;
    static ColorTable	colorTable4060;
    static ColorTable	colorTable6040;
    static ColorTable	colorTable8020;

    // Used for not using transparency when blending.
    //static ColorTable	colorTableSolidTrans0;

    // Used until I get an assembly scaling routine.
    //static ColorTable	colorTableSolid;

    // Brighten.
    static ColorTable	colorTableBrighten;

    // Darken.
    static ColorTable	colorTableDarkenALittle;
    static ColorTable	colorTableDarkenALot;

    // LightDark
    static ColorTable	colorTableLightDark;

    // Specific color value tables.
    static ColorTable fire;
    static ColorTable gray16;
    static ColorTable gray64;
    static ColorTable gray128;
    static ColorTable gray256;
    static ColorTable darkGray256;
    static ColorTable brightness256;
    static ColorTable red256;
    static ColorTable green256;
    static ColorTable blue256;
    static ColorTable red32;
    static ColorTable green32;
    static ColorTable blue32;
    static ColorTable lightorange256;

    //static ColorTable earth256;  // Earth colors from sea 2 land 2 mountain 2 snow.

    // Gradient color tables.
    static ColorTable gradientWhite2Green;
    static ColorTable gradientWhite2Blue;
    static ColorTable gradientWhite2Red;

    // The current loaded palette.
    static SDL_Color color[PALETTE_LENGTH];         // This has any brightness value added to it.
    static SDL_Color originalColor[PALETTE_LENGTH]; // This is the original source loaded values.

    SDL_Color &operator [](size_t index)
    {
        assert(index < PALETTE_LENGTH);
        return color[index];
    }

    inline const SDL_Color &operator [](size_t index) const
    {
        assert(index < PALETTE_LENGTH);
        return color[index];
    }

    static void ramp(SDL_Color table [], int startRGB, int r1, int g1, int b1, int endRGB, int r2, int g2, int b2);
    static void loadACT(const std::string& filename);
    static Uint8 findNearestColor(int r, int g, int b, const bool &ignoreIndexZero = false);
    static void init(const std::string& name);
    static std::string getName()
    {
        return name;
    }
    static void setColors();
    static void setColorTables();

    static void setBrightnessAbsolute(float brightness);
};

#endif // end __Palette_hpp__
