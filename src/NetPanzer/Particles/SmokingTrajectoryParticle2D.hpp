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

#ifndef __SmokingTrajectoryParticle2D_hpp__
#define __SmokingTrajectoryParticle2D_hpp__


#include "TrajectoryParticle2D.hpp"
#include "PuffParticle2D.hpp"


// SmokingTrajectoryParticle2D
//--------------------------------------------------------------------------
class SmokingTrajectoryParticle2D : public TrajectoryParticle2D
{
protected:
    float waitMin;
    float waitRand;
    float waitTime;
    float incrementScale;

    // XXX PUFF_PARTICLE_TYPE ?!? added a definition here
    typedef int PUFF_PARTICLE_TYPE;
    PUFF_PARTICLE_TYPE particleType; // Which kind of particle to use for this explosion.

public:
    SmokingTrajectoryParticle2D(const fXYZ &pos, int maxSpeed, float waitMin,
                                float waitRand, PUFF_PARTICLE_TYPE particleType);

    virtual void sim();

}
; // end SmokingTrajectoryParticle2D

#endif // __SmokingTrajectoryParticle2D_hpp__
