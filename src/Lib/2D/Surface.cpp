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


#include <math.h>
#include <ctype.h>
#include <memory>
#include <vector>
#include <string>
#include <algorithm>

#include "Util/Log.hpp"
#include "Util/FileSystem.hpp"
#include "Util/UtilInterface.hpp"
#include "Types/fXY.hpp"
#include "Util/TimerInterface.hpp"
#include "Util/Exception.hpp"
#include "Palette.hpp"
#include "Surface.hpp"
#include "Span.hpp"

using std::swap;
using std::min;
using std::max;

static TTF_Font* font;
#define FONT_SIZE 14
#define FONT_WIDTH 14

// orderCoords
//---------------------------------------------------------------------------
// Purpose: Orders a pair of (x,y) coordinates
//---------------------------------------------------------------------------
template <class T>
inline void orderCoords(T &a, T &b)
{
    if (a > b) swap(a, b);
} // end orderCoords

// orderCoords
//---------------------------------------------------------------------------
// Purpose: Orders a 2 pairs of (x,y) coordinates, making sure x1 <= x2 and y1 <= y2.
//---------------------------------------------------------------------------
template <class T>
inline void orderCoords(T &x1, T &y1, T &x2, T &y2)
{
    if (x1 > x2) swap(x1, x2);
    if (y1 > y2) swap(y1, y2);
} // end orderCoords

inline void orderCoords(iRect &bounds)
{
    if (bounds.min.x > bounds.max.x) {
        swap(bounds.min.x, bounds.max.x);
    }
    if (bounds.min.y > bounds.max.y) {
        swap(bounds.min.y, bounds.max.y);
    }
} // end orderCoords

class BitmapFileHeader
{
public:
    Uint16    bfType;
    Uint32   bfSize;
    Uint16    bfReserved1;
    Uint16    bfReserved2;
    Uint32   bfOffBits;

    BitmapFileHeader(filesystem::ReadFile* file);
};

BitmapFileHeader::BitmapFileHeader(filesystem::ReadFile* file)
{
    bfType = file->readULE16();
    bfSize = file->readULE32();
    bfReserved1 = file->readULE16();
    bfReserved2 = file->readULE16();
    bfOffBits = file->readULE32();
}

#define BI_RGB      0L
#define BI_RLE8     1L
#define BI_RLE4     2L

class BitmapInfoHeader
{
public:
    Uint32  biSize;
    Uint32  biWidth;
    Uint32  biHeight;
    Uint16   biPlanes;
    Uint16   biBitCount;
    Uint32  biCompression;
    Uint32  biSizeImage;
    Uint32  biXPelsPerMeter;
    Uint32  biYPelsPerMeter;
    Uint32  biClrUsed;
    Uint32  biClrImportant;

    BitmapInfoHeader(filesystem::ReadFile* file);
};

BitmapInfoHeader::BitmapInfoHeader(filesystem::ReadFile* file)
{
    biSize = file->readULE32();
    biWidth = file->readULE32();
    biHeight = file->readULE32();
    biPlanes = file->readULE16();
    biBitCount = file->readULE16();
    biCompression = file->readULE32();
    biSizeImage = file->readULE32();
    biXPelsPerMeter = file->readULE32();
    biYPelsPerMeter = file->readULE32();
    biClrUsed = file->readULE32();
    biClrImportant = file->readULE32();
}

int Surface::totalSurfaceCount = 0;
int Surface::totalByteCount    = 0;

// Surface
//---------------------------------------------------------------------------

Surface::Surface()
{
    reset();

    totalSurfaceCount++;
    totalByteCount += sizeof(Surface);
} // end Surface::Surface

// Surface
//---------------------------------------------------------------------------
Surface::Surface(unsigned int w, unsigned int h, unsigned int nframes)
{
    reset();

    alloc( w, h, nframes);

    totalSurfaceCount++;
    totalByteCount += sizeof(Surface);
} // end Surface::Surface

// ~Surface
//---------------------------------------------------------------------------
Surface::~Surface()
{
    if ((doesExist != false) && (myMem != false)) {
        free();
    }

    totalSurfaceCount--;
    assert(totalSurfaceCount >= 0);

    totalByteCount -= sizeof(Surface);
    assert(totalByteCount >= 0);
} // end Surface::~Surface

//---------------------------------------------------------------------------
void Surface::free()
{
    if (myMem && frame0 != 0) {
        ::free(frame0);

        totalByteCount -= getPitch() * getHeight() * sizeof(PIX) * getNumFrames();

        assert(totalByteCount >= 0);
    }

    frame0     = 0;
    mem        = 0;
    myMem      = false;
    doesExist  = false;
    numFrames = 0;
}

// reset
//---------------------------------------------------------------------------
void Surface::reset()
{
    assert(this != 0);

    twidth      = 0;
    theight     = 0;
    tpitch      = 0;

    mem         = 0;
    frame0      = 0;
    myMem       = false;
    numFrames   = 0;
    curFrame    = 0;
    fps         = 0;
    offset.zero();
    doesExist   = 0;
} // end Surface::reset

// setOffsetCenter
//---------------------------------------------------------------------------
// Purpose: Set the offset to the center of the image.
//---------------------------------------------------------------------------
void Surface::setOffsetCenter()
{
    assert(getDoesExist());
    assert(this != 0);

    offset = iXY(-(getWidth()>>1), -(getHeight()>>1));

} // end Surface::setOffsetCenter

// alloc
//---------------------------------------------------------------------------
// Purpose: Allocates memory for the surface, while setting the pix.x, pix.y,
//          xCenter, yCenter, and stride.  If successful, true is returned,
//          otherwise false is returned.
//---------------------------------------------------------------------------
void
Surface::alloc(unsigned int w, unsigned int h, int nframes)
{
    assert(this != 0);

    free();

    twidth = w;
    theight= h;
    tpitch = getWidth();

    size_t requestedBytes = getPitch() * getHeight() * sizeof(PIX) * nframes;

    if (requestedBytes > 0) {
        frame0 = (PIX *) malloc(requestedBytes);

        if (frame0 == 0)
            throw Exception("out of memory while allocating surface.");

        totalByteCount += requestedBytes;
        myMem = true;
    }

    Surface::mem        = frame0;
    numFrames           = nframes;
    Surface::doesExist  = true;
} // end Surface::alloc

// resize
//---------------------------------------------------------------------------
// Purpose: Resizes the calling surface to a new length and width, while
//          remaining the same number of frames.
//---------------------------------------------------------------------------
void Surface::resize(int x, int y)
{
    assert(getDoesExist());
    assert(this != 0);

    create(x, y, getNumFrames());

} // end Surface::resize

// setTo
//---------------------------------------------------------------------------
// Purpose: Maps a Surface's coordinates to an existing surface.  This can
//          save you from having to allocate memory for every single surface.
//          You can just draw everything onto one surface in the given bounds.
//---------------------------------------------------------------------------
void Surface::setTo(const Surface &source, iRect bounds)
{
    assert(source.getDoesExist());
    assert(this != 0);

    free();
    orderCoords(bounds);

    myMem      = false;
    frame0     = source.pixPtr(bounds.min.x, bounds.min.y);
    mem	       = frame0;
    if ( (unsigned int)bounds.max.x > source.getWidth() )
        twidth = source.getWidth() - bounds.min.x;
    else
        twidth     = bounds.getSizeX();

    if ( (unsigned int)bounds.max.y > source.getHeight() )
        theight = source.getHeight() - bounds.min.y;
    else
        theight    = bounds.getSizeY();

    tpitch     = source.getPitch();
    numFrames  = source.getNumFrames();
    fps        = source.getFPS();

    doesExist  = source.getDoesExist();

} // end Surface::setTo

// setTo
//---------------------------------------------------------------------------
// Purpose: Maps the calling surface to some specified coordinates of the
//          another Surface.
//---------------------------------------------------------------------------
void Surface::setTo(const Surface &source)
{
    assert(this != 0);
    assert(source.getDoesExist());

    free();

    myMem      = false;
    frame0     = source.getFrame0();
    mem        = frame0;
    twidth     = source.getWidth();
    theight    = source.getHeight();
    tpitch     = source.getPitch();
    numFrames  = source.getNumFrames();
    fps        = source.getFPS();
    offset     = source.getOffset();
    doesExist  = source.getDoesExist();

} // end Surface::setTo

