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
#ifndef _UNITPROFILEINTERFACE_HPP
#define _UNITPROFILEINTERFACE_HPP

#include <string>
#include <vector>

#include "Units/UnitTypes.hpp"
#include "Units/UnitState.hpp"
#include "2D/PackedSurface.hpp"
#include "Interfaces/GameConfig.hpp"
#include "Interfaces/GameManager.hpp"
#include "Core/CoreTypes.hpp"

class NetMessage;

class UnitProfile
{
public:
    NPString unitname;
    Uint16 unit_type;
    Uint16 hit_points;
    Uint16 attack_factor;
    Uint32 attack_range;
    Uint16 cfg_attack_range;
    Uint32  defend_range;
    Uint16 cfg_defend_range;
    Uint8  speed_factor;
    Uint8  speed_rate;
    Uint8  reload_time;
    Uint16 regen_time;
    NPString imagefile;
    NPString bodySprite_name;
    NPString bodyShadow_name;
    NPString turretSprite_name;
    NPString turretShadow_name;
    //PackedSurface bodySprite;
    //PackedSurface bodyShadow;
    //PackedSurface turretSprite;
    //PackedSurface turretShadow;
    NPString soundSelected;
    NPString fireSound;
    NPString weaponType;
    Uint16 boundBox;
};

class UnitProfileSprites
{
public:
    PackedSurface bodySprite;
    PackedSurface bodyShadow;
    PackedSurface turretSprite;
    PackedSurface turretShadow;

protected:

public:
    static std::vector<UnitProfileSprites *> profiles_sprites;
    static UnitProfileSprites * getUnitProfileSprites( unsigned short vector_index );
    static void clearProfiles();

};

class UnitProfileInterface
{
protected:
    static std::vector<UnitProfile *> profiles;
    static void doLoadUnitProfiles();


private:
    static std::vector<unsigned short> su_speed_rate;
    static std::vector<unsigned short> su_speed_factor;
    static std::vector<unsigned short> su_speed;
    static std::vector<short> su_hit_points;
    static std::vector<short> su_damage_factor;
    static std::vector<unsigned short> su_reload_time;
    static std::vector<unsigned long> su_weapon_range;

public:

    static unsigned short tsu_speed_rate;
    static unsigned short tsu_speed_factor;
    static unsigned short tsu_speed;
    static short tsu_hit_points;
    static short tsu_damage_factor;
    static unsigned short tsu_reload_time;
    static unsigned long tsu_weapon_range;

    static void clearProfiles();

    static bool addLocalProfile(const NPString& name);

    static void loadUnitProfiles( void );


    static UnitProfile * getUnitProfile( unsigned short unit_type );
    static UnitProfile * getProfileByName( const NPString& name );

    static unsigned int getNumUnitTypes()
    {
        return profiles.size();
    }

    static void cleaning();

    static int fillProfileSyncMessage(NetMessage* message, int profile_id);
    static int fillProfileResetMessage(NetMessage* message);
    static UnitProfile* loadProfileFromMessage(const NetMessage* message, size_t size);
    static void processNetMessage(const NetMessage* net_message, size_t size);
    static void handleProfileDescMessage(const NetMessage* net_message, size_t size);
};


#endif // ** _UNITPROFILEINTERFACE_HPP
