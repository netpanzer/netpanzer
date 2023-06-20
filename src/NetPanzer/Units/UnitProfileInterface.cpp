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


#include "Units/UnitProfileInterface.hpp"
#include "Interfaces/GameConfig.hpp"
#include "Util/Exception.hpp"
#include "Util/FileSystem.hpp"
#include "Util/Log.hpp"
#include "Scripts/ScriptManager.hpp"
#include <ctype.h>
#include <memory>
#include <string.h>
#include <algorithm>
#include <vector>

#include "Classes/Network/NetMessage.hpp"
#include "Classes/Network/NetPacket.hpp"

#include "Util/StringUtil.hpp"

#include <iostream>


unsigned short UnitProfileInterface::tsu_speed_rate;
unsigned short UnitProfileInterface::tsu_speed_factor;
unsigned short UnitProfileInterface::tsu_speed;
short UnitProfileInterface::tsu_hit_points;
short UnitProfileInterface::tsu_damage_factor;
unsigned short UnitProfileInterface::tsu_reload_time;
unsigned long UnitProfileInterface::tsu_weapon_range;

std::vector<UnitProfile *> UnitProfileInterface::profiles;

std::vector<UnitProfileSprites *> UnitProfileSprites::profiles_sprites;

enum
{
    _profile_msg_profile_desc = 0,
    _profile_msg_reset
};

class ByteBufferWriter
{
private:
    unsigned char * buffer;
    size_t len;
    size_t pos;

public:
    ByteBufferWriter(unsigned char * buffer, int len)
    {
        this->buffer = buffer;
        this->len = len;
        this->pos = 0;
    }

    int writedBytesCount() const { return pos; }

    bool writeInt8( Uint8 c )
    {
        if ( len >= pos+1 )
        {
            buffer[pos++] = c;
            return true;
        }
        return false;
    }

    bool writeInt16( Uint16 c )
    {
        if ( len >= pos+2 )
        {
            buffer[pos++] = c & 0xff;
            buffer[pos++] = (c >> 8) & 0xff;
            return true;
        }
        return false;
    }

    bool writeInt32( Uint32 c )
    {
        if ( len >= pos+4 )
        {
            buffer[pos++] = c & 0xff;
            buffer[pos++] = (c >> 8) & 0xff;
            buffer[pos++] = (c >> 16) & 0xff;
            buffer[pos++] = (c >> 24) & 0xff;
            return true;
        }
        return false;
    }

    bool writeString( const NPString& str)
    {
        if ( len >= pos + 2 + str.length() )
        {
            writeInt16(str.length());
            str.copy((char *)&buffer[pos], str.length());
            pos += str.length();
            return true;
        }
        return false;
    }
};

class ByteBufferReader
{
private:
    const unsigned char * buffer;
    size_t len;
    size_t pos;

public:
    ByteBufferReader(const unsigned char * buffer, int len)
    {
        this->buffer = buffer;
        this->len = len;
        this->pos = 0;
    }

    int readedBytesCount() const { return pos; }

    bool readInt8( Uint8* c )
    {
        if ( pos < len )
        {
            *c = buffer[pos++];
            return true;
        }
        return false;
    }

    bool readInt16( Uint16* c )
    {
        if ( len >= pos+2 )
        {
            *c = buffer[pos] | (buffer[pos+1] << 8);
            pos+=2;
            return true;
        }
        return false;
    }

    bool readInt32( Uint32* c )
    {
        if ( len >= pos+4 )
        {
            *c = buffer[pos]
               | (buffer[pos+1] << 8)
               | (buffer[pos+2] << 16)
               | (buffer[pos+3] << 24);
            pos += 4;
            return true;
        }
        return false;
    }

    bool readString( NPString& str)
    {
        Uint16 slen = 0;
        if ( readInt16(&slen) )
        {
            if ( len >= pos+slen )
            {
                if ( slen > 0 )
                {
                    str.assign((char *)&buffer[pos], slen);
                    pos += str.length();
                }
                else
                {
                    str.clear();
                }
                return true;
            }
            else
            {
                pos -= 2;
            }
        }
        else
        {
            pos -= 2;
        }
        return false;
    }
};

