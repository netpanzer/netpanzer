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
#ifndef _UNITNETMESSAGE_HPP
#define _UNITNETMESSAGE_HPP

#include "NetMessage.hpp"
#include "Units/UnitState.hpp"
#include "Units/UnitBase.hpp"
#include "Units/UnitOpcodes.hpp"

#define _OPCODE_MESSAGE_LIMIT 488

enum { _net_message_id_opcode_mesg,
       _net_message_id_ini_sync_mesg,
       _net_message_id_destroy_unit,
       _net_message_id_create_unit,
       _net_message_id_unit_sync_integrity_check
     };

#ifdef MSVC
#pragma pack(1)
#endif

class UnitOpcodeMessage : public NetMessage
{
public:
    Uint8 data[ _OPCODE_MESSAGE_LIMIT ];
    
    UnitOpcodeMessage()
    {
        message_class = _net_message_class_unit;
        message_id = _net_message_id_opcode_mesg;
        memset(data, 0, sizeof(data));
    }
    
    static size_t getHeaderSize()
    {
        return sizeof(NetMessage);
    }
} __attribute__((packed));


class UnitIniSyncMessage : public NetMessage
{
public:
    Uint8 unit_type;
private:
    PlayerID player_id;
    Uint16 unit_id;
    Uint32 location_x;
    Uint32 location_y;
public:
    NetworkUnitState unit_state;

    UnitIniSyncMessage(Uint8 unit_type, PlayerID player_id, UnitID unit_id,
        Uint32 location_x, Uint32 location_y)
    {
        message_class = _net_message_class_unit;
        message_id = _net_message_id_ini_sync_mesg;
        this->unit_type = unit_type;
        //this->player_id = htol16(player_id);
        this->player_id = player_id;
        this->unit_id = htol16(unit_id);
        this->location_x = htol32(location_x);
        this->location_y = htol32(location_y);
    }
      
    unsigned short realSize() const
    {
        return( sizeof( UnitIniSyncMessage ) );
    }
    Uint32 getLocX() const
    {
        return ltoh32(location_x);
    }
    Uint32 getLocY() const
    {
        return ltoh32(location_y);
    }

    UnitID getUnitID() const
    {
        return ltoh16(unit_id);
    }
    PlayerID getPlayerID() const
    {
        return player_id;
    }
} __attribute__((packed));

// ** NOTE: A big, mother fucking HACK

class UnitRemoteDestroy : public NetMessage
{
private:
    Uint16 unit_to_destroy;

public:
    UnitRemoteDestroy()
    {
        message_class = _net_message_class_unit;
        message_id = _net_message_id_destroy_unit;
    }

    void setUnitToDestroy(UnitID id)
    {
        unit_to_destroy = htol16(id);
    }
    UnitID getUnitToDestroy() const
    {
        return ltoh16(unit_to_destroy);
    }
} __attribute__((packed));


class UnitRemoteCreate : public NetMessage
{
private:
    PlayerID player_id;
    Uint16 new_unit_id;
    Uint32 location_x;
    Uint32 location_y;
public:
    Uint8 unit_type;

    UnitRemoteCreate(PlayerID player_id, UnitID id,
            Uint32 x, Uint32 y, Uint8 type)
    {
        message_class = _net_message_class_unit;
        message_id = _net_message_id_create_unit;
        this->player_id = player_id;
        new_unit_id = htol16(id);
        location_x = htol32(x);
        location_y = htol32(y);
        unit_type = type;
    }
        
    Uint32 getLocX() const
    {
        return ltoh32(location_x);
    }
    Uint32 getLocY() const
    {
        return ltoh32(location_y);
    }
    UnitID getUnitID() const
    {
        return ltoh16(new_unit_id);
    }
    PlayerID getPlayerID() const
    {
        return player_id;
    }
} __attribute__((packed));

class UnitSyncIntegrityCheck : public NetMessage
{
public:
    UnitSyncIntegrityCheck()
    {
        message_class = _net_message_class_unit;
        message_id = _net_message_id_unit_sync_integrity_check;
    }

} __attribute__((packed));

#ifdef MSVC
#pragma pack()
#endif

#endif
