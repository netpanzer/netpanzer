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


#include <memory>

#include "Util/FileSystem.hpp"
#include "Util/Log.hpp"
#include "Util/Exception.hpp"
#include "ColorTable.hpp"
#include "Palette.hpp"
#include "Surface.hpp"
#include "Util/UtilInterface.hpp"

const char *ColorTable::extension = ".tbl";

int ColorTable::totalColorArrayCount = 0;
int ColorTable::totalByteCount       = 0;


// ColorTable
//---------------------------------------------------------------------------
ColorTable::ColorTable()
{
    colorCount = 0;
    colorArray = 0;

    totalColorArrayCount++;
    totalByteCount += sizeof(ColorTable);

} // end ColorTable::ColorTable

// ~ColorTable
//---------------------------------------------------------------------------
ColorTable::~ColorTable()
{
    free();

    totalColorArrayCount--;

    assert(totalColorArrayCount >= 0);

    totalByteCount -= sizeof(ColorTable);

} // end ColorTable::~ColorTable

// init
//---------------------------------------------------------------------------
void ColorTable::init(int colorCount)
{
    free();

    ColorTable::colorCount = colorCount;

    int numBytes = sizeof(PIX) * colorCount;

    colorArray = (PIX *) malloc(numBytes);
    if (colorArray == 0)
        throw Exception("ERROR: Unable to allocate color table.");

    totalByteCount += numBytes;

    memset(&colorArray[0], 7, numBytes);

} // end ColorTable::init

// setColor
//---------------------------------------------------------------------------
void ColorTable::setColor(int index, Uint8 color)
{
    assert(index < colorCount);

    *(colorArray + index) = color;

} // end ColorTable::setColor

// free
//---------------------------------------------------------------------------
void ColorTable::free()
{
    if (colorArray != 0) {
        ::free(colorArray);

        totalByteCount -= colorCount * sizeof(PIX);
        colorArray      = 0;
        colorCount      = 0;
    }

} // end ColorTable::free

// createBrightenFilter
//---------------------------------------------------------------------------
void ColorTable::createBrightenFilter(
    const char *filename,
    const int  &brightness)
{
    assert(brightness > 0 && brightness <= 256);
    init(256 * 256);

    if(filesystem::exists(filename)) {
        try {
            loadTable(filename);
            return;
        } catch(std::exception& e) {
            LOG( ("Error while loading palette '%s': %s", filename,
                  e.what()) );
        }
    }

    LOG ( ("Creating ColorTable '%s'.", filename) );
    // Since the file was not found, create the color tables and dump
    // it to a file.
    int   curOffset;
    int   curRed;
    int   curGreen;
    int   curBlue;
    float nb;        // The new brightness color.

    float fBrightness = float(brightness) / 256.0f;

    for (int y = 0; y < 256; y++) {
        for (int x = 0; x < 256; x++) {
            nb = float(x) * fBrightness;
            curOffset = (y * 256) + x;

            // !SOMDEDAY! Try holding a threshold when any value gets to 255.
            curRed   = (int) (nb + Palette::color[y].r);
            curGreen = (int) (nb + Palette::color[y].g);
            curBlue  = (int) (nb + Palette::color[y].b);

            if (curRed   > 255) curRed   = 255;
            if (curGreen > 255) curGreen = 255;
            if (curBlue  > 255) curBlue  = 255;
            //curColor = Palette::color[y];

            setColor(curOffset, Palette::findNearestColor(curRed, curGreen, curBlue));
        }
    }

    try {
        saveTable(filename);
    } catch(std::exception& e) {
        LOG ( ("Caching of ColorTable '%s' failed: %s", filename,
               e.what()) );
    }
} // end createBrightenFilter