bool read_vehicle_profile(const NPString& unitName, UnitProfile *profile)
{
    int temp_int;

    NPString file_path = "units/profiles/";
    file_path += unitName;
    file_path += ".upf";


    profile->unitname = unitName;


    bool isok = ScriptManager::loadSimpleConfig(file_path);
    if ( isok )
    {
        // loaded file in top
        profile->hit_points       = ScriptManager::getIntField("hitpoints",    100);
        profile->attack_factor    = ScriptManager::getIntField("attack",       100);
        profile->reload_time      = ScriptManager::getIntField("reload",       100);
        profile->cfg_attack_range = ScriptManager::getIntField("range",         10);
        profile->cfg_defend_range = ScriptManager::getIntField("defend_range",  10);
        profile->regen_time       = ScriptManager::getIntField("regen",         60);
        profile->speed_rate       = ScriptManager::getIntField("speed_rate",    16);
        profile->speed_factor     = ScriptManager::getIntField("speed_factor",   2);
        profile->boundBox         = ScriptManager::getIntField("boundbox",      40);

        profile->imagefile         = ScriptManager::getStringField("image",        "");
        profile->bodySprite_name   = ScriptManager::getStringField("bodysprite",   "");
        profile->bodyShadow_name   = ScriptManager::getStringField("bodyshadow",   "");
        profile->turretSprite_name = ScriptManager::getStringField("turretsprite", "");
        profile->turretShadow_name = ScriptManager::getStringField("turretshadow", "");
        profile->soundSelected     = ScriptManager::getStringField("soundselected","");
        profile->fireSound         = ScriptManager::getStringField("soundfire",    "");
        profile->weaponType        = ScriptManager::getStringField("weapon",       "");

        ScriptManager::popElements(1);

        temp_int = profile->cfg_attack_range << 5;
        profile->attack_range = temp_int * temp_int;

        temp_int = profile->cfg_defend_range << 5;
        profile->defend_range = temp_int * temp_int;

        // just a check here, but it's  just the server

        int style_index = 0;
        while ( style_index < GameConfig::getUnitStylesNum() )
        {
        UnitProfileSprites * ups = new UnitProfileSprites();
        NPString spath = GameConfig::getUnitStyle(style_index);
        NPString ustylepath = "units/pics/pak/" + spath + "/";

        try
        {
            ups->bodySprite.load(ustylepath+profile->bodySprite_name);
            ups->bodyShadow.load(ustylepath+profile->bodyShadow_name);
            ups->turretSprite.load(ustylepath+profile->turretSprite_name);
            ups->turretShadow.load(ustylepath+profile->turretShadow_name);

        }
        catch (std::exception& e)
        {
            LOGGER.warning("Error loading unitprofile sprites '%s': %s",
                           file_path.c_str(), e.what() );

            isok = false;
        }
        UnitProfileSprites::profiles_sprites.push_back(ups);
        style_index++;
        }

    }

    if ( ! isok )
    {
        LOGGER.warning("Can't load unit profile '%s'", unitName.c_str());
    }

    return isok;
} // function


//------------------------------------------------
// Units Profiles Mgmt



std::vector<unsigned short> UnitProfileInterface::su_speed_rate;
std::vector<unsigned short> UnitProfileInterface::su_speed_factor;
std::vector<unsigned short> UnitProfileInterface::su_speed;
std::vector<short> UnitProfileInterface::su_hit_points;
std::vector<short> UnitProfileInterface::su_damage_factor;
std::vector<unsigned short> UnitProfileInterface::su_reload_time;
std::vector<unsigned long> UnitProfileInterface::su_weapon_range;


void
UnitProfileInterface::clearProfiles()
{
    std::vector<UnitProfile *>::iterator i = profiles.begin();
    while ( i != profiles.end() )
    {
        delete *i;
        i++;
    }
    profiles.clear();
}

void
UnitProfileSprites::clearProfiles()
{
    std::vector<UnitProfileSprites *>::iterator i = profiles_sprites.begin();
    while ( i != profiles_sprites.end() )
    {
        delete *i;
        i++;
    }
    profiles_sprites.clear();
}

void UnitProfileInterface::doLoadUnitProfiles()
{
    std::vector<NPString> plist;
    NPString pl = *GameConfig::game_unit_profiles;

    string_to_params(pl, plist);

    unsigned int maxprofsnum = plist.size();

    for ( unsigned int n = 0; n < maxprofsnum; ++n )
    {
        addLocalProfile(plist[n]);
    }
}

