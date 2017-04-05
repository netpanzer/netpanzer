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
#include "Types/Angle.hpp"

#define _OPCODE_MESSAGE_LIMIT 488

enum { _net_message_id_opcode_mesg,
       _net_message_id_ini_sync_mesg,
       _net_message_id_destroy_unit,
       _net_message_id_create_unit,
       _net_message_id_unit_sync_integrity_check,
       _net_message_id_mod_speed,
       _net_message_id_mod_reload,
       _net_message_id_mod_fire,
       _net_message_id_mod_global_fire,
       _net_message_id_mod_w_range,
       _net_message_id_mod_hit_points,
       _net_message_id_mod_max_hit_points,
       _net_message_id_mod_global_repair,
       _net_message_id_mod_superunit,
       _net_message_id_mod_global_speed,
       _net_message_id_mod_global_reload,
       _net_message_id_create_unit_full
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

class UnitModSpeed : public NetMessage
{
private:
    Uint16 unit_id;
    Uint16 speed_rate;
    Uint16 speed_factor;

public:
    UnitModSpeed(UnitID id, Uint16 s_rate, Uint16 s_factor)
    {
        message_class = _net_message_class_unit;
        message_id = _net_message_id_mod_speed;
        unit_id = htol16(id);
        speed_rate = htol16(s_rate);
        speed_factor = htol16(s_factor);
    }

    Uint16 getUnitID() const
    {
        return ltoh16(unit_id);
    }
    Uint16 getSpeedRate() const
    {
        return ltoh16(speed_rate);
    }
    Uint16 getSpeedFactor() const
    {
        return ltoh16(speed_factor);
    }

} __attribute__((packed));

class UnitModReload : public NetMessage
{
private:
    Uint16 unit_id;
    Uint16 reload_time;

public:
    UnitModReload(UnitID id, Uint16 reload)
    {
        message_class = _net_message_class_unit;
        message_id = _net_message_id_mod_reload;
        unit_id = htol16(id);
        reload_time = htol16(reload);

    }

    Uint16 getUnitID() const
    {
        return ltoh16(unit_id);
    }
    Uint16 getReloadTime() const
    {
        return ltoh16(reload_time);
    }

} __attribute__((packed));

class UnitModFire : public NetMessage
{
private:
    Uint16 unit_id;
    Uint16 damage_factor;

public:
    UnitModFire(UnitID id, Uint16 damage)
    {
        message_class = _net_message_class_unit;
        message_id = _net_message_id_mod_fire;
        unit_id = htol16(id);
        damage_factor = htol16(damage);

    }

    Uint16 getUnitID() const
    {
        return ltoh16(unit_id);
    }
    Uint16 getFirePwp() const
    {
        return ltoh16(damage_factor);
    }

} __attribute__((packed));

class UnitModWRange : public NetMessage
{
private:
    Uint16 unit_id;
    Uint32 weapon_range;

public:
    UnitModWRange(UnitID id, Uint32 w_range)
    {
        message_class = _net_message_class_unit;
        message_id = _net_message_id_mod_w_range;
        unit_id = htol16(id);
        weapon_range = htol32(w_range);

    }

    Uint16 getUnitID() const
    {
        return ltoh16(unit_id);
    }
    Uint32 getWRange() const
    {
        return ltoh32(weapon_range);
    }

} __attribute__((packed));

class UnitModHP : public NetMessage
{
private:
    Uint16 unit_id;
    Sint16 hit_points;

public:
    UnitModHP(UnitID id, Sint16 hp)
    {
        message_class = _net_message_class_unit;
        message_id = _net_message_id_mod_hit_points;
        unit_id = htol16(id);
        hit_points = htol16(hp);

    }

    Uint16 getUnitID() const
    {
        return ltoh16(unit_id);
    }
    Sint16 getHP() const
    {
        return ltoh16(hit_points);
    }

} __attribute__((packed));

class UnitModMHP : public NetMessage
{
private:
    Uint16 unit_id;
    Sint16 max_hit_points;

public:
    UnitModMHP(UnitID id, Sint16 mhp)
    {
        message_class = _net_message_class_unit;
        message_id = _net_message_id_mod_max_hit_points;
        unit_id = htol16(id);
        max_hit_points = htol16(mhp);

    }

