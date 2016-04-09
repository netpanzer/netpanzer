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
#ifndef _UNITOPCODES_HPP
#define _UNITOPCODES_HPP

#include "Units/UnitBase.hpp"
#include <queue>

enum { _unit_opcode_flag_sync = 0x01 };

#ifdef MSVC
#pragma pack(1)
#endif

#define _UNIT_OPCODE_MOVE 1
#define _UNIT_OPCODE_TURRET_TRACK_POINT 2
#define _UNIT_OPCODE_TURRET_TRACK_TARGET 3
#define _UNIT_OPCODE_FIRE_WEAPON 4
#define _UNIT_OPCODE_SYNC_UNIT 5
#define _UNIT_OPCODE_UPDATE_STATE 6
#define _UNIT_OPCODE_DESTRUCT 7

// do not use this directly, cast to 1 of the UnitOpcode classes...
struct UnitOpcodeStruct
{
public:
    Uint8 opcode;
private:
    Uint16 unit_index;
public:
    Uint8 flags;
private:
    Uint8 op_data[7];
} __attribute__((packed));

typedef std::queue<UnitOpcodeStruct> UnitOpcodeQueue;

class UnitOpcode
{
public:
    Uint8 opcode;
    //Uint8 player_index;
private:
    Uint16 unit_id;
public:
    Uint8 flags;

    void setUnitID(UnitID id)
    {
        unit_id = htol16(id);
    }

    UnitID getUnitID()
    {
        return ltoh16(unit_id);
    }
    Uint8 getSize() const
    {
        switch (opcode)
        {
        case _UNIT_OPCODE_MOVE:
            return(sizeof(UnitOpcode)+sizeof(Sint8)*2+sizeof(Uint32));
            break;
        case _UNIT_OPCODE_TURRET_TRACK_POINT:
            return(sizeof(UnitOpcode)+sizeof(Uint16)*2+sizeof(Uint8));
            break;
        case _UNIT_OPCODE_TURRET_TRACK_TARGET:
            return(sizeof(UnitOpcode)+sizeof(Uint8)+sizeof(Uint16));
            break;
        case _UNIT_OPCODE_FIRE_WEAPON:
            return(sizeof(UnitOpcode)+sizeof(Uint16)*2);
            break;
        case _UNIT_OPCODE_SYNC_UNIT:
            return(sizeof(UnitOpcode));
            break;
        case _UNIT_OPCODE_UPDATE_STATE:
            return(sizeof(UnitOpcode)+sizeof(Sint16));
            break;
        case _UNIT_OPCODE_DESTRUCT:
            return(sizeof(UnitOpcode));
            break;
        default:
            return(0);
            break;
        }
    }
} __attribute__((packed));


class MoveOpcode : public UnitOpcode
{
private:
    Uint32 square;
public:
    Sint8 loc_x_offset;
    Sint8 loc_y_offset;

    MoveOpcode( )
    {
        flags = 0;
        opcode = _UNIT_OPCODE_MOVE;

        square = 0;
        loc_x_offset = 0;
        loc_y_offset = 0;
    }

    void setSquare(Uint32 square)
    {
        this->square = htol32(square);
    }

    Uint32 getSquare() const
    {
        return ltoh32(square);
    }
} __attribute__((packed));


class TurretTrackPointOpcode : public UnitOpcode
{
private:
    Uint16 target_x;
    Uint16 target_y;

public:
    Uint8  activate;

    TurretTrackPointOpcode( )
    {
        flags = 0;
        opcode = _UNIT_OPCODE_TURRET_TRACK_POINT;

        target_x = target_y = 0;
        activate = false;
    }

    void setTarget(iXY pos)
    {
        target_x = htol16(pos.x);
        target_y = htol16(pos.y);
    }

    iXY getTarget() const
    {
        return iXY(ltoh16(target_x), ltoh16(target_y));
    }
} __attribute__((packed));


class TurretTrackTargetOpcode : public UnitOpcode
{
private:
    Uint16 targetUnitID;
public:
    Uint8 activate;

    TurretTrackTargetOpcode( )
    {
        flags = 0;
        opcode = _UNIT_OPCODE_TURRET_TRACK_TARGET;

        targetUnitID = 0;
        activate = false;
    }

    void setTargetUnitID(UnitID id)
    {
        targetUnitID = htol16(id);
    }

    UnitID getTargetUnitID() const
    {
        return ltoh16(targetUnitID);
    }
} __attribute__((packed));


class FireWeaponOpcode : public UnitOpcode
{
private:
    Uint16 x;
    Uint16 y;
public:

    FireWeaponOpcode( )
    {
        flags = 0;
        opcode = _UNIT_OPCODE_FIRE_WEAPON;
    }

    void setTarget(iXY target)
    {
        x = htol16(target.x);
        y = htol16(target.y);
    }

    iXY getTarget() const
    {
        return iXY(ltoh16(x), ltoh16(y));
    }

} __attribute__((packed));


class SyncUnitOpcode : public UnitOpcode
{
public:
    SyncUnitOpcode( )
    {
        flags = 0;
        opcode = _UNIT_OPCODE_SYNC_UNIT;
    }
} __attribute__((packed));


class UpdateStateUnitOpcode : public UnitOpcode
{
private:
    Sint16 hit_points;
public:
    UpdateStateUnitOpcode( )
    {
        flags = 0;
        opcode = _UNIT_OPCODE_UPDATE_STATE;
    }

    void setHitPoints(Sint16 newhitpoints)
    {
        hit_points = htol16(newhitpoints);
    }

    Sint16 getHitPoints() const
    {
        return ltoh16(hit_points);
    }
} __attribute__((packed));


class DestructUnitOpcode : public UnitOpcode
{
public:
    DestructUnitOpcode( )
    {
        flags = 0;
        opcode = _UNIT_OPCODE_DESTRUCT;
    }
} __attribute__((packed));

#ifdef MSVC
#pragma pack()
#endif

#endif