void UnitProfileInterface::loadUnitProfiles( void )
{

    UnitProfileSprites::clearProfiles();
    clearProfiles();
    doLoadUnitProfiles();

    //superunit calculation

    sort(su_speed_rate.begin(),su_speed_rate.end());
    sort(su_speed_factor.begin(),su_speed_factor.end());
    sort(su_speed.begin(),su_speed.end());
    sort(su_hit_points.begin(),su_hit_points.end());
    sort(su_damage_factor.begin(),su_damage_factor.end());
    sort(su_reload_time.begin(),su_reload_time.end());
    sort(su_weapon_range.begin(),su_weapon_range.end());

    UnitProfileInterface::tsu_speed_rate = su_speed_rate[su_speed_rate.size()-1];
    UnitProfileInterface::tsu_speed_factor = su_speed_factor[su_speed_factor.size()-1];
    UnitProfileInterface::tsu_speed = su_speed[su_speed.size()-1];
    UnitProfileInterface::tsu_hit_points = su_hit_points[su_hit_points.size()-1];
    UnitProfileInterface::tsu_damage_factor = su_damage_factor[su_damage_factor.size()-1];
    UnitProfileInterface::tsu_reload_time = su_reload_time[0];
    UnitProfileInterface::tsu_weapon_range = su_weapon_range[su_weapon_range.size()-1];
/*
    LOGGER.info("best speed_rate = %hu", UnitProfileInterface::tsu_speed_rate);
    LOGGER.info("best speed_factor = %hu", UnitProfileInterface::tsu_speed_factor);
    LOGGER.info("best hit_points = %d", UnitProfileInterface::tsu_hit_points);
    LOGGER.info("best damage_factor = %d", UnitProfileInterface::tsu_damage_factor);
    LOGGER.info("best reload_time = %hu", UnitProfileInterface::tsu_reload_time);
    LOGGER.info("best weapon_range = %lu", UnitProfileInterface::tsu_weapon_range);
*/
    if ( profiles.size() == 0 )
    {
        LOGGER.warning("Error loading profiles provided by user, trying defaults");

        GameConfig::game_unit_profiles->assign(DEFAULT_UNIT_PROFILES);
        doLoadUnitProfiles();

        if ( profiles.size() == 0 )
        {
            LOGGER.warning("Didn't load ANY PROFILE!!! I should die.");
        }
    }
//    addLocalProfile("Manta");
//    addLocalProfile("Panther1");
//    addLocalProfile("Titan");
//    addLocalProfile("Stinger");
//    addLocalProfile("Bobcat");
//    addLocalProfile("Bear");
//    addLocalProfile("Archer");
//    addLocalProfile("Wolf");
//    addLocalProfile("Drake");
//    addLocalProfile("Spanzer");
}

bool UnitProfileInterface::addLocalProfile(const NPString& name)
{


    //while (unitstyle < GameConfig::getUnitStylesNum()) {
    UnitProfile * p = new UnitProfile();


    bool isok = read_vehicle_profile(name, p);
    if ( isok )
    {

        p->unit_type = profiles.size();
        profiles.push_back(p);

        //collecting superunit data
        //if (unitstyle == 1) {

            su_speed_rate.push_back(p->speed_rate);
            su_speed_factor.push_back(p->speed_factor);
            su_speed.push_back(p->speed_factor*p->speed_rate);
            su_hit_points.push_back(p->hit_points);
            su_damage_factor.push_back(p->attack_factor);
            su_reload_time.push_back(p->reload_time);
            su_weapon_range.push_back(p->attack_range);

        //}



    }


    //if (isok == false) {return false;}

    //}

    return true;
}


UnitProfileSprites * UnitProfileSprites::getUnitProfileSprites( unsigned short vector_index )
{
    if ( vector_index < profiles_sprites.size() )
        return profiles_sprites[vector_index];
    return 0;
}





UnitProfile * UnitProfileInterface::getUnitProfile( unsigned short unit_type )
{
    if ( unit_type < profiles.size() )
        return profiles[unit_type];
    return 0;
}

UnitProfile *
UnitProfileInterface::getProfileByName( const NPString& name )
{
    std::vector<UnitProfile *>::iterator i = profiles.begin();
    while ( i != profiles.end() )
    {
        if ( name.length() != (*i)->unitname.length() )
        {
            i++;
            continue; // continue if size !=
        }

        std::string::size_type s = 0;
        while ( s < name.length()
               && toupper(name[s]) == toupper((*i)->unitname[s]) )
        {
            s++;
        }

        if ( s == name.length() )
            return *i;

        i++;
    }
    return 0; // null pointer warning
}

