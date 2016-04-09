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
#include "2D/Color.hpp"
#include "SnowParticle2D.hpp"

class SnowParticleSystem2D;

//---------------------------------------------------------------------------
SnowParticle2D::SnowParticle2D()
{
    reset();
    color    = Color::white;//24+rand()%8;
    lifetime = 2 + rand() % 5;
    //SnowParticleSystem2D::snowParticleCount++;

} // end SnowParticle2D constructor

// reset
//---------------------------------------------------------------------------
void SnowParticle2D::reset()
{
    pos               = 0;
    totalTime         = 0;
    color             = 0;
    size              = 0;
    age               = 0;
    lifetime          = 0;
    timeElapsedPeriod = 0;

} // end SnowParticle2D::reset

// init
//---------------------------------------------------------------------------
void SnowParticle2D::init(fXYZ pos, unsigned nSize)
{
    this->pos  = fXYZ(pos.x, 0, pos.y);
    size = nSize;

} // end SnowParticle2D::init

// sim
//---------------------------------------------------------------------------
// Purpose: Handles the particle simulation, no drawing.
//---------------------------------------------------------------------------
void SnowParticle2D::sim()
{
    /*
    	float x = gravity.wind.ParticleSystem2D::wind.x;
    	float z = ParticleSystem2D::wind.z;
     
    	if (timeElapsedPeriod > 2.0 && size >= 1)
    	{
    		size--;
    		timeElapsedPeriod = 0;
    	}
     
    	age               += TimerInterface::getTimeSlice();
    	timeElapsedPeriod += TimerInterface::getTimeSlice();
     
    	pos += fXYZ(x+x*(rand()%3), 0, z+z*(rand()%3));
     
    	// Handle the wrap around.
    	if      (pos.x < 0)            pos.x = SCREEN_XSIZE-1;
    	else if (pos.x > SCREEN_XSIZE) pos.x = 1;
     
    	if      (pos.z < 0)            pos.z = SCREEN_YSIZE-1;
    	else if (pos.z > SCREEN_YSIZE) pos.z = 1;
     
    	// If the particle goes off the screen, then start up a new particle 
    	// randomly on the screen.
    	if (age > lifetime || size == 0 && timeElapsedPeriod > 2.0)
    	{
    		pos      = fXYZ(rand()%SCREEN_XSIZE, 0, rand()%SCREEN_YSIZE);
    		size     = rand()%4;
    		lifetime = 2+rand()%5;
    		age      = 0;
    	}
    */
} // end SnowParticle2D::sim

// draw
//---------------------------------------------------------------------------
// Purpose: Draws a single particle, no simulation.
//---------------------------------------------------------------------------
void SnowParticle2D::draw(const Surface &dest, SpriteSorter &sorter)
{
    (void) dest;
    (void) sorter;
    //if      (size == 1) dest.putPixel(iXY(pos.x, pos.z), color);
    //else if (size == 2) dest.fillRect(iRect(pos.x, pos.z, pos.x+1, pos.z+1), color);
    //else if (size == 3) dest.fillRect(iRect(pos.x, pos.z, pos.x+2, pos.z+2), color);
    //else                dest.fillRect(iRect(pos.x, pos.z, pos.x+3, pos.z+3), color);

} // end SnowParticle2D::draw
