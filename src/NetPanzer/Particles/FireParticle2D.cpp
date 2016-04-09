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


#include "FireParticle2D.hpp"
#include "Util/TimerInterface.hpp"
#include "2D/Color.hpp"

void FireParticle2D::reset()
{
    totalTime = 0;
    color     = 0;
    isAlive   = false;

} // end FireParticle2D::reset

// sim
//---------------------------------------------------------------------------
// Purpose: Handles the particle simulation, no drawing.
//---------------------------------------------------------------------------
void FireParticle2D::sim()
{
    //cWindParticle2D::sim();

    totalTime += TimerInterface::getTimeSlice();
    if (totalTime > 0.5) {
        //delete this;
        //return;
        isAlive = false;
    }

} // end FireParticle2D::sim

// draw
//---------------------------------------------------------------------------
// Purpose: Draws a single particle, no simulation.
//---------------------------------------------------------------------------
void FireParticle2D::draw(Surface& dest, SpriteSorter&)
{
    //if (!isAlive) return;
    dest.fillRect(iRect(100, 100, 110, 110), Color::red);

    //int x = (int) pos.x;
    //int y = pos.y;
    //int z = (int) pos.z;

    //color = Uint8(totalTime * 44); // This 44 is based on the life of 0.25 secs and 12 colors possible.
    color = Uint8(totalTime * 22); // This 22 is based on the life of 0.50 secs and 12 colors possible.

    //if (color >= 12) throw Exception("ERROR: Invalid color reached for fire.");
    assert(color < 12);

    // const unsigned halfSize = 1;

    //dest.fillRect(iRect(x - halfSize, z - halfSize, x + halfSize, z + halfSize), cFireParticleSystem2D::fireColor[color]);

} // end FireParticle2D::draw
