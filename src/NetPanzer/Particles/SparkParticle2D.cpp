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


#include "SparkParticle2D.hpp"
#include "Util/TimerInterface.hpp"

Surface SparkParticle2D::sparkSprite;

//---------------------------------------------------------------------------
SparkParticle2D::SparkParticle2D(fXYZ nPos) : Particle2D(nPos)
{
    curFrame = 0;

    // XXX only a setData for packedSurface defined?!?
    // packedSurface.setData(sparkSprite);

    // XXX
    //packedSurface.yPos = rand() % 20 + 10;

} // end SparkParticle2D

// init
//---------------------------------------------------------------------------
void SparkParticle2D::init()
{
    //sparkSprite.create(iXY(96, 128), 96, 16);
    //sparkSprite.extractPCX("pics/particles/explosion/pcx/flack1.pcx", 5, 0);
    //sparkSprite.setOffsetCenter();

    //explosion.loadRAW("pics/explode/expl0002.raw");
    //spark.create(iXY(64, 64), 64, 16);
    //spark.fill(Color::red);
    //spark.extractPCX("pics/explode/expl01.pcx", 4, 0);
    //spark.scale(iXY(20, 20));

} // end SparkParticle2D::init

// draw
//---------------------------------------------------------------------------
void SparkParticle2D::draw(const Surface&, SpriteSorter &sorter)
{
    if (!isAlive) return;

    assert(packedSurface.getFrameCount() > 0);

    curFrame += TimerInterface::getTimeSlice() * 20;

    //int x = pos.x-mainViewMin.x;
    //int y = pos.y;
    //int z = pos.z-mainViewMin.y;

    // Draw the explosion frames.
    if (int(curFrame) >= packedSurface.getFrameCount()) {
        delete this;
        return;
    }	else {
        packedSurface.setFrame(curFrame);
        packedSurface.setAttrib( iXY((int) pos.x, (int) pos.z), 3);
        sorter.addSprite(&packedSurface);
        //spark.blt(dest, iXY(x, z));
    }

} // end SparkParticle2D::draw
