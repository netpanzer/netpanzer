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


#include "Util/Log.hpp"
#include "FireParticleSystem2D.hpp"
#include "Util/TimerInterface.hpp"
#include "2D/Palette.hpp"

Uint8 FireParticleSystem2D::fireColor[12];

// FireParticleSystem2D
//---------------------------------------------------------------------------
FireParticleSystem2D::FireParticleSystem2D(fXYZ pos, int isFarAway) : ParticleSystem2D(pos, isFarAway)
{
    reset();

    init();
} // end FireParticleSystem2D::FireParticleSystem2D

// reset
//---------------------------------------------------------------------------
void FireParticleSystem2D::reset()
{
    combinedTime = 0.0;
    totalTime    = 0.0;

    for (int num = 0; num < 100; num++) {
        fireParticles[num].reset();
    }

} // end FireParticleSystem2D::reset

// init
//---------------------------------------------------------------------------
void FireParticleSystem2D::init()
{
} // end FireParticleSystem2D::init

// initColors
//---------------------------------------------------------------------------
// Purpose: Goes throught the current loaded palette and finds the colors
//          which best match the specified RGB values.
//---------------------------------------------------------------------------
void FireParticleSystem2D::initColors()
{
    fireColor[ 0] = Palette::findNearestColor(255, 237, 146);
    fireColor[ 1] = Palette::findNearestColor(255, 229, 126);
    fireColor[ 2] = Palette::findNearestColor(255, 198, 130);
    fireColor[ 3] = Palette::findNearestColor(253, 191,  62);
    fireColor[ 4] = Palette::findNearestColor(250, 160,  52);
    fireColor[ 5] = Palette::findNearestColor(251, 157,  12);
    fireColor[ 6] = Palette::findNearestColor(243, 130,   3);
    fireColor[ 7] = Palette::findNearestColor(238, 105,   1);
    fireColor[ 8] = Palette::findNearestColor(223,  67,   0);
    fireColor[ 9] = Palette::findNearestColor(201,  53,   1);
    fireColor[10] = Palette::findNearestColor(171,  21,   0);
    fireColor[11] = Palette::findNearestColor(140,   3,   0);

} // end FireParticleSystem2D::initColors

// sim
//---------------------------------------------------------------------------
void FireParticleSystem2D::sim()
{
    if (totalTime < 10) {
        combinedTime += TimerInterface::getTimeSlice();

        const float offset = 7.0f;

        for (int num = 0; num < 100; num++) {
            if (!fireParticles[num].isAlive) {
                fireParticles[num].reset();
                // XXX statement with no effect
                //fireParticles[num].isAlive;
                fireParticles[num].pos.x = pos.x-offset+float(rand()%int(offset*offset*2))/offset; // [-n .. n-1]
                fireParticles[num].pos.z = pos.z-offset+float(rand()%int(offset*offset*2))/offset; // [-n .. n-1]
            }	else {
                fireParticles[num].sim();
            }
        }

        totalTime += combinedTime;
        combinedTime = 0.0;
    }	else {
        delete this;
        return;
    }

} // end FireParticleSystem2D::sim

// draw
//---------------------------------------------------------------------------
void FireParticleSystem2D::draw(const Surface &dest, SpriteSorter &sorter)
{
    (void) dest;
    (void) sorter;
    for (int num = 0; num < 100; num++) {
        //dest.fillRect(100, 100, 110, 110, Color::red);
        //fireParticles[num].draw(dest);
        if (fireParticles[num].isAlive) {
            //dest.fillRect(100, 100, 110, 110, Color::red);
            //fireParticles[num].draw(dest);
        }
    }

} // end FireParticleSystem2D::draw
