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
#ifndef __Surface_hpp__
#define __Surface_hpp__

#include <string>
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#include "Types/iXY.hpp"
#include "Types/iRect.hpp"
#include "Util/NoCopy.hpp"

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

class ColorTable;
class Palette;
typedef Uint8 PIX;

// This must be called before any of the string blitting functions are used.
void initFont();

/////////////////////////////////////////////////////////////////////////////
// class Surface
/////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------
class Surface : public NoCopy
{
public:
    static int getTotalSurfaceCount()   { return totalSurfaceCount; }
    static int getTotalByteCount()      { return totalByteCount; }

    Surface();
    Surface(unsigned int w, unsigned int h, unsigned int nframes);

    virtual ~Surface();

    void create(unsigned int w, unsigned int h, unsigned int nframes);
    void free();

    void setOffset(const iXY &o) { offset = o; }
    void setOffsetX(int ox)      { offset.x = ox; }
    void setOffsetY(int oy)      { offset.y = oy; }
    void setFPS(unsigned int f)  { fps = f; }
    void setOffsetCenter();

    // Accessor functions.
    unsigned int getWidth()  const { return twidth; }
    unsigned int getHeight() const { return theight; }
    unsigned int getPitch()  const { return tpitch; }

    iXY     getCenter()     const { return iXY(getWidth()>>1, getHeight()>>1); }
    int     getCenterX()    const { return getWidth()>>1; }
    int     getCenterY()    const { return getHeight()>>1; }

    unsigned int getNumFrames() const { return numFrames; }

    float  getFPS()         const { return fps; }
    bool   getDoesExist()   const { return doesExist; }
    int    getCurFrame ()   const { return (int) curFrame; }
    PIX   *getMem()         const { return mem; }
    PIX   *getFrame0()      const { return frame0; }
    iXY    getOffset()      const { return offset; }
    int    getOffsetX()     const { return offset.x; }
    int    getOffsetY()     const { return offset.y; }
    int    getArea()        const { return getWidth() * getHeight(); }

    float  getAspectXOverY() const
    {
        return float(getWidth()) / float(getHeight());
    }
    float  getAspectYOverX() const
    {
        return float(getHeight()) / float(getWidth());
    }
    iRect  getRect() const
    {
        return iRect(0, 0, getWidth() - 1, getHeight() - 1);
    }

    void putPixel(unsigned int x, unsigned int y, const PIX &color)
    {
        if ( x >= getWidth() || y >= getHeight())
            return;
        *(mem + (y * getPitch()) + x) = color;
    }

    PIX getPixel(unsigned int x, unsigned int y) const
    {
        return *pixPtr(x,y);
    }

    void drawRect(iRect bounds, const PIX &color);
    void fillRect(iRect bounds, const PIX &color);

private:
    friend class ScreenSurface;
    friend class PackedSurface;
    PIX   *mem;       // Pointer to upperleft most pixel
    PIX   *frame0;    // Pointer to first frame

    void alloc(unsigned int w, unsigned int h, int nframes);
    bool grab(const Surface &s, iRect bounds);
    PIX *pixPtr(unsigned int x, unsigned int y) const;

protected:
    iXY   offset;     // Used like a hot spot for drawing.
    float fps;        // The speed to change the frames. Make sure TimerInterface::getTimeSlice() is being updated.
    unsigned int twidth;
    unsigned int theight;
    unsigned int tpitch;     // Number of bytes from one row to the next.
    unsigned int numFrames;

    float curFrame;   // Current frame of frameCount.
    bool  myMem;	  // Am I the owner of this surface, or am I mapped to some other surface.
    bool  doesExist;  // Is it ok o write to this surface?

    static int totalSurfaceCount;  // The number of surfaces alive.
    static int totalByteCount;     // The number of bytes of the surfaces alive.

    void        reset();

public:

    void resize(int xPix, int yPix);

    void setTo(const Surface &s, iRect bounds);
    void setTo(const Surface &source);

