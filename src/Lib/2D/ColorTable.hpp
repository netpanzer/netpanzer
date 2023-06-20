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
#ifndef __ColorTable_hpp__
#define __ColorTable_hpp__

#include <stdio.h>
#include <assert.h>
#include <SDL2/SDL.h>

//--------------------------------------------------------------------------
class ColorTable
{
    friend class Palette;

protected:
    static const char *extension;

    int   colorCount;
    Uint8 *colorArray;

    static int totalColorArrayCount;
    static int totalByteCount;

public:
    ColorTable();
    ~ColorTable();

    void init(int colorCount);
    void setColor(int index, Uint8 color);

    // Give us an array index into the table.
    inline Uint8 operator[](int index) const
    {
        assert(index < colorCount);
        return *(colorArray + index);
    }

    inline int getColorCount()
    {
        return colorCount;
    }
    inline const Uint8 *getColorArray()
    {
        return colorArray;
    }

    static int getTotalColorArrayCount()
    {
        return totalColorArrayCount;
    }
    static int getTotalByteCount()
    {
        return totalByteCount;
    }

    void create(const int color1Percent, const int color2Percent, const char *filename);
    void createTrans0(const int &color1Percent, const int &color2Percent, const char *filename);
    void createBrightenFilter(const char *filename, const int &brightness);
    void createDarkenFilter(const char *filename, float fudgeValue);
    void createLightDarkFilter(const char *filename);

    void loadTable(const char *filename);
    void saveTable(const char *filename) const;

private:
    void free();
}
; // end ColorTable

#endif // end __ColorTable_hpp__