// grab
//---------------------------------------------------------------------------
// Purpose: Copies a section from another Surface.
//---------------------------------------------------------------------------
bool Surface::grab(const Surface &source,
                   iRect bounds)
{
    assert(source.getDoesExist());
    assert(this != 0);

    free();
    orderCoords(bounds);

    alloc(bounds.getSizeX(), bounds.getSizeY(), 1);

    // We can blit like this because everything will be clipped away for us.
    source.blt(*this, -bounds.min.x, -bounds.min.y);

    return true;

} // end Surface::grab

// pixPtr
//---------------------------------------------------------------------------
// Purpose: Gets a pixel from a given position.
//---------------------------------------------------------------------------
PIX *Surface::pixPtr(unsigned int x, unsigned int y) const
{
    assert((y * getPitch() + x) < getPitch() * getHeight());
    return mem + (y * getPitch()) + x;
} // end Surface::pixPtr

// blt
//---------------------------------------------------------------------------
// Purpose: Puts the surface onto the destination while performing clipping
//          on the bounds of the object.
//---------------------------------------------------------------------------
void Surface::blt(Surface &dest, int x, int y) const
{
    assert(getDoesExist());
    assert(dest.getDoesExist());
    assert(this != 0);
    assert(mem != 0);
    assert(dest.mem != 0);

    // Add in the offset factor.
    x+=offset.x;
    y+=offset.y;

    // Trivial clipping rejection - no overlap.
    // Also will jump out immediately if either image has zero size.
    if (x >= (int)dest.getWidth() || y >= (int)dest.getHeight())
        return;

    int end_x = x + getWidth();
    int end_y = y + getHeight();
    if ( end_x <= 0 || end_y <= 0 ) return;

    unsigned int pixelsPerRow = getWidth();
    unsigned int numRows      = getHeight();

    PIX	*sPtr	= mem;      // Pointer to source Surface start of memory.
    PIX	*dPtr	= dest.mem; // Pointer to destination Surface start of memory.

    // Check for partial clip, calculate number of pixels
    // per row to copy, and number of rows to copy.  Adjust
    // sPtr and dPtr.

    // CLIP LEFT
    if (x < 0) {
        pixelsPerRow +=  x; // This will subtract the neg. x value.
        sPtr         += -x; // This will move the sPtr to x = 0, from the neg. x.
    } else {
        dPtr += x;
    }

    // CLIP RIGHT
    // This subtracts only the portion hanging over the right edge of the
    // destination Surface
    if ((unsigned int)end_x > dest.getWidth())
        pixelsPerRow -= end_x - dest.getWidth();

    // CLIP TOP
    if (y < 0) {
        numRows += y;
        sPtr    -= y * (int)getPitch();
    } else {
        dPtr += y * (int)dest.getPitch();
    }

    // CLIP BOTTOM
    // This subtracts only the portion hanging over the bottom edge of the
    // destination Surface
    if ((unsigned int)end_y > dest.getHeight())
        numRows -= end_y - dest.getHeight();

    // Now, Check to make sure I actually have something
    // to draw.  I should - because I checked for trivial
    // rejection first.  These asserts just make sure
    // my clipping is working...
    assert(pixelsPerRow > 0);
    assert(numRows > 0);

    // Now blt the sucker!  But first, see if we can do it in one
    // big blt, without doing each row individually...
    if (getPitch() == pixelsPerRow && dest.getPitch() == pixelsPerRow) {
        memcpy(dPtr, sPtr, pixelsPerRow * numRows * sizeof(PIX));
    } else {
        do {
            memcpy(dPtr, sPtr, pixelsPerRow * sizeof(PIX));
            sPtr += getPitch();
            dPtr += dest.getPitch();
        } while (--numRows > 0);
    }
} // end Surface::blt

// bltTrans
//---------------------------------------------------------------------------
// Purpose: Puts the surface onto the destination using the slowest form of
//          transparency detection (pixel by pixel basis), while performing
//          clipping on the bounds of the object.
//---------------------------------------------------------------------------
void Surface::bltTrans(Surface &dest, int x, int y) const
{
    assert(getDoesExist());
    assert(dest.getDoesExist());
    assert(this != 0);
    assert(mem != 0);
    assert(dest.mem != 0);

    // Add in the offset factor.
    x+=offset.x;
    y+=offset.y;

    // Trivial clipping rejection - no overlap.
    // Also will jump out immediately if either image has zero size.
    if (x >= (int)dest.getWidth() || y >= (int)dest.getHeight())
        return;

    int end_x = x + getWidth();
    int end_y = y + getHeight();
    if ( end_x <= 0 || end_y <= 0 ) return;

    unsigned int pixelsPerRow = getWidth();
    unsigned int numRows      = getHeight();

    PIX	*sPtr	= mem;      // Pointer to source Surface start of memory.
    PIX	*dPtr	= dest.mem; // Pointer to destination Surface start of memory.

    // Check for partial clip, calculate number of pixels
    // per row to copy, and number of rows to copy.  Adjust
    // sPtr and dPtr.

    // CLIP LEFT
    if (x < 0) {
        pixelsPerRow +=  x; // This will subtract the neg. x value.
        sPtr         += -x; // This will move the sPtr to x = 0, from the neg. x.
    } else {
        dPtr += x;
    }

    // CLIP RIGHT
    // This subtracts only the portion hanging over the right edge of the
    // destination Surface
    if ((unsigned int)end_x > dest.getWidth())
        pixelsPerRow -= end_x - dest.getWidth();

    // CLIP TOP
    if (y < 0) {
        numRows += y;
        sPtr    -= y * (int)getPitch();
    } else {
        dPtr += y * (int)dest.getPitch();
    }

    // CLIP BOTTOM
    // This subtracts only the portion hanging over the bottom edge of the
    // destination Surface
    if ((unsigned int)end_y > dest.getHeight())
        numRows -= end_y - dest.getHeight();

    // Now, Check to make sure I actually have something
    // to draw.  I should - because I checked for trivial
    // rejection first.  These asserts just make sure
    // my clipping is working...
    assert(pixelsPerRow > 0);
    assert(numRows > 0);

    int srcAdjustment  = getPitch()      - pixelsPerRow;
    int destAdjustment = dest.getPitch() - pixelsPerRow;
    for (unsigned int row = 0; row < numRows; row++) {
        for (unsigned int col = 0; col < pixelsPerRow; col++) {
            if (*sPtr != 0)
                *dPtr = *sPtr;
            sPtr++;
            dPtr++;
        }

        sPtr += srcAdjustment;
        dPtr += destAdjustment;
    }

} // end Surface::bltTrans