int
UnitProfileInterface::fillProfileSyncMessage(NetMessage* message, int profile_id)
{
    UnitProfile *p = getUnitProfile(profile_id);
    ByteBufferWriter bb((unsigned char *)message, _MAX_NET_PACKET_SIZE);

    bb.writeInt8( _net_message_class_unit_profile );
    bb.writeInt8( _profile_msg_profile_desc );

    bb.writeString( p->unitname );
    bb.writeInt16(  p->unit_type );
    bb.writeInt16(  p->hit_points );
    bb.writeInt16(  p->attack_factor );
    bb.writeInt16(  p->cfg_attack_range );
    bb.writeInt16(  p->cfg_defend_range );
    bb.writeInt8(   p->speed_factor );
    bb.writeInt8(   p->speed_rate );
    bb.writeInt8(   p->reload_time );
    bb.writeInt16(  p->regen_time );
    bb.writeString( p->imagefile );
    bb.writeString( p->bodySprite_name );
    bb.writeString( p->bodyShadow_name );
    bb.writeString( p->turretSprite_name );
    bb.writeString( p->turretShadow_name );
    bb.writeString( p->soundSelected );
    bb.writeString( p->fireSound );
    bb.writeString( p->weaponType );
    bb.writeInt16(  p->boundBox );

    return bb.writedBytesCount();
}

int
UnitProfileInterface::fillProfileResetMessage(NetMessage* message)
{
    ByteBufferWriter bb((unsigned char *)message, _MAX_NET_PACKET_SIZE);

    bb.writeInt8( _net_message_class_unit_profile );
    bb.writeInt8( _profile_msg_reset );

    return bb.writedBytesCount();
}


UnitProfile *
UnitProfileInterface::loadProfileFromMessage(const NetMessage *message, size_t size)
{
    UnitProfile *p = new UnitProfile();
    ByteBufferReader br((const unsigned char *)message, size);

    Uint8 unused;
    br.readInt8( &unused );
    br.readInt8( &unused );

    br.readString( p->unitname );
    br.readInt16( &p->unit_type );
    br.readInt16( &p->hit_points );
    br.readInt16( &p->attack_factor );
    br.readInt16( &p->cfg_attack_range );
    br.readInt16( &p->cfg_defend_range );
    br.readInt8(  &p->speed_factor );
    br.readInt8(  &p->speed_rate );
    br.readInt8(  &p->reload_time );
    br.readInt16( &p->regen_time );
    br.readString( p->imagefile );
    br.readString( p->bodySprite_name );
    br.readString( p->bodyShadow_name );
    br.readString( p->turretSprite_name );
    br.readString( p->turretShadow_name );
    br.readString( p->soundSelected);
    br.readString( p->fireSound);
    br.readString (p->weaponType);
    br.readInt16( &p->boundBox );


    int style_index = 0;
    while ( style_index < GameManager::ststylesnum )
    {
        UnitProfileSprites * ups = new UnitProfileSprites();
        NPString spath = GameManager::stlist[style_index];
        NPString ustylepath = "units/pics/pak/" + spath + "/";

            ups->bodySprite.load(ustylepath+p->bodySprite_name);
            ups->bodyShadow.load(ustylepath+p->bodyShadow_name);
            ups->turretSprite.load(ustylepath+p->turretSprite_name);
            ups->turretShadow.load(ustylepath+p->turretShadow_name);

        UnitProfileSprites::profiles_sprites.push_back(ups);


        style_index++;
    }


    Uint32 i = p->cfg_attack_range * 32;
    p->attack_range = i*i;

    i = p->cfg_defend_range * 32;
    p->defend_range = i*i;

    return p;
}

void
UnitProfileInterface::processNetMessage(const NetMessage* net_message, size_t size)
{
    switch ( net_message->message_id )
    {
        case _profile_msg_profile_desc:
            handleProfileDescMessage(net_message, size);
            break;

        case _profile_msg_reset:
            UnitProfileSprites::clearProfiles();
            clearProfiles();
            break;

        default:
            LOGGER.warning("Unknown message id in UnitProfileMessage (%d)",
                           net_message->message_id);
    }
}

void
UnitProfileInterface::handleProfileDescMessage(const NetMessage *net_message, size_t size)
{
    UnitProfile* p = loadProfileFromMessage(net_message, size);
    if ( p )
    {
        p->unit_type = profiles.size();
        profiles.push_back(p);
    }
}

void
UnitProfileInterface::cleaning()
{
    //UnitProfileSprites::profiles_sprites.clear();
    //profiles.clear();
    su_speed_rate.clear();
    su_speed_factor.clear();
    su_speed.clear();
    su_hit_points.clear();
    su_damage_factor.clear();
    su_reload_time.clear();
    su_weapon_range.clear();
}