    Uint16 getUnitID() const
    {
        return ltoh16(unit_id);
    }
    Sint16 getMHP() const
    {
        return ltoh16(max_hit_points);
    }

} __attribute__((packed));

class UnitModGR : public NetMessage
{
private:
    UnitID unit_id;

public:
    UnitModGR(UnitID id)
    {
        message_class = _net_message_class_unit;
        message_id = _net_message_id_mod_global_repair;
        unit_id = htol16(id);
    }

    Uint16 getUnitID() const
    {
        return ltoh16(unit_id);
    }
} __attribute__((packed));

class UnitModSuperunit : public NetMessage
{
private:
    UnitID new_unit_id;
    Uint16 speed_rate;
    Uint16 speed_factor;
    Uint16 reload_time;
    Sint16 max_hit_points;
    Uint16 damage_factor;
    Uint32 weapon_range;

public:
    UnitModSuperunit(UnitID unit_id, Uint16 s_rate, Uint16 s_factor,
                     Uint16 r_time, Sint16 m_h_p,
                     Uint16 d_factor, Uint32 w_range)
    {
        message_class = _net_message_class_unit;
        message_id = _net_message_id_mod_superunit;

        new_unit_id = htol16(unit_id);

        speed_rate = htol16(s_rate);
        speed_factor = htol16(s_factor);
        reload_time = htol16(r_time);
        max_hit_points = htol16(m_h_p);

        damage_factor = htol16(d_factor);
        weapon_range = htol32(w_range);
    }

    UnitID getUnitID() const
    {
        return ltoh16(new_unit_id);    }

    Uint16 getSpeedRate() const
    {
        return ltoh16(speed_rate);
    }
    Uint16 getSpeedFactor() const
    {
        return ltoh16(speed_factor);
    }
    Uint16 getReloadTime() const
    {
        return ltoh16(reload_time);
    }
    Uint16 getMaxHitPoints() const
    {
        return ltoh16(max_hit_points);
    }

    Uint16 getDamageFactor() const
    {
        return ltoh16(damage_factor);
    }
    Uint32 getWeaponRange() const
    {
        return ltoh32(weapon_range);
    }
} __attribute__((packed));

class UnitModGS : public NetMessage
{
private:
    UnitID unit_id;
    unsigned short top_speed_rate;

public:
    UnitModGS(UnitID id, unsigned short tsr)
    {
        message_class = _net_message_class_unit;
        message_id = _net_message_id_mod_global_speed;
        unit_id = htol16(id);
        top_speed_rate = htol16(tsr);
    }

    Uint16 getUnitID() const
    {
        return ltoh16(unit_id);
    }

    Uint16 getTopSpeed() const
    {
        return ltoh16(top_speed_rate);
    }

} __attribute__((packed));

class UnitModGRT : public NetMessage
{
private:
    UnitID unit_id;
    unsigned short top_reload_time;

public:
    UnitModGRT(UnitID id, unsigned short trt)
    {
        message_class = _net_message_class_unit;
        message_id = _net_message_id_mod_global_reload;
        unit_id = htol16(id);
        top_reload_time = htol16(trt);
    }

    Uint16 getUnitID() const
    {
        return ltoh16(unit_id);
    }

    Uint16 getTopReload() const
    {
        return ltoh16(top_reload_time);
    }

} __attribute__((packed));

class UnitModGDM : public NetMessage
{
private:
    UnitID unit_id;
    unsigned short top_damage_factor;

public:
    UnitModGDM(UnitID id, unsigned short tdf)
    {
        message_class = _net_message_class_unit;
        message_id = _net_message_id_mod_global_fire;
        unit_id = htol16(id);
        top_damage_factor = htol16(tdf);

    }

    Uint16 getUnitID() const
    {
        return ltoh16(unit_id);
    }