// bltTransC
//---------------------------------------------------------------------------
// Purpose: Puts the surface onto the destination using the slowest form of
//          transparency detection (pixel by pixel basis), while performing
//          clipping on the bounds of the object. The non-transparent pixels
//          are blitted in the specified color.
//---------------------------------------------------------------------------
void Surface::bltTransColor(Surface &dest, int x, int y, const Uint8 color) const
{
    assert(getDoesExist());
    assert(dest.getDoesExist());
    assert(this != 0);
    assert(mem != 0);
    assert(dest.mem != 0);

    // Add in the offset factor.
    x+=offset.x;
    y+=offset.y;

    // Trivial clipping rejection - no overlap.
    // Also will jump out immediately if either image has zero size.
    if (x >= (int)dest.getWidth() || y >= (int)dest.getHeight())
        return;

    int end_x = x + getWidth();
    int end_y = y + getHeight();
    if ( end_x <= 0 || end_y <= 0 ) return;

    unsigned int pixelsPerRow = getWidth();
    unsigned int numRows      = getHeight();

    PIX	*sPtr	= mem;      // Pointer to source Surface start of memory.
    PIX	*dPtr	= dest.mem; // Pointer to destination Surface start of memory.

    // Check for partial clip, calculate number of pixels
    // per row to copy, and number of rows to copy.  Adjust
    // sPtr and dPtr.

    // CLIP LEFT
    if (x < 0) {
        pixelsPerRow +=  x; // This will subtract the neg. x value.
        sPtr         += -x; // This will move the sPtr to x = 0, from the neg. x.
    } else {
        dPtr += x;
    }

    // CLIP RIGHT
    // This subtracts only the portion hanging over the right edge of the
    // destination Surface
    if ((unsigned int)end_x > dest.getWidth())
        pixelsPerRow -= end_x - dest.getWidth();

    // CLIP TOP
    if (y < 0) {
        numRows += y;
        sPtr    -= y * (int)getPitch();
    } else {
        dPtr += y * (int)dest.getPitch();
    }

    // CLIP BOTTOM
    // This subtracts only the portion hanging over the bottom edge of the
    // destination Surface
    if ((unsigned int)end_y > dest.getHeight())
        numRows -= end_y - dest.getHeight();

    // Now, Check to make sure I actually have something
    // to draw.  I should - because I checked for trivial
    // rejection first.  These asserts just make sure
    // my clipping is working...
    assert(pixelsPerRow > 0);
    assert(numRows > 0);

    int srcAdjustment  = getPitch()      - pixelsPerRow;
    int destAdjustment = dest.getPitch() - pixelsPerRow;

    for (unsigned int row = 0; row < numRows; row++) {
        for (unsigned int col = 0; col < pixelsPerRow; col++) {
            if (*sPtr != 0)
                *dPtr = color;
            sPtr++;
            dPtr++;
        }

        sPtr += srcAdjustment;
        dPtr += destAdjustment;
    }
} // end Surface::bltTransC

void Surface::bltTransColorFromSDLSurface(SDL_Surface *source, int x, int y, const Uint8 color) const
{
    if (mem == 0) {
        return; // TODO??? how?
    }
    assert(getDoesExist());
    assert(source != 0);
    assert(mem != 0);

    // Trivial clipping rejection - no overlap.
    // Also will jump out immediately if either image has zero size.
    if (x >= (int)getWidth() || y >= (int)getHeight())
        return;

    int end_x = x + source->w;
    int end_y = y + source->h;
    if ( end_x <= 0 || end_y <= 0 ) return;

    unsigned int pixelsPerRow = source->w;
    unsigned int numRows      = source->h;

    PIX	*sPtr	= (PIX*) source->pixels; // Pointer to source Surface start of memory.
    PIX	*dPtr	= mem; // Pointer to destination Surface start of memory.

    // Check for partial clip, calculate number of pixels
    // per row to copy, and number of rows to copy.  Adjust
    // sPtr and dPtr.

    // CLIP LEFT
    if (x < 0) {
        pixelsPerRow +=  x; // This will subtract the neg. x value.
        sPtr         += -x; // This will move the sPtr to x = 0, from the neg. x.
    } else {
        dPtr += x;
    }

    // CLIP RIGHT
    // This subtracts only the portion hanging over the right edge of the
    // destination Surface
    if ((unsigned int)end_x > getWidth())
        pixelsPerRow -= end_x - getWidth();

    // CLIP TOP
    if (y < 0) {
        numRows += y;
        sPtr    -= y * source->pitch;
    } else {
        dPtr += y * (int)getPitch();
    }

    // CLIP BOTTOM
    // This subtracts only the portion hanging over the bottom edge of the
    // destination Surface
    if ((unsigned int)end_y > getHeight())
        numRows -= end_y - getHeight();

    // Now, Check to make sure I actually have something
    // to draw.  I should - because I checked for trivial
    // rejection first.  These asserts just make sure
    // my clipping is working...
    assert(pixelsPerRow > 0);
    assert(numRows > 0);

    int srcAdjustment  = source->pitch - pixelsPerRow;
    int destAdjustment = getPitch() - pixelsPerRow;

    for (unsigned int row = 0; row < numRows; row++) {
        for (unsigned int col = 0; col < pixelsPerRow; col++) {
            if (*sPtr != 0)
                *dPtr = color;
            sPtr++;
            dPtr++;
        }

        sPtr += srcAdjustment;
        dPtr += destAdjustment;
    }
} // end Surface::bltTransColorFromSDLSurface

// drawHLine
//---------------------------------------------------------------------------
// Purpose: Draws a horizontal drawLine.
//---------------------------------------------------------------------------
void Surface::drawHLine(int x1, int y, int x2, const PIX &color)
{
    assert(getDoesExist());
    assert(this != 0);

    // Check for trivial rejection
    if ( y < 0 || x2 <= 0
         || y >= (int)getHeight()
         || x1 >= (int)getWidth() )
         return;

    assert(mem != 0);
    if (mem == 0) return;

    orderCoords(x1, x2);

    unsigned length = x2 - x1;
    PIX *ptr = mem + y * (int)getPitch();

    // CLIP LEFT
    if (x1 < 0) {
        length += x1;
    }	else {
        ptr += x1;
    }

    // CLIP RIGHT
    if (x2 >= (int)getWidth()) length -= (x2 - getWidth());

    memset(ptr, color, length * sizeof(PIX));

} // end Surface::drawHLine

// drawVLine
//---------------------------------------------------------------------------
// Purpose: Draws a vertical drawLine.
//---------------------------------------------------------------------------
void Surface::drawVLine(int x, int y1, int y2, const PIX &color)
{
    assert(getDoesExist());
    assert(this != 0);

    // Check for trivial rejection
    if ( x < 0 || y2 <= 0
        || x >= (int)getWidth()
        || y1 >= (int)getHeight() )
        return;

    assert(mem != 0);
    if (mem == 0) return;

    orderCoords(y1, y2);

    // CLIP TOP
    if (y1 < 0) y1 = 0;

    // CLIP BOTTOM
    if (y2 >= (int)getHeight()) y2 = getHeight()-1;

    PIX	*ptr	= mem+y1*(int)getPitch()+x;

    int	width	= y2 - y1;

    while(width > 0) {
        *ptr	= color;
        ptr	+= getPitch();
        width--;
    }
} // end Surface::drawVLine

// fill
//---------------------------------------------------------------------------
// Purpose: Fills the Surface will the specified color.
//---------------------------------------------------------------------------
void Surface::fill(const PIX &color)
{
    assert(getDoesExist());
    assert(this != 0);

    if ( !getWidth() || !getHeight() ) return;

    if (getWidth() == getPitch()) {
        memset(mem, color, getWidth() * getHeight() * sizeof(PIX));
    }	else {
        int	n = getHeight();
        PIX *ptr = mem;
        do {
            memset(ptr, color, getWidth());
            ptr += getPitch();
        } while (--n > 0);
    }

} // end Surface::fill

// fillRect
//---------------------------------------------------------------------------
// Purpose: Fills the specified rectangle in the calling Surface with the
//          specified color.
//---------------------------------------------------------------------------
void Surface::fillRect(iRect bounds, const PIX &color)
{
    assert(getDoesExist());
    assert(this != 0);

    if ( !getWidth() || !getHeight() ) return;

    orderCoords(bounds);

    // Check for trivial rejection
    //if (bounds.max < 0 || bounds.min >= pix) return;
    if (bounds.max.x <  0)     return;
    if (bounds.max.y <  0)     return;
    if (bounds.min.x >= (int)getWidth()) return;
    if (bounds.min.y >= (int)getHeight()) return;

    // Check for clipping
    if (bounds.min.x <  0)     bounds.min.x = 0;
    if (bounds.min.y <  0)     bounds.min.y = 0;
    if (bounds.max.x > (int)getWidth())  bounds.max.x = getWidth();
    if (bounds.max.y > (int)getHeight()) bounds.max.y = getHeight();

    iXY diff;
    diff = (bounds.max - bounds.min);

    // Set memory to the top-left pixel of the rectangle.
    PIX	*ptr = mem + bounds.min.y * (int)getPitch() + bounds.min.x;

    for (int y = 0; y < diff.y; y++) {
        // Lay the horizontal strip.
        memset(ptr, color, diff.x * sizeof(PIX));
        ptr += getPitch();
    }
} // end Surface::fillRect

