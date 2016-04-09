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

#include "Color.hpp"
#include "Palette.hpp"
#include "Scripts/ScriptManager.hpp"
#include "Scripts/ScriptHelper.hpp"

// Here is some preprocessor abuse

#define GEN_COLORS(GENERATOR) \
    GENERATOR(unitAqua) \
    GENERATOR(unitYellow) \
    GENERATOR(unitRed) \
    GENERATOR(unitBlue) \
    GENERATOR(unitDarkBlue) \
    GENERATOR(unitLightGreen) \
    GENERATOR(unitGreen) \
    GENERATOR(unitBlueGray) \
    GENERATOR(unitDarkRed) \
    GENERATOR(unitBlack) \
    GENERATOR(unitDarkGreen) \
    GENERATOR(unitWhite) \
    GENERATOR(unitLightOrange) \
    GENERATOR(unitOrange) \
    GENERATOR(unitGray) \
    GENERATOR(unitDarkGray) \
    GENERATOR(black) \
    GENERATOR(blue) \
    GENERATOR(brown) \
    GENERATOR(cyan) \
    GENERATOR(gray) \
    GENERATOR(green) \
    GENERATOR(magenta) \
    GENERATOR(orange) \
    GENERATOR(pink) \
    GENERATOR(red) \
    GENERATOR(white) \
    GENERATOR(yellow) \
    GENERATOR(tan) \
    GENERATOR(chartreuse) \
    GENERATOR(cobaltGreen) \
    GENERATOR(emeraldGreen) \
    GENERATOR(forestGreen) \
    GENERATOR(darkOliveGreen) \
    GENERATOR(terreVerte) \
    GENERATOR(darkBlue) \
    GENERATOR(darkBrown) \
    GENERATOR(darkCyan) \
    GENERATOR(darkGray) \
    GENERATOR(darkGreen) \
    GENERATOR(darkMagenta) \
    GENERATOR(darkOrange) \
    GENERATOR(darkPink) \
    GENERATOR(darkRed) \
    GENERATOR(darkYellow) \
    GENERATOR(lightBlue) \
    GENERATOR(lightBrown) \
    GENERATOR(lightCyan) \
    GENERATOR(lightGray) \
    GENERATOR(lightGreen) \
    GENERATOR(lightMagenta) \
    GENERATOR(lightOrange) \
    GENERATOR(lightPink) \
    GENERATOR(lightRed) \
    GENERATOR(lightYellow) \
    GENERATOR(gray32) \
    GENERATOR(gray64) \
    GENERATOR(gray96) \
    GENERATOR(gray128) \
    GENERATOR(gray160) \
    GENERATOR(gray192) \
    GENERATOR(gray224)

// Have to merge like this, cant use ##
#define MERGE_CLASS(a) a::
#define GEN_VAR(CNAME) Uint8 MERGE_CLASS(Color)CNAME;

#define GEN_GETSTRUCT(CNAME) { #CNAME, GETSVTYPE_BYTE, &MERGE_CLASS(Color)CNAME },
#define GEN_SETSTRUCT(CNAME) { #CNAME, SETSVTYPE_BYTE, &MERGE_CLASS(Color)CNAME },

// This generate the Uint8 Color::colorname for all the colors
// Must not have ';' at end
GEN_COLORS(GEN_VAR)

// This generates the tables needed for script binding
static const ScriptVarBindRecord color_getters[] =
{
    GEN_COLORS(GEN_GETSTRUCT)
    {0,0}
};
        
static const ScriptVarBindRecord color_setters[] =
{
    GEN_COLORS(GEN_SETSTRUCT)
    {0,0}
};

static int color_rgb(lua_State *L)
{
    int r = luaL_checkint(L,1); // r
    int g = luaL_checkint(L,2); // g
    int b = luaL_checkint(L,3); // b

    int color = Palette::findNearestColor(r,g,b, true);

    lua_pushinteger(L, color);
    return 1;
}

static const luaL_Reg color_methods[] =
{
    { "rgb", color_rgb },
    { 0, 0}
};

void
Color::registerScript(const NPString& table_name)
{
    ScriptManager::registerLib( table_name, color_methods);
    ScriptManager::bindStaticVariables(table_name, "ColorMetaTable",
                                       color_getters, color_setters);
}

        