    Uint16 getTopDamage() const
    {
        return ltoh16(top_damage_factor);
    }

} __attribute__((packed));





class UnitRemoteCreateFull : public NetMessage
{
private:
    PlayerID player_id;
    Uint16 new_unit_id;
    Uint32 location_x;
    Uint32 location_y;

    //AngleInt body_angle;
    //AngleInt turret_angle;
    Sint32 body_angle_angle_int;
    Uint32 body_angle_grain;
    Sint32 body_angle_angle_limit;
    Sint32 turret_angle_angle_int;
    Uint32 turret_angle_grain;
    Sint32 turret_angle_angle_limit;
    Uint16 orientation;
    Uint16 speed_rate;
    Uint16 speed_factor;
    Uint16 reload_time;
    Sint16 max_hit_points;
    Sint16 hit_points;
    Uint16 damage_factor;
    Uint32 weapon_range;
    Uint32 defend_range;


public:
    Uint8 unit_type;
    Uint8 unit_style;
    Uint8 moving;

    UnitRemoteCreateFull(PlayerID player_id, UnitID id, Uint8 style, Uint8 mov,
            Uint32 x, Uint32 y, Uint8 type,
            AngleInt b_angle, AngleInt t_angle,
            Uint16 orient, Uint16 s_rate, Uint16 s_factor,
            Uint16 r_time, Sint16 m_h_p, Sint16 h_p,
            Uint16 d_factor, Uint32 w_range, Uint32 d_range)
    {
        message_class = _net_message_class_unit;
        message_id = _net_message_id_create_unit_full;
        this->player_id = player_id;
        new_unit_id = htol16(id);
        location_x = htol32(x);
        location_y = htol32(y);
        unit_type = type;
        unit_style = style;
        moving = mov;

        body_angle_angle_int = htol32(b_angle.angle_int);
        body_angle_grain = htol32(b_angle.grain);
        body_angle_angle_limit = htol32(b_angle.angle_limit);

        turret_angle_angle_int = htol32(t_angle.angle_int);
        turret_angle_grain = htol32(t_angle.grain);
        turret_angle_angle_limit = htol32(t_angle.angle_limit);

        orientation = htol16(orient);
        speed_rate = htol16(s_rate);
        speed_factor = htol16(s_factor);
        reload_time = htol16(r_time);
        max_hit_points = htol16(m_h_p);
        hit_points = htol16(h_p);
        damage_factor = htol16(d_factor);
        weapon_range = htol32(w_range);
        defend_range = htol32(d_range);
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

    long getBodyAngleAI() const
    {
        return  ltoh32(body_angle_angle_int);
    }
    unsigned long getBodyAngleGrain() const
    {
        return  ltoh32(body_angle_grain);
    }
    long getBodyAngleAL() const
    {
        return  ltoh32(body_angle_angle_limit);
    }

    long getTurretAngleAI() const
    {
        return  ltoh32(turret_angle_angle_int);
    }
    unsigned long getTurretAngleGrain() const
    {
        return  ltoh32(turret_angle_grain);
    }
    long getTurretAngleAL() const
    {
        return  ltoh32(turret_angle_angle_limit);
    }

    Uint16 getOrientation() const
    {
        return ltoh16(orientation);
    }
    Uint16 getSpeedRate() const
    {
        return ltoh16(speed_rate);
    }
    Uint16 getSpeedFactor() const
    {
        return ltoh16(speed_factor);
    }
    Uint16 getReloadTime() const
    {
        return ltoh16(reload_time);
    }
    Uint16 getMaxHitPoints() const
    {
        return ltoh16(max_hit_points);
    }
    Uint16 getHitPoints() const
    {
        return ltoh16(hit_points);
    }
    Uint16 getDamageFactor() const
    {
        return ltoh16(damage_factor);
    }
    Uint32 getWeaponRange() const
    {
        return ltoh32(weapon_range);
    }
    Uint32 getDefendRange() const
    {
        return ltoh32(defend_range);
    }



} __attribute__((packed));







#ifdef MSVC
#pragma pack()
#endif

#endif