// drawRect
//---------------------------------------------------------------------------
// Purpose: Draws a rectagle in the specified color on the calling Surface.
//---------------------------------------------------------------------------
void Surface::drawRect(iRect bounds, const PIX &color)
{
    assert(getDoesExist());
    assert(this != 0);

    if ( !getWidth() || !getHeight() ) return;

    orderCoords(bounds);

    // Check for trivial rejection
    if      (bounds.max.x <  0)     return;
    else if (bounds.max.y <  0)     return;
    else if (bounds.min.x >= (int)getWidth()) return;
    else if (bounds.min.y >= (int)getHeight()) return;

    // Check for clipping
    if (bounds.min.x <  0)     bounds.min.x = 0;
    if (bounds.min.y <  0)     bounds.min.y = 0;
    if (bounds.max.x >= (int)getWidth())  bounds.max.x = getWidth() - 1;
    if (bounds.max.y >= (int)getHeight()) bounds.max.y = getHeight() - 1;

    drawHLine(bounds.min.x, bounds.min.y, bounds.max.x,   color);
    drawHLine(bounds.min.x, bounds.max.y, bounds.max.x+1, color);
    drawVLine(bounds.min.x, bounds.min.y, bounds.max.y,   color);
    drawVLine(bounds.max.x, bounds.min.y, bounds.max.y,   color);
} // end Surface::drawRect

// drawLine
//---------------------------------------------------------------------------
// Purpose: Draws a drawLine with any slope.
//---------------------------------------------------------------------------
void Surface::drawLine(int x1, int y1, int x2, int y2, const PIX &color)
{
    assert(getDoesExist());
    assert(this != 0);

    // Check for horizontal lines
    if (y1 == y2) {
        orderCoords(x1, x2);
        drawHLine(x1, y1, x2 + 1, color);
        return;
    }

    // Check for vertical lines
    if (x1 == x2) {
        orderCoords(y1, y2);
        drawVLine(x1, y1, y2, color);
        return;
    }

    int	y_unit		= 1;
    int	x_unit		= 1;
    int	ydiff		= y2 - y1;
    int	xdiff		= x2 - x1;
    int	errorTerm	= 0;

    // These values are where a pixel is located in the drawLine
    int	xOffset	= x1;
    int	yOffset	= y1;

    // Finds out whether the drawLine moves up or down vertically
    if (ydiff < 0) {
        ydiff  = -ydiff;
        y_unit = -1;   // Move up the screen
    }

    // Finds out whether the drawLine moves left or right horizontally
    if (xdiff < 0) {
        xdiff  = -xdiff;
        x_unit = -1;
    }

    if (xdiff > ydiff) {
        int length = xdiff + 1;
        for (int i = 0; i < length;i++) {
            putPixel(xOffset, yOffset, color);  // Draw the pixel
            xOffset+=x_unit;
            errorTerm += ydiff;
            if (errorTerm > xdiff) {
                errorTerm -= xdiff;
                yOffset+=y_unit;
            }
        }
    }	else {
        int	length	= ydiff + 1;

        for (int i = 0; i < length; i++) {
        putPixel(xOffset, yOffset, color);  // Draw the pixel
            yOffset+=y_unit;
            errorTerm += xdiff;
            if (errorTerm > 0) {
                errorTerm -= ydiff;
                xOffset+=x_unit;
            }
        }
    }
} //end Surface::drawLine

// flipVertical
//---------------------------------------------------------------------------
// Purpose: Goes through all the frames of the surface and flips then
//          vertically.
//---------------------------------------------------------------------------
void Surface::flipVertical()
{
    assert(getDoesExist());
    assert(this != 0);

    Surface tempSurface(getWidth(), getHeight(), 1);

    for (unsigned int frameNum = 0; frameNum < getNumFrames(); frameNum++) {
        // This sets the mem pointer of the source Surface
        setFrame(frameNum);

        PIX *sPtr = mem + getWidth() * getHeight() - getWidth();
        PIX *dPtr = tempSurface.mem;

        for (unsigned int y = 0; y < getHeight(); y++) {
            memcpy(dPtr, sPtr, getWidth() * sizeof(PIX));
            sPtr -= getWidth();
            dPtr += getWidth();
        }
        tempSurface.blt(*this,0 ,0);
    }
} // end Surface::flipVertical

// copy
//---------------------------------------------------------------------------
// Purpose: Copies the specified number of frames from the source Surface to
//          the calling Surface.
//---------------------------------------------------------------------------
void Surface::copy(const Surface &source)
{
    if(!source.getDoesExist())
        return;

    assert(this != 0);

    // XXX ugly ugly ugly
    Surface& nonconstsource = const_cast<Surface&> (source);

    // Create a Surface the surface the same size as the source.
    create(source.getWidth(), source.getHeight(), source.getNumFrames());

    PIX* oldmem = source.mem;
    for (unsigned int frameNum = 0; frameNum < source.getNumFrames(); frameNum++) {
        // Set the source Surface frame.
        setFrame(frameNum);
        nonconstsource.setFrame(frameNum);

        // Blit the source frame to the calling frame
        source.blt(*this, 0, 0);
    }
    nonconstsource.mem = oldmem;
} // end Surface::copy

// rotate
//---------------------------------------------------------------------------
// Purpose: Rotates a surface.  Accepts 0..360 integers for the degrees.
//---------------------------------------------------------------------------
void Surface::rotate(int angle)
{
    assert(this != 0);
    int center_x = getWidth()>>1;
    int center_y = getHeight()>>1;

    for (unsigned int i = 0; i < getNumFrames(); i++) {
        setFrame(i);

        if (angle % 360 != 0) {
            Surface tempSurface(getWidth(), getHeight(), 1);

            float angleRadians = -float(angle) / float(180.0 / M_PI);
            float cosAngle     = cos(angleRadians);
            float sinAngle     = sin(angleRadians);

            int index   = 0;

            for (int y = -center_y; y < center_y; y++) {
                for (int x = -center_x; x < center_x; x++) {
                    int xSource = int((x * cosAngle - y * sinAngle) + center_x);
                    int ySource = int((y * cosAngle + x * sinAngle) + center_y);

                    if ((xSource >= 0) && ((unsigned int)xSource < getWidth()) && (ySource >= 0) && ((unsigned int)ySource < getHeight())) {
                        tempSurface.putPixel(index % getWidth(), index / getHeight(), getPixel(xSource, ySource));
                    } else {
                        // Set the pixel transparent
                        tempSurface.putPixel(index % getWidth(), index / getHeight(), 0);
                    }
                    index++;
                }
            }

            tempSurface.blt(*this, 0, 0);
        }
    }
} // end ROTATE