// createDarkenFilter
//---------------------------------------------------------------------------
void ColorTable::createDarkenFilter(const char *filename, float fudgeValue)
{
    init(256 * 256);

    if(filesystem::exists(filename)) {
        try {
            loadTable(filename);
            return;
        } catch(std::exception& e) {
            LOG( ("Error while loading palette'%s': %s", filename, e.what()) );
        }
    }

    LOG ( ("Creating colortable '%s'.", filename) );
    // Since the file was not found, create the color tables and dump
    // it to a file.
    float    curPercent;
    int      curOffset;
    SDL_Color curColor;
    const float percent = fudgeValue;

    for (int y = 0; y < 256; y++) {
        for (int x = 0; x < 256; x++) {
            curPercent = (float(255 - x) / 255.0f) * percent + 1.0f - percent;
            curOffset  = (y * 256) + x;

            curColor.r   = (Uint8) (curPercent * float(Palette::color[y].r));
            curColor.g = (Uint8) (curPercent * float(Palette::color[y].g));
            curColor.b  = (Uint8) (curPercent * float(Palette::color[y].b));

            setColor(curOffset, Palette::findNearestColor(curColor.r, curColor.g, curColor.b));
        }
    }

    try {
        saveTable(filename);
    } catch(std::exception& e) {
        LOG ( ("Caching of ColorTable '%s' failed: %s", filename,
               e.what()) );
    }
} // end createDarkenFilter

// create
//---------------------------------------------------------------------------
void ColorTable::create(
    const int  color1Percent,
    const int  color2Percent,
    const char *filename)
{
    init(256 * 256);

    if(filesystem::exists(filename)) {
        try {
            loadTable(filename);
            return;
        } catch(std::exception& e) {
            LOG( ("Error while loading palette'%s': %s", filename, e.what()) );
        }
    }

    LOG ( ("Creating colortable '%s'.", filename) );
    //float curPercent;
    //int	  totalColors   = colorCount;
    //int   curColorIndex = 0;
    //int   num           = 0;
    //int   numInterval   = (totalColors) / 100;

    // Since the file was not found, create the color tables and dump
    // it to a file.
    unsigned curOffset = 0;

    float color1 = float(color1Percent) / 100.0f;
    float color2 = float(color2Percent) / 100.0f;

    for (unsigned index = 0; index < 256; index++) {
        const SDL_Color col = Palette::color[index];

        for (unsigned indexPic = 0; indexPic < 256; indexPic++) {
            const SDL_Color colPic = Palette::color[indexPic];

            curOffset = (int(index) << 8) + indexPic;

//            SDL_Color curColor((Uint8) (color1 * col.r   + color2 * colPic.r),
//                              (Uint8) (color1 * col.g + color2 * colPic.g),
//                              (Uint8) (color1 * col.b  + color2 * colPic.b));
            SDL_Color curColor;
            curColor.r = (Uint8) (color1 * col.r + color2 * colPic.r);
            curColor.g = (Uint8) (color1 * col.g + color2 * colPic.g);
            curColor.b = (Uint8) (color1 * col.b + color2 * colPic.b);

            // Makes the color table use color 0 as transparent.
            if (indexPic == 0) {
                setColor(curOffset, index);

            } else {
                setColor(curOffset, Palette::findNearestColor(curColor.r, curColor.g, curColor.b));
            }

            // Display a progress update every 1%.
            /*if (num > numInterval)
            {
            	curColorIndex += numInterval;
            	curPercent = float(curColorIndex) / float(totalColors);
            	num = 0;
            } else num++;
            */
        }
    }

    try {
        saveTable(filename);
    } catch(std::exception& e) {
        LOG ( ("Caching of ColorTable '%s' failed: %s", filename,
               e.what()) );
    }
} // end ColorTable::create

// loadTable
//---------------------------------------------------------------------------
void ColorTable::loadTable(const char *filename)
{
    try {
        std::unique_ptr<filesystem::ReadFile> file(filesystem::openRead(filename));

    	// make sure palette in file is the same as current one
	for(size_t i=0; i<PALETTE_LENGTH; i++) {
	    SDL_Color checkcolor;
	    file->read(&checkcolor, sizeof(Uint8), 3);
	}

	// put the color table data into the colorArray
	file->read(colorArray, colorCount, 1);
    } catch(std::exception& e) {
	throw Exception("couldn't load colortable '%s': %s",
		filename, e.what());
    }
    LOGGER.info("Loading ColorTable '%s'", filename);
} // end ColorTable::loadTable

