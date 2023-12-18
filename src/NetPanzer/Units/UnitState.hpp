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
#ifndef _UNITSTATE_HPP
#define _UNITSTATE_HPP

#include <string.h>

#include "ArrayUtil/BoundBox.hpp"
#include "Types/Angle.hpp"

#include "UnitLifecycles.hpp"

enum { _threat_level_all_clear,
       _threat_level_under_attack,
       _threat_level_defending
     };

#ifdef MSVC
#pragma pack(1)
#endif

class NetworkUnitState
{
public:
    NetworkUnitState()
    { }

private:
    Uint8     unit_type;

    Sint32     location_x;
    Sint32     location_y;
    Sint32     bbox_min_x;
    Sint32     bbox_min_y;
    Sint32     bbox_max_x;
    Sint32     bbox_max_y;

    NetworkAngleInt body_angle;
    NetworkAngleInt turret_angle;

    Uint16    orientation;
    Uint16    speed_rate;
    Uint16    speed_factor;

    Uint16    reload_time;
    Sint16    max_hit_points;
    Sint16    hit_points;
    Uint16    damage_factor;
    Uint32    weapon_range;
    Uint32    defend_range;

    Uint8     unit_style;
    Uint8     moving;

    Uint8 threat_level;
    Uint8 lifecycle_state;

    friend class UnitState;
} __attribute__((packed));

#ifdef MSVC
#pragma pack()
#endif

class UnitState
{
public:
    unsigned char  unit_type;

    /// true if the unit is currently selected
    bool           select;
    /// position of the unit
    iXY            location;
    BoundBox       bbox;

    AngleInt       body_angle;
    AngleInt       turret_angle;

    unsigned short orientation;
    unsigned short speed_rate;
    unsigned short speed_factor;

    unsigned short reload_time;
    short	       max_hit_points;
    short          hit_points;
    unsigned short damage_factor;
    unsigned long  weapon_range;
    unsigned long  defend_range;

    unsigned char  unit_style;
    bool           moving;

    unsigned char  threat_level;
    unsigned char  lifecycle_state;

    UnitState( );

    bool bounds(const iXY& loc) const
    {
        return( bbox.bounds( location, loc ) );
    }

    int percentDamageInt();
    float percentDamageFloat();

    NetworkUnitState getNetworkUnitState() const;
    void setFromNetworkUnitState(const NetworkUnitState& state);
};

#endif