// scale
//---------------------------------------------------------------------------
// Purpose: Scales all the frames of the surface from the current size to the
//          requested size.
//---------------------------------------------------------------------------
void Surface::scale(unsigned int x, unsigned int y)
{
    if ( !x || !y || (x==getWidth() && y==getHeight()))
        return;

    // Create a temporary surface to scale the image onto.
    Surface tempSurface(x, y, getNumFrames());

    iRect r(0, 0, x, y);

    // Go through all the frames of the surface.
    unsigned int frame;
    for (frame = 0; frame < tempSurface.getNumFrames(); frame++) {
        tempSurface.setFrame(frame);
        Surface::setFrame(frame);

        tempSurface.bltScale(*this, r);
    }

    // Resize the calling surface, then copy all the scaled images on it.
    Surface::resize(x, y);

    for (frame = 0; frame < tempSurface.getNumFrames(); frame++) {
        Surface::setFrame(frame);
        tempSurface.setFrame(frame);

        tempSurface.blt(*this, 0, 0);
    }

    /* OLD VERSION 6.29.1998
    	// Find out the number of pix to step in the image.
    	fXY stepPix;
    	stepPix.x = float(Surface::pix.x) / float(pix.x);
    	stepPix.y = float(Surface::pix.y) / float(pix.y);

    	// Create a temporary surface to scale the image onto.
    	Surface tempSurface(pix, pix.x, frameCount);

    	// build a table the first time you go through, then scaling all
    	// additional frames can be done by getting the values from the table.

    	fXY curPix;
    	curPix = 0.0;

    	// Go through all the frames of the surface.
    	for (int frame = 0; frame < frameCount; frame++)
    	{
    		//LOG(("frame: %d; frameCount: %d", frame, frameCount));
    		tempSurface.setFrame(frame);
    		this->setFrame(frame);

    		for (int x = 0; x < pix.x; x++)
    		{
    			for (int y = 0; y < pix.y; y++)
    			{
    				tempSurface.putPixel(x, y, this->getPixel(int(curPix.x), int(curPix.y)));
    				curPix.y += stepPix.y;
    			}
    			curPix.y = 0.0;
    			curPix.x += stepPix.x;
    		}
    		curPix.x = 0.0;
    	}

    	// Resize the calling surface, then map all the scaled images on it.
    	this->resize(pix);

    	assert(frameCount == tempSurface.frameCount);

    	for (frame = 0; frame < frameCount; frame++)
    	{
    		//LOG(("frame: %d; frameCount: %d", frame, frameCount));
    		this->setFrame(frame);
    		tempSurface.setFrame(frame);
    		tempSurface.blt(*this);
    	}
    */
} // end Surface::scale

void Surface::bltLookup(const iRect &destRect, const PIX table[])
{
    assert(getDoesExist());
    assert(this != 0);
    assert(mem != 0);

    iXY min = destRect.min + offset;
    if (min.x >= (int)getWidth()) return;
    if (min.y >= (int)getHeight()) return;

    iXY max = destRect.max + offset;
    if (max.x <= 0) return;
    if (max.y <= 0) return;

    // Clip destination rectangle
    if (min.x < 0) min.x = 0;
    if (min.y < 0) min.y = 0;
    if (max.x >= (int)getWidth())  max.x = getWidth();
    if (max.y >= (int)getHeight()) max.y = getHeight();

    size_t pixelsPerRow = max.x - min.x;
    size_t numRows      = max.y - min.y;

    PIX *dRow = mem + min.y*(int)getPitch() + min.x;

    for (size_t yCount = 0 ; yCount < numRows ; yCount++) {
        for(size_t x=0; x<pixelsPerRow; x++)
            dRow[x] = table[dRow[x]];

        dRow += getPitch();
    }
}

//---------------------------------------------------------------------------
void Surface::bltScale(const Surface &source, const iRect &destRect)
{
    assert(getDoesExist());
    assert(source.getDoesExist());
    assert(this != 0);

    iXY min = destRect.min + source.offset;
    iXY max = destRect.max + source.offset;

    if (min.x >= (int)getWidth()) return;
    if (min.y >= (int)getHeight()) return;

    // Something is overlapping, so we need to verify that both
    // surfaces are valid.
    assert(mem        != 0);
    assert(source.mem != 0);

    size_t pixelsPerRow = max.x-min.x;
    size_t numRows      = max.y-min.y;

    int srcX1 = 0;
    int srcY = 0;

    // FIXME - if inverted bitmap, then flip x1 and y to right or botom edges

    // Compute source deltas
    int xSrcDelta = int((float(source.getWidth()) / float(max.x - min.x)) * 65536.0);
    int ySrcDelta = int((float(source.getHeight()) / float(max.y - min.y)) * 65536.0);

    PIX  *dRow = mem;			// Pointer to the destination Surface

    // CLIP LEFT
    if (min.x < 0) {
        pixelsPerRow += min.x;
        srcX1 -= min.x*xSrcDelta;
    }	else {
        dRow += min.x;
    }

    // CLIP RIGHT
    if (max.x > (int)getWidth()) {
        pixelsPerRow -= max.x - getWidth();
    }

    // CLIP TOP
    if (min.y < 0) {
        numRows += min.y;
        srcY   -= min.y*ySrcDelta;
    }	else {
        dRow += (min.y*(int)getPitch());
    }

    // CLIP BOTTOM
    if (max.y > (int)getHeight()) {
        numRows -= max.y-getHeight();
    }

    // Check to make sure the clipping is working.
    if (pixelsPerRow <= 0) return;
    if (numRows <= 0) return;

#if 0
    int stepAndDecCount = (xSrcDelta << 16) | 0xffff;
    int stepWholePart = xSrcDelta >> 16;
    int srcX1FracWithCount = (srcX1 << 16) | pixelsPerRow;
#endif

    float xdelta = float(source.getWidth()) / float(max.x - min.x);
    for (size_t yCount = 0 ; yCount < numRows ; yCount++) {
        const PIX *sRow = source.pixPtr(0, srcY >> 16) + (srcX1 >> 16);

#if 0
        bltScaleSpan(dRow, sRow, srcX1FracWithCount, stepAndDecCount, stepWholePart);
#else
        // XXX: WARNING SLOW CODE
        float sPos = 0;
        for(size_t x=0; x<pixelsPerRow; x++) {
            dRow[x] = sRow[(size_t) sPos];
            sPos += xdelta;
        }
#endif

        srcY += ySrcDelta;
        dRow += getPitch();
    }
}

// shrinkWrap
//---------------------------------------------------------------------------
void Surface::shrinkWrap()
{
    assert(getDoesExist());
    assert(this != 0);

    int center_x = getWidth()>>1;
    int center_y = getHeight()>>1;

    // Start the bounds values in the center of the surface.
    iRect bounds;
    bounds.min.x = center_x;
    bounds.max.x = center_x;
    bounds.min.y = center_y;
    bounds.max.y = center_y;

    unsigned int num;
    for (num = 0; num < getNumFrames(); num++) {
        setFrame(num);
        //LOG(("curFrame:  %d", curFrame));
        //LOG(("frameCount: %d", frameCount));
        //LOG(("pix.x:     %d", pix.x));
        //LOG(("pix.y:     %d", pix.y));

        // Check the x bounds.
        for (unsigned int y = 0; y < getHeight(); y++) {
            for (unsigned int x = 0; x < getWidth(); x++) {
                if (getPixel(x, y) != 0) {
                    if (x < (unsigned int)bounds.min.x) {
                        bounds.min.x = x;
                    }
                    if (x > (unsigned int)bounds.max.x) {
                        bounds.max.x = x;
                    }
                }
            }
        }

        // Check the y bounds.
        for (unsigned int x = 0; x < getWidth(); x++) {
            for (unsigned int y = 0; y < getHeight(); y++) {
                if (getPixel(x, y) != 0) {
                    if (y < (unsigned int)bounds.min.y) {
                        bounds.min.y = y;
                    }
                    if (y > (unsigned int)bounds.max.y) {
                        bounds.max.y = y;
                    }
                }
            }
        }
    }

    int xDiff = bounds.max.x-bounds.min.x;
    int yDiff = bounds.max.y-bounds.min.y;

    //LOG(("bounds.min.x: %d", bounds.min.x));
    //LOG(("bounds.max.x: %d", bounds.max.x));
    //LOG(("bounds.min.y: %d", bounds.min.y));
    //LOG(("bounds.max.y: %d", bounds.max.y));
    //LOG(("xDiff:        %d", xDiff));
    //LOG(("yDiff:        %d", yDiff));

    // Create a temporary surface to draw all the cropped frames onto.
    Surface tempSurface(xDiff, yDiff, getNumFrames());
    //tempSurface.create(xDiff, yDiff, xDiff, frameCount);

    // Crop the surface frames onto the temp surface.
    for (num = 0; num < getNumFrames(); num++) {
        setFrame(num);
        tempSurface.setFrame(num);
        tempSurface.fill(0);
        blt(tempSurface, -bounds.min.x, -bounds.min.y);
    }

    resize(xDiff, yDiff);

    copy(tempSurface);

} // end Surface::shrinkWrap

