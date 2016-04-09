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


#include <string.h>
#include "Span.hpp"

int allowSpanBlitting = 1;

//--------------------------------------------------------------------------
void bltBlendSpan(PIX *dRow, const PIX *sRow, size_t pixelsPerRow,
                  const Uint8 *table)
{
    if (!allowSpanBlitting) {
        return;
    } // Remove for release candidate.

    for(size_t idx=0;idx<pixelsPerRow;idx++) {
        dRow[idx] = table[ (dRow[idx]<<8)+sRow[idx] ];
    }
} // end bltBlendSpan

//--------------------------------------------------------------------------
void bltBlendScaleSpan(PIX *dRow, const PIX *sRow, int srcX1FracWithCount, int
                       stepAndDecCount, int stepWholePart, const Uint8 *table)
{
    if (!allowSpanBlitting) {
        return;
    } // Remove for release candidate.

    int stepDecPart = stepAndDecCount>>16;
    int stepCount=srcX1FracWithCount>>16;
    for(int i=0; i < (srcX1FracWithCount&0xffff); i++)
    {
        dRow[i] = table[(dRow[i]<<8)+*sRow];
        sRow+=stepWholePart;
        stepCount+=stepDecPart;
        if(stepCount>0xffff) {
            stepCount-=0xffff;
            sRow+=1;
        }
    }
} // end bltBlendScaleSpan

//--------------------------------------------------------------------------
void bltLookupSpan(PIX *dRow, int pixelsPerRow, const PIX *table)
{
    if (!allowSpanBlitting) {
        return;
    } // Remove for release candidate.

    for(int idx=0;idx<pixelsPerRow;idx++)
        dRow[idx] = table[dRow[idx]];
} // end bltLookupSpan

//--------------------------------------------------------------------------
void bltScaleSpan(PIX *dRow, const PIX *sRow, int srcX1FracWithCount,
                  int stepAndDecCount, int stepWholePart)
{
    if (!allowSpanBlitting) {
        return;
    } // Remove for release candidate.
    int stepDecPart = stepAndDecCount>>16;
    int stepCount=srcX1FracWithCount>>16;
    for(int i=0; i < (srcX1FracWithCount&0xffff); i++)
    {
        dRow[i] = *sRow;
        sRow+=stepWholePart;
        stepCount+=stepDecPart;
        if(stepCount>0xffff) {
            stepCount-=0xffff;
            sRow+=1;
        }
    }
} // end bltScaleSpan