// saveTable
//---------------------------------------------------------------------------
void ColorTable::saveTable(const char *filename) const
{
    try {
        std::unique_ptr<filesystem::WriteFile> file(
                filesystem::openWrite(filename));

    	file->write(&Palette::color, 768, 1);
        file->write(colorArray, colorCount, 1);
    } catch(std::exception& e) {
        throw Exception("error while writing to file '%s': %s",
                        filename, e.what());
    }
} // end ColorTable::saveTable

// createTrans0
//---------------------------------------------------------------------------
void ColorTable::createTrans0(
    const int  &color1Percent,
    const int  &color2Percent,
    const char *filename)
{
    init(256 * 256);

    if(filesystem::exists(filename)) {
        try {
            loadTable(filename);
            return;
        } catch(std::exception& e) {
            LOG( ("Error while loading palette'%s': %s", filename, e.what()) );
        }
    }

    LOG ( ("Creating colortable '%s'.", filename) );
    float color1        = float(color1Percent) / 100.0f;
    float color2        = float(color2Percent) / 100.0f;
    //int	  totalColors   = colorCount;
    //int   curColorIndex = 0;
    //int   num           = 0;
    //int   numInterval   = (totalColors) / 100;

    // Since the file was not found, create the color tables and dump
    // it to a file.
    unsigned curOffset = 0;

    for (unsigned index = 0; index < 256; index++) {
        const SDL_Color col = Palette::color[index];

        for (unsigned indexPic = 0; indexPic < 256; indexPic++) {
            const SDL_Color colPic = Palette::color[indexPic];

            curOffset = (int(index) << 8) + indexPic;

//            SDL_Color curColor((int) (color1 * col.r   + color2 * colPic.r),
//                              (int) (color1 * col.g + color2 * colPic.g),
//                              (int) (color1 * col.b  + color2 * colPic.b));
            SDL_Color curColor;
            curColor.r = (int) (color1 * col.r   + color2 * colPic.r);
            curColor.g = (int) (color1 * col.g + color2 * colPic.g);
            curColor.b = (int) (color1 * col.b  + color2 * colPic.b);

            // Makes the color table use color 0 as transparent.

            if (indexPic == 0) {
                setColor(curOffset, index);
            } else {

                setColor(curOffset, Palette::findNearestColor(curColor.r, curColor.g, curColor.b));
            }
        }
    }

    try {
        saveTable(filename);
    } catch(std::exception& e) {
        LOG ( ("Caching of ColorTable '%s' failed: %s",
               filename, e.what()) );
    }
} // end ColorTable::createTrans0

// lightDark table builder logic.
// 0-----------Color (x)----------255
// |
// |
// brightness (y)
// |
// |
// 255
//---------------------------------------------------------------------------
void ColorTable::createLightDarkFilter(const char *filename)
{
    init(256 * 256);

    if(filesystem::exists(filename)) {
        try {
            loadTable(filename);
            return;
        } catch(std::exception& e) {
            LOG( ("Error while loading palette'%s': %s", filename, e.what()) );
        }
    }

    LOG ( ("Creating colortable '%s'.", filename) );

    int curOffset;
    int curRed;
    int curGreen;
    int curBlue;

    for (int y = 0; y < 256; y++) {
        int x;
        for (x = 0; x <= 128; x++) {
            curOffset = x + (y << 8);
            curRed   = Palette::color[y].r   * x / 128;
            curGreen = Palette::color[y].g * x / 128;
            curBlue  = Palette::color[y].b  * x / 128;

            setColor(curOffset, Palette::findNearestColor(curRed, curGreen, curBlue));
        }
        for (x = 129; x < 256; x++) {
            curOffset = x + (y << 8);
            curRed   = Palette::color[y].r + ((255 - Palette::color[y].r) * (x-128) / 127);
            curGreen = Palette::color[y].g + ((255 - Palette::color[y].g) * (x-128) / 127);
            curBlue  = Palette::color[y].b + ((255 - Palette::color[y].b) * (x-128) / 127);

            setColor(curOffset, Palette::findNearestColor(curRed, curGreen, curBlue));
        }
    }

    try {
        saveTable(filename);
    } catch(std::exception& e) {
        LOG ( ("Caching of ColorTable '%s' failed: %s", filename, e.what()) );
    }
} // end ColorTable::createLightDarkFilter