// getAverageColor
//---------------------------------------------------------------------------
// Purpose: Recalculates the best single color to represent this Surface.
//---------------------------------------------------------------------------
PIX Surface::getAverageColor()
{
    int avgR = 0;
    int avgG = 0;
    int avgB = 0;

    // Go through a single cTile and get all the additive color values.
    for (unsigned int x = 0; x < getWidth(); x++) {
        for (unsigned int y = 0; y < getHeight(); y++) {
            avgR += Palette::color[getPixel(x, y)].r;
            avgG += Palette::color[getPixel(x, y)].g;
            avgB += Palette::color[getPixel(x, y)].b;
        }
    }

    // Divide each individual amount by the number of bytes in the image.
    int numPix = getArea();

    avgR /= numPix;
    avgG /= numPix;
    avgB /= numPix;

    return Palette::findNearestColor(avgR, avgG, avgB);
} // end Surface::getAverageColor

// initFont
//---------------------------------------------------------------------------
// Purpose: Load the TTF font.
//---------------------------------------------------------------------------
void initFont()
{
    if (TTF_Init() < 0) {
        printf("Couldn't initialize SDL TTF: %s\n", SDL_GetError());
        exit(1);
    }
    // Quantico-Regular looked good too but some issues with some characters.
    font = TTF_OpenFont("fonts/GNUUnifont9FullHintInstrUCSUR.ttf", FONT_SIZE);
    TTF_SetFontStyle(font, TTF_STYLE_BOLD);
    TTF_SetFontHinting(font, TTF_HINTING_NORMAL);
} // Surface::initFont

unsigned int
Surface::getFontHeight()
{
    // TODO pass in string? some characters are taller than others
    // TODO cache
    SDL_Surface* font_surface = TTF_RenderUTF8_Solid(font, ".", Palette::color[0]);
    unsigned int height = font_surface->h + 3; // magic number works with GNUUnifont9FullHintInstrUCSUR font
    SDL_FreeSurface(font_surface);
    return height;
}

unsigned int Surface::getTextLength(const char* text)
{
    // TODO cache
    SDL_Surface* font_surface = TTF_RenderUTF8_Solid(font, text, Palette::color[0]);
    unsigned int width = font_surface->w;
    SDL_FreeSurface(font_surface);
    return width;
}

// renderText
//---------------------------------------------------------------------------
// Purpose: Renders a string of text to the surface using color and background
//          color. The surface is created, if it was already created and is
//          the exact size needed, clears it and use it, if size is different
//          will delete it and create new one
// Parameters:
//   str     = string to render
//   color   = foreground color
//   bgcolor = background color
//---------------------------------------------------------------------------
void
Surface::renderText(const char *str, PIX color, PIX bgcolor)
{
    if ( !str )
        return;

    int len = strlen(str);
    if ( !len )
        return;

    printf("rendering text %s\n", str);
    SDL_Surface* font_surface = TTF_RenderUTF8_Solid(font, str, Palette::color[color]);
    if (!font_surface) {
        printf("Could not renderText() %s %s\n", str, SDL_GetError());
        return;
    }
    unsigned int need_width = font_surface->w;
    unsigned int need_height = font_surface->h;

    if ( frame0 != 0 ) {
        if ( getWidth() != need_width || getHeight() != need_height ) {
            free();
            create( need_width, need_height, 1);
        }
    } else {
        create( need_width, need_height, 1);
    }

    drawRect(iRect(0, 0, getWidth(), getHeight()), bgcolor);
    bltTransColorFromSDLSurface(font_surface, 0, 0, color);

    SDL_FreeSurface(font_surface); // todo optimize
}

// bltString
//---------------------------------------------------------------------------
// Purpose: Blits the specified string of text to the screen by making
//          calls to blitChar for each character of the string. Does not
//          handle wrapping.
//---------------------------------------------------------------------------
void Surface::bltString(int x, int y, const char * str, const Uint8 &color)
{
    int len = strlen(str);
    if ( !len ) {
        return;
    }
    SDL_Surface* font_surface = TTF_RenderUTF8_Solid(font, str, Palette::color[color]);
    if (!font_surface) {
        printf("Could not bltString() %s %s\n", str, SDL_GetError());
        return;
    }
    bltTransColorFromSDLSurface(font_surface, x, y, color);
    SDL_FreeSurface(font_surface);
} // end Surface::bltString

// bltStringShadowed
//---------------------------------------------------------------------------
void Surface::bltStringShadowed(int x, int y, char const *str, const Uint8 &textColor, const Uint8 &shadowColor)
{
    int len = strlen(str);
    if ( !len )
        return;
    SDL_Surface* font_surface_back = TTF_RenderUTF8_Solid(font, str, Palette::color[shadowColor]);
    if (!font_surface_back) {
        printf("Could not bltStringShadowed() %s %s\n", str, SDL_GetError());
        return;
    }
    bltTransColorFromSDLSurface(font_surface_back, x + 1, y + 1, shadowColor);
    SDL_FreeSurface(font_surface_back);
    SDL_Surface* font_surface_front = TTF_RenderUTF8_Solid(font, str, Palette::color[textColor]);
    if (!font_surface_front) {
        printf("Could not bltStringShadowed() %s %s\n", str, SDL_GetError());
        return;
    }
    bltTransColorFromSDLSurface(font_surface_front, x, y, textColor);
    SDL_FreeSurface(font_surface_front);
} // end Surface::bltStringShadowed

// bltStringCenter
//---------------------------------------------------------------------------
// Purpose: Blits a string of text and centers it horizontally and vertically
//          on the screen. Does not handle wrapping.
//---------------------------------------------------------------------------
void Surface::bltStringCenter(const char *str, PIX color)
{
    int len = strlen(str);
    if ( !len )
        return;
    SDL_Surface* font_surface = TTF_RenderUTF8_Solid(font, str, Palette::color[color]);
    if (!font_surface) {
        return;
    }
    bltTransColorFromSDLSurface(font_surface, (getWidth() - std::min(font_surface->w, ((int) getWidth()))) / 2,
                                (getHeight() - std::min(font_surface->h, ((int) getHeight()))) / 2, color);
    SDL_FreeSurface(font_surface);
} // end Surface::bltStringCenter

void Surface::bltStringCenterMin30(const char *str, PIX color)
{
    int len = strlen(str);
    if ( !len )
        return;
    SDL_Surface* font_surface = TTF_RenderUTF8_Solid(font, str, Palette::color[color]);
    if (!font_surface) {
        printf("Could not bltStringCenterMin30() %s %s\n", str, SDL_GetError());
        return;
    }
    bltTransColorFromSDLSurface(font_surface, (getWidth() - (font_surface->w)) / 2,
                                (getHeight() - font_surface->h) / 2 - 30, color);
    SDL_FreeSurface(font_surface);
} // end Surface::bltStringCenter

// bltStringShadowedCenter
//---------------------------------------------------------------------------
// Purpose: Blits a string of text and centers it horizontally and vertically
//          on the screen. Does not handle wrapping.
//---------------------------------------------------------------------------
void Surface::bltStringShadowedCenter(const char *str, PIX textColor, PIX shadowColor)
{
    int len = strlen(str);
    if ( !len )
        return;
    // TODO OPTIMIZE
    SDL_Surface* font_surface_back = TTF_RenderUTF8_Solid(font, str, Palette::color[shadowColor]);
    if (!font_surface_back) {
        printf("Could not bltStringShadowedCenter() %s %s\n", str, SDL_GetError());
        return;
    }
    int x = (getWidth() - font_surface_back->w) / 2;
    int y = (getHeight() - font_surface_back->h) / 2;
    bltTransColorFromSDLSurface(font_surface_back, x + 1, y + 1, shadowColor);
    SDL_FreeSurface(font_surface_back);
    SDL_Surface* font_surface_front = TTF_RenderUTF8_Solid(font, str, Palette::color[textColor]);
    if (!font_surface_front) {
        printf("Could not bltStringShadowedCenter() %s %s\n", str, SDL_GetError());
        return;
    }
    bltTransColorFromSDLSurface(font_surface_front, x, y, textColor);
    SDL_FreeSurface(font_surface_front);

} // end Surface::bltStringShadowedCenter

