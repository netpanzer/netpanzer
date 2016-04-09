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
#ifndef _TIMER_HPP
#define _TIMER_HPP

#include "TimeStamp.hpp"
#include "TimerInterface.hpp"

class Timer
{
private:
    float period;
    TimeStamp last_count;

public:
    Timer()
    { }

    Timer(float rate)
    {
        period = 1/rate;
    }

    void changeRate(float rate)
    {
        period = 1/rate;
        last_count = now();
    }

    void changePeriod(float period)
    {
        Timer::period = period;
        last_count = now();
    }

    void reset()
    {
        last_count = now();
    }

    void zero()
    {
        last_count = now() - period;
    }

    bool count()
    {
        float difference;
        float remainder;

        difference = now() - last_count;

        if ( difference >= period ) {
            remainder = difference - period;

            if ( remainder >= period )
                remainder = 0;

            last_count = now() - remainder;

            return true;
        }
        return false;
    }

    float getPeriod() const
    {
        return period;
    }

    float getRate() const
    {
        return 1/period;
    }

    float getTime() const
    {
        return( (float) (now() - last_count) );
    }

    float getTimeLeft() const
    {
        return( (float) ( period - (now() - last_count) ) );
    }

    void setTimeLeft(float timeleft)
    {
        last_count = now();
        last_count -= float(period - timeleft);
    }
};

class TimerFrameBase
{
private:
    float period;
    TimeStamp last_count;

public:
    TimerFrameBase()
    { }

    TimerFrameBase(float rate)
    {
        period = 1/rate;
    }

    void changeRate(float rate)
    {
        period = 1/rate;
        last_count = TimerInterface::getFrameStartTime();
    }

    void changePeriod(float period)
    {
        TimerFrameBase::period = period;
        last_count = TimerInterface::getFrameStartTime();
    }

    void reset()
    {
        last_count = TimerInterface::getFrameStartTime();
    }

    void zero()
    {
        last_count = TimerInterface::getFrameStartTime() - period;
    }

    bool count()
    {
        float difference;
        float remainder;

        difference = TimerInterface::getFrameStartTime() - last_count;

        if ( difference >= period ) {
            remainder = difference - period;

            if ( remainder >= period )
                remainder = 0;

            last_count = TimerInterface::getFrameStartTime() - remainder;

            return true;
        }
        return false;
    }

    float getPeriod() const
    {
        return period;
    }

    float getRate() const
    {
        return 1/period;
    }

    float getTime() const
    {
        return( (float) (TimerInterface::getFrameStartTime() - last_count) );
    }

    float getTimeLeft() const
    {
        return( (float) ( period - (TimerInterface::getFrameStartTime() - last_count) ) );
    }
};


#endif // ** _TIMER_HPP
