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


#include "TimerInterface.hpp"

bool gTimeSliceFlag = false;

TimeStamp TimerInterface::frameStartTime;
unsigned  TimerInterface::framesUntilNextFPSUpdate = 0;
double    TimerInterface::totalFPSGroupTime        = 0;
double    TimerInterface::fps                      = 0;
double    TimerInterface::fpsAVG                   = 0;
double    TimerInterface::timeSlice                = 0;

// start
//--------------------------------------------------------------------------
void TimerInterface::start()
{
    frameStartTime = now();

} // end TimerInterface::start

// update
//--------------------------------------------------------------------------
void TimerInterface::update()
{
    // Get the current time of the frame.
    timeSlice = now() - frameStartTime;

    if (timeSlice > 0.0f) {
        fps = 1.0f / timeSlice;
    }

    if (framesUntilNextFPSUpdate >= FRAMES_PER_FPS_UPDATE) {
        if (totalFPSGroupTime > 0.0f) {
            fpsAVG = FRAMES_PER_FPS_UPDATE / totalFPSGroupTime;

        } else {
            fpsAVG = 0.0f;
        }

        framesUntilNextFPSUpdate = 0;
        totalFPSGroupTime = 0.0f;

    }	else {
        totalFPSGroupTime += timeSlice;
        framesUntilNextFPSUpdate++;
    }

} // end TimerInterface::update
