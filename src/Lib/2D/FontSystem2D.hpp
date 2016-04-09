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
#ifndef __FontSystem2D_hpp__
#define __FontSystem2D_hpp__

#include "Types/iXY.hpp"
#include "Surface.hpp"

//--------------------------------------------------------------------------
class FontSystem2D
{
public:
    FontSystem2D(iXY pos, const char *string, const Surface &dest, float delay, PIX color)
    {
        create(pos, string, dest, delay, color);
    }
    FontSystem2D(int x, int y, const char *string, const Surface &dest, float delay, PIX color)
    {
        create(iXY(x, y), string, dest, delay, color);
    }

    FontSystem2D(iXY pos, const char *string, const Surface &dest, float delay, const ColorTable &colorTable)
    {
        create(pos, string, dest, delay, colorTable);
    }
    FontSystem2D(int x, int y, const char *string, const Surface &dest, float delay, const ColorTable &colorTable)
    {
        create(iXY(x, y), string, dest, delay, colorTable);
    }

    void create(iXY pos, const char *string, const Surface &dest, float delay, PIX color);
    void create(iXY pos, const char *string, const Surface &dest, float delay, const ColorTable &colrorTable);

    virtual ~FontSystem2D();

    static FontSystem2D *const zFontSystem2D;
    static FontSystem2D *firstFontSystem2D;
    FontSystem2D *next;
    FontSystem2D *prev;

    static float scrollSpeed;

    static void     removeAll();
    static void     simAll();
    static void     drawAll();
    static unsigned getFontSystemCount()
    {
        return fontSystemCount;
    }


protected:

    static unsigned fontSystemCount;

    bool     isAlive;         // Is this system alive.  When false, sim will destroy this system.
    float    age;             // How old the system is.
    float    lifetime;        // How long the system will live.
    float    delay;           // Time between wipe.
    float    groupWait;       // Time we have waited.
    int      charCount;       // How many characters to be displayed so far.
    iXY      pos;             // The position to draw the system on the surface.
    Surface  destSurface;     // Which surface to draw this system on.
    char    *string;          // The string to draw.
    int      stringLength;    // The length of the string to draw.

    void reset();

    virtual void sim();
    virtual void draw();

    void insertMe();
    void removeMe();

}
; // end FontSystem2D

#endif // __FontSystem2D_hpp__
