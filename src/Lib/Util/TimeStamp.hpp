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
#ifndef __TimeStamp_hpp__
#define __TimeStamp_hpp__

#include <SDL2/SDL.h>

/**
 * Defines a way to record a point in time.  The actual value
 * recorded is irrelevent (and hidden), but the user of this class is able
 * to determine if one timestamp is before or after another timestamp, and
 * how much time is marked between two timestamps (returned as a float, in
 * seconds), and adjust a timestamp by a certain number of seconds. 
 */
class TimeStamp
{
private:
    typedef Uint32 Ticks;
    Ticks v; // ticks count

    inline static double ticksToSecs(Ticks t)
    {
        return double(t) / 1000.0;
    }
    inline static Ticks secsToTicks(float s)
    {
        return (Ticks) (s * 1000.0);
    }

public:
    TimeStamp(Ticks nV = 0)
    {
        v = nV;
    }
    TimeStamp(const TimeStamp& other)
    {
        v = other.v;
    }

    TimeStamp &operator =(const TimeStamp& other)
    {
        v  = other.v; return *this;
    }

    TimeStamp &operator +=(float seconds)
    {
        v += secsToTicks(seconds); return *this;
    }
    TimeStamp &operator -=(float seconds)
    {
        v -= secsToTicks(seconds); return *this;
    }

    TimeStamp operator +(float seconds) const
    {
        return TimeStamp(v + secsToTicks(seconds));
    }
    TimeStamp operator -(float seconds) const
    {
        return TimeStamp(v - secsToTicks(seconds));
    }

    double getSecs() const
    {
        return ticksToSecs(v);
    }

    double operator -(const TimeStamp &a) const
    {
        return ticksToSecs(v - a.v);
    }
    bool operator < (const TimeStamp &a) const
    {
        return v < a.v;
    }
    bool operator <=(const TimeStamp &a) const
    {
        return v <= a.v;
    }
    bool operator >=(const TimeStamp &a) const
    {
        return v >= a.v;
    }
    bool operator > (const TimeStamp &a) const
    {
        return v > a.v;
    }
    bool operator ==(const TimeStamp &a) const
    {
        return v == a.v;
    }

    /** returns the ticks count. This is intended for network packets only! */
    Uint32 getNetworkTicks() const
    {
        return v;
    }
};

// Return the current timestamp
extern TimeStamp now();

#endif