    void blt(Surface &dest, int x, int y) const;
    void bltTrans(Surface &dest, int x, int y) const;
    void bltTransColor(Surface &dest, int x, int y, const PIX color) const;
    void bltTransColorFromSDLSurface(SDL_Surface *source, int x, int y, const PIX color) const;
    void bltScale(const Surface &source, const iRect &destRect);
    void bltLookup(const iRect &destRect, const PIX table[]);


    void drawHLine(int x1, int y,  int x2, const PIX &color);
    void drawVLine(int x,  int y1, int y2, const PIX &color);

    void drawLine(int x1, int y1, int x2, int y2, const PIX &color);

    void drawLine(const iXY &a, const iXY &b, const PIX &color)
    {
        drawLine(a.x, a.y, b.x, b.y, color);
    }

    // Surface Effects.
    void drawButtonBorder(iRect bounds, PIX topLeftColor, PIX bottomRightColor);
    void drawButtonBorder(PIX topLeftColor, PIX bottomRightColor)
    {
        drawButtonBorder(iRect(0, 0, getWidth(), getHeight()), topLeftColor, bottomRightColor);
    }

    void drawWindowsBorder();

    void fill(const PIX &color);
    void flipVertical();
    void rotate(int angle);
    void copy(const Surface &source);

    int nextFrame();

    void setFrame(const float &frameNum)
    {
        assert(frameNum >= 0.0);
        assert(frameNum < getNumFrames());
        mem = frame0 + (getHeight() * getPitch()) * int(frameNum);
    }

    PIX getAverageColor();

    void scale(unsigned int x, unsigned int y);

    void shrinkWrap();

    // Text rendering functions
    void renderText(const char *str, PIX color, PIX bgcolor);

    void bltString(int x, int y, const char * str, const PIX& color);
    void bltStringInBox(const iRect &rect, const char *string, PIX color, int gapSpace = 14, bool drawBox = false);

    // Blit a shadowed string of text.
    void bltStringShadowed(int x, int y, const char *str, const PIX &textColor, const PIX &shadowColor);

    // Blits a string of text and centers it horizontally and vertically on the screen.
    void bltStringCenter(const char *string, PIX color);
    void bltStringCenterMin30(const char *string, PIX color);
    void bltStringShadowedCenter(const char *string, PIX foreground, PIX background);
    void bltStringCenteredInRect(const iRect &rect, const char *string, const PIX &color);

    void loadBMP(const char *fileName, bool needAlloc = true);

    void drawBoxCorners(const iRect &rect, int cornerLength, PIX color);
    void drawBoxCorners(const iXY &min, const iXY &max,
            int cornerLength, PIX color)
    {
        drawBoxCorners(iRect(min.x, min.y, max.x, max.y), cornerLength, color);
    }
    void drawBoxCorners(int cornerLength, PIX color)
    {
        drawBoxCorners(iRect(0, 0, getWidth() - 1, getHeight() - 1), cornerLength, color);
    }

    static unsigned int getFontHeight();
    static unsigned int getTextLength(const char* text);
    static unsigned int getTextLength(const std::string& text)
    {
        return getTextLength(text.c_str());
    }

    void circle(int cx, int cy, int radius, PIX color);
    void FillCircle(int cx, int cy, int radius, PIX color);
    void BltCircle(int cx, int cy, int radius, const PIX table[]);
    void RoundRect(iRect rect, int radius, PIX color);
    void FillRoundRect(iRect rect, int radius, PIX color);
    void BltRoundRect(iRect rect, int radius, const PIX table[]);
    void bltHLine(int x1, int y, int x2, const PIX table[]);

    void frameToBuffer(Uint8* dest, size_t dest_len)
    {
        size_t frame_len = getPitch()*getHeight();
        memcpy(dest, mem, std::min(frame_len, dest_len));
    }

    void bufferToFrame(const Uint8* src, const size_t src_len)
    {
        size_t frame_len = getPitch()*getHeight();
        memcpy(mem, src, std::min(frame_len, src_len));
    }

}; // end Surface

#endif // __Surface_HPP__