// bltStringCenteredInRect
//---------------------------------------------------------------------------
// Purpose: Blits the string centered inside the specified rectangle.
//---------------------------------------------------------------------------
void Surface::bltStringCenteredInRect(const iRect &rect, const char *str, const PIX &color)
{
    int len = strlen(str);
    if ( !len )
        return;
    SDL_Surface* font_surface = TTF_RenderUTF8_Solid(font, str, Palette::color[color]);
    if (!font_surface) {
        printf("Could not bltStringCenteredInRect() %s %s\n", str, SDL_GetError());
        return;
    }
    int x = rect.min.x + (rect.getSizeX() - font_surface->w) / 2;
    int y = rect.min.y + (rect.getSizeY() - font_surface->h) / 2;
    bltTransColorFromSDLSurface(font_surface, x,y, color);
    SDL_FreeSurface(font_surface);
} // end Surface::bltStringCenteredInRect

// create
//---------------------------------------------------------------------------
void
Surface::create(unsigned int w, unsigned int h, unsigned int nframes)
{
    //reset();
    alloc( w, h, nframes);
} // end Surface::create

// nextFrame
//
// Move to the next frame based off time and the fps.  Returns a 0 if it
// was the last frame and 1 otherwise.
//---------------------------------------------------------------------------
int Surface::nextFrame()
{
    curFrame += TimerInterface::getTimeSlice() * fps;

    if (curFrame >= getNumFrames()) {
        curFrame = 0.0;
        return 0;
    }

    setFrame(curFrame);
    return 1;
}

void Surface::loadBMP(const char *fileName, bool needAlloc)
{
    assert(this != 0);

    if (needAlloc) free();

    std::unique_ptr<filesystem::ReadFile> file(
            filesystem::openRead(fileName));

    try {
        BitmapFileHeader file_header(file.get());

        if ( file_header.bfType != 0x4d42 ) // file_header.bfType != "BM"
            throw Exception("%s is not a valid 8-bit BMP file", fileName);

        BitmapInfoHeader info_header(file.get());

        if ( info_header.biBitCount != 8 )
            throw Exception("%s is not a 8-bit BMP file", fileName);

        if ( info_header.biCompression != BI_RGB )
            throw Exception("%s is not a 8-bit UnCompressed BMP file", fileName);

        if (needAlloc) {
            alloc(info_header.biWidth, info_header.biHeight, 1);

        } else {
            // Check and make sure the picture will fit
            if (getWidth() < (unsigned long) info_header.biWidth|| getHeight() < (unsigned long) info_header.biHeight )
                throw Exception("Not enough memory to load BMP image %s", fileName);
        }

        file->seek(file_header.bfOffBits);

        if ( (info_header.biWidth % 4) == 0 ) {
            file->read(mem, getWidth() * getHeight(), 1);
        } else {
            int padding = ((info_header.biWidth / 4 + 1) * 4) - info_header.biWidth;

            PIX buffer[10];
            int numRows = getHeight();

            //PIX *sPtr = mem;

            for (int row = 0; row < numRows; row++) {
                file->read(mem, getWidth(), 1);
                file->read(buffer, padding, 1);
                mem += getPitch();
            }
        }

        flipVertical();
    } catch(std::exception& e) {
        throw Exception("Error reading .bmp file '%s': %s",
                fileName, e.what());
    }
}

// drawButtonBorder
//---------------------------------------------------------------------------
void Surface::drawButtonBorder(iRect bounds, PIX topLeftColor, PIX bottomRightColor)
{
    assert(getDoesExist());
    assert(this != 0);

    orderCoords(bounds);
//    drawHLine(bounds.min.x,bounds.min.y,bounds.max.x-1,topLeftColor);
//    drawVLine(bounds.min.x,bounds.min.y,bounds.max.y-1,topLeftColor);
//    drawHLine(bounds.min.x,bounds.max.y-1,bounds.max.x-1,bottomRightColor);
//    drawVLine(bounds.max.x-1,bounds.min.y,bounds.max.y-1,bottomRightColor);
    RoundRect(bounds,3, topLeftColor);

} // end Surface::drawButtonBorder

// drawWindowsBorder
//--------------------------------------------------------------------------
void Surface::drawWindowsBorder()
{
    //drawRect(iRect(0,0,getWidth(),getHeight()), Color::darkGray);
    RoundRect(iRect(0,0,getWidth(),getHeight()),8, Color::lightGray);
} // end Surface::drawWindowsBorder

// bltStringInBox
//--------------------------------------------------------------------------
void Surface::bltStringInBox(const iRect &rect, const char *string, PIX color, int gapSpace, bool drawBox)
{
    if (drawBox) {
        drawRect(rect, Color::yellow);
    }

    iXY pos(rect.min);

    int  totalLength = strlen(string);
    int  length      = 0;
    int  done        = 0;

    while (length < totalLength - 1) {
        while (string[length] == '\n') {
            pos.x = rect.min.x;
            pos.y += gapSpace;
            length++;
        }

        // Remove any spaces.
        while (string[length] == ' ') {
            pos.x += FONT_WIDTH; // TODO REMOVE
            length++;
        }

        char strBuf[256];
        memset(strBuf, 0, sizeof(strBuf));
        int strBufLength = 0;

        while (!isspace(string[length + strBufLength]) && string[length + strBufLength] != '\n') {
            if (string[length + strBufLength] == '\0') {
                done = true;
                break;
            }

            assert(strBufLength < 256);
            strBuf[strBufLength] = string[length + strBufLength];
            strBufLength++;
        }

        strBuf[strBufLength] = '\0';

        if ((int) (pos.x + strlen(strBuf) * FONT_WIDTH) > rect.max.x) {
            pos.x = rect.min.x;
            pos.y += gapSpace;
        }

        bltString(pos.x, pos.y, strBuf, color);

        if (done) {
            return;
        }

        pos.x += strlen(strBuf) * FONT_WIDTH;

        length += strBufLength;
    }

} // end Surface::bltStringInBox

// drawBoxCorners
//--------------------------------------------------------------------------
// Purpose: Draws lines in the corners of the surface of the specified length
//          and color.
//--------------------------------------------------------------------------
void Surface::drawBoxCorners(const iRect &rect, int cornerLength, PIX color)
{
    // Make sure the corner lines are not longer than the rect.
    if (rect.getSizeX() < cornerLength) {
        cornerLength -= cornerLength - rect.getSizeX();
    }

    if (rect.getSizeY() < cornerLength) {
        cornerLength -= cornerLength - rect.getSizeY();
    }

    // Draw the rectangle edge-only selection box.
    // Top-left
    drawHLine(rect.min.x, rect.min.y, rect.min.x + cornerLength, color);
    drawVLine(rect.min.x, rect.min.y, rect.min.y + cornerLength, color);

    // Top-right
    drawHLine(rect.max.x - 1, rect.min.y, rect.max.x - cornerLength, color);
    drawVLine(rect.max.x - 1, rect.min.y, rect.min.y + cornerLength, color);

    // Bottom-right
    drawHLine(rect.max.x, rect.max.y - 1, rect.max.x - cornerLength, color);
    drawVLine(rect.max.x - 1, rect.max.y - 1, rect.max.y - cornerLength, color);

    // Bottom-left
    drawHLine(rect.min.x, rect.max.y - 1, rect.min.x + cornerLength, color);
    drawVLine(rect.min.x, rect.max.y - 1, rect.max.y - cornerLength, color);

} // end Surface::drawBoxCorners

void Surface::circle(int cx, int cy, int radius, PIX color)
{
  int d, y, x;

  d = 3 - (2 * radius);
  x = 0;
  y = radius;

  while (y >= x) {
    putPixel(cx + x, cy + y, color);// down right corner
    putPixel(cx + y, cy + x, color);// down right corner
    putPixel(cx - x, cy + y, color);// up left corner
    putPixel(cx - y, cy + x, color);// up left corner
    putPixel(cx + x, cy - y, color);// up right corner
    putPixel(cx + y, cy - x, color);// up right corner
    putPixel(cx - x, cy - y, color);// down left corner
    putPixel(cx - y, cy - x, color);// down left corner

    if (d < 0)
      d = d + (4 * x) + 6;
    else {
      d = d + 4 * (x - y) + 10;
      y--;
    }
    x++;
  }
}

void Surface::FillCircle(int cx, int cy, int radius, PIX color)
{
  int d, y, x;

  d = 3 - (2 * radius);
  x = 0;
  y = radius;

  while (y >= x) {
    drawHLine(cx - x, cy - y,cx+ (x + 1), color);
    drawHLine(cx - x, cy + y,cx+ (x + 1), color);
    drawHLine(cx - y, cy - x,cx+ (y + 1), color);
    drawHLine(cx - y, cy + x,cx+ (y + 1), color);

    if (d < 0)
      d = d + (4 * x) + 6;
    else {
      d = d + 4 * (x - y) + 10;
      y--;
    }
    x++;
  }
}

void Surface::BltCircle(int cx, int cy, int radius, const PIX table[])
{
  int d, y, x;

  d = 3 - (2 * radius);
  x = 0;
  y = radius;

  while (y >= x) {
    bltHLine(cx - x, cy - y,cx+ (x + 1), table);
    bltHLine(cx - x, cy + y,cx+ (x + 1), table);
    bltHLine(cx - y, cy - x,cx+ (y + 1), table);
    bltHLine(cx - y, cy + x,cx+ (y + 1), table);

    if (d < 0)
      d = d + (4 * x) + 6;
    else {
      d = d + 4 * (x - y) + 10;
      y--;
    }
    x++;
  }
}


void Surface::RoundRect(iRect rect, int radius, PIX color)
{
    int d, y, x;

    d = 3 - (2 * radius);
    x = 0;
    y = radius;

    if ( !getWidth() || !getHeight() ) return;

    orderCoords(rect);

    // Check for trivial rejection
    if      (rect.max.x <  0)     return;
    else if (rect.max.y <  0)     return;
    else if (rect.min.x >= (int)getWidth()) return;
    else if (rect.min.y >= (int)getHeight()) return;

    // Check for clipping
    if (rect.min.x <  0)     rect.min.x = 0;
    if (rect.min.y <  0)     rect.min.y = 0;
    if (rect.max.x >= (int)getWidth())  rect.max.x = getWidth() - 1;
    if (rect.max.y >= (int)getHeight()) rect.max.y = getHeight() - 1;

    drawHLine(rect.min.x+radius, rect.min.y, rect.max.x-radius,   color);
    drawHLine(rect.min.x+radius, rect.max.y, rect.max.x+1-radius, color);
    drawVLine(rect.min.x, rect.min.y+radius, rect.max.y-radius,   color);
    drawVLine(rect.max.x, rect.min.y+radius, rect.max.y-radius,   color);

  while (y >= x) {
    putPixel((rect.max.x-radius) + x, (rect.max.y-radius) + y, color);// down right corner
    putPixel((rect.max.x-radius) + y, (rect.max.y-radius) + x, color);// down right corner
    putPixel((rect.min.x+radius) - x, (rect.max.y-radius) + y, color);// down left corner
    putPixel((rect.min.x+radius) - y, (rect.max.y-radius) + x, color);// down left corner
    putPixel((rect.max.x-radius) + x, (rect.min.y+radius) - y, color);// up right corner
    putPixel((rect.max.x-radius) + y, (rect.min.y+radius) - x, color);// up right corner
    putPixel((rect.min.x+radius) - x, (rect.min.y+radius) - y, color);// up left corner
    putPixel((rect.min.x+radius) - y, (rect.min.y+radius) - x, color);// up left corner
//
    if (d < 0)
      d = d + (4 * x) + 6;
    else {
      d = d + 4 * (x - y) + 10;
      y--;
    }
    x++;
  }
}

void Surface::FillRoundRect(iRect rect, int radius, PIX color)
{
    int d, y, x;

    d = 3 - (2 * radius);
    x = 0;
    y = radius;

    if ( !getWidth() || !getHeight() ) return;

    orderCoords(rect);

    // Check for trivial rejection
    if      (rect.max.x <  0)     return;
    else if (rect.max.y <  0)     return;
    else if (rect.min.x >= (int)getWidth()) return;
    else if (rect.min.y >= (int)getHeight()) return;

    // Check for clipping
    if (rect.min.x <  0)     rect.min.x = 0;
    if (rect.min.y <  0)     rect.min.y = 0;
    if (rect.max.x >= (int)getWidth())  rect.max.x = getWidth() - 1;
    if (rect.max.y >= (int)getHeight()) rect.max.y = getHeight() - 1;

    fillRect(iRect(rect.min.x,rect.min.y+radius,
                   rect.max.x,rect.max.y-radius), color);

  while (y >= x) {
    drawHLine((rect.min.x+radius) - x, (rect.min.y+radius) - y,(rect.max.x-radius)+ x, color);//up
    drawHLine((rect.min.x+radius) - x, (rect.max.y-radius) + y,(rect.max.x-radius)+ x, color);//down
    drawHLine((rect.min.x+radius) - y, (rect.min.y+radius) - x,(rect.max.x-radius)+ y, color);//up
    drawHLine((rect.min.x+radius) - y, (rect.max.y-radius) + x,(rect.max.x-radius)+ y, color);//down

    if (d < 0)
      d = d + (4 * x) + 6;
    else {
      d = d + 4 * (x - y) + 10;
      y--;
    }
    x++;
  }
}

void Surface::BltRoundRect(iRect rect, int radius, const PIX table[])
{
    if ( !getWidth() || !getHeight() ) return;

    orderCoords(rect);

    // Check for trivial rejection
    if      (rect.max.x <  0)     return;
    else if (rect.max.y <  0)     return;
    else if (rect.min.x >= (int)getWidth()) return;
    else if (rect.min.y >= (int)getHeight()) return;

    // Check for clipping
    if (rect.min.x <  0)     rect.min.x = 0;
    if (rect.min.y <  0)     rect.min.y = 0;
    if (rect.max.x >= (int)getWidth())  rect.max.x = getWidth() - 1;
    if (rect.max.y >= (int)getHeight()) rect.max.y = getHeight() - 1;

    bltLookup(iRect(rect.min.x,rect.min.y+radius,
                   rect.max.x,rect.max.y-radius+1), table);

    int d = 3 - (2 * radius);
    int x = 0;
    int y = radius;

    while (x < y)
    {
        if (d < 0)
        {
            d = d + (4 * x) + 6;
        }
        else
        {
            bltHLine((rect.min.x+radius) - x, (rect.max.y-radius) + y,(rect.max.x-radius)+ x, table);//down
            bltHLine((rect.min.x+radius) - x, (rect.min.y+radius) - y,(rect.max.x-radius)+ x, table);//up
            d = d + 4 * (x - y) + 10;
            y--;
        }

        x++;
        bltHLine((rect.min.x+radius) - y, (rect.min.y+radius) - x,(rect.max.x-radius)+ y, table);//up
        bltHLine((rect.min.x+radius) - y, (rect.max.y-radius) + x,(rect.max.x-radius)+ y, table);//down
    }
}

void Surface::bltHLine(int x1, int y, int x2, const PIX table[])
{
    assert(getDoesExist());
    assert(this != 0);

    // Check for trivial rejection
    if ( y < 0 || x2 <= 0
         || y >= (int)getHeight()
         || x1 >= (int)getWidth() )
         return;

    assert(mem != 0);
    if (mem == 0) return;

    orderCoords(x1, x2);

    unsigned length = x2 - x1;
    PIX *ptr = mem + y * (int)getPitch();

    // CLIP LEFT
    if (x1 < 0) {
        length += x1;
    }	else {
        ptr += x1;
    }

    // CLIP RIGHT
    if (x2 >= (int)getWidth()) length -= (x2 - getWidth());

    for(size_t x=0; x<length; x++)
        ptr[x] = table[ptr[x]];

} // end Surface::drawHLine

