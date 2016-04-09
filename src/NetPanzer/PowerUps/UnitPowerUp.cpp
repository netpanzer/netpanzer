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

#include "UnitPowerUp.hpp"

#include <stdlib.h>
#include "Units/UnitTypes.hpp"
#include "Units/UnitInterface.hpp"
#include "Classes/UnitMessageTypes.hpp"
#include "Interfaces/PlayerInterface.hpp"
#include "Interfaces/MapInterface.hpp"
#include "Interfaces/ConsoleInterface.hpp"

#include "Classes/Network/NetworkServer.hpp"
#include "Classes/Network/NetworkState.hpp"
#include "Classes/Network/UnitNetMessage.hpp"
#include "Classes/Network/PowerUpNetMessage.hpp"

#include "System/Sound.hpp"
#include "Classes/Network/NetworkServer.hpp"

enum { _unit_powerup_hitpoints,
       _unit_powerup_range,
       _unit_powerup_firepower,
       _unit_powerup_speed,
       _unit_powerup_repair,
       _unit_powerup_reload,
       _unit_powerup_destruct,
       _unit_powerup_enum_count
     };

UnitPowerUp::UnitPowerUp(iXY map_loc, int type)
    : PowerUp( map_loc, type )
{
    unit_powerup_type = rand() % _unit_powerup_enum_count;
}

void UnitPowerUp::powerUpHitPoints( UnitState *unit_state)
{
    int percent = (int)(rand() % 8)+1;
    int new_hitpoints = unit_state->max_hit_points+
                        (int)((double)unit_state->max_hit_points * ((double)percent/(double)100));
    unit_state->hit_points = new_hitpoints;
    unit_state->max_hit_points = new_hitpoints;
}

void UnitPowerUp::powerUpRange( UnitState *unit_state)
{
    int percent = (int)(rand() % 8)+1;
    int new_weapon_range = unit_state->weapon_range+
                           (int)((double)unit_state->weapon_range * ((double)percent/(double)100));
    unit_state->weapon_range = new_weapon_range;
}

void UnitPowerUp::powerUpFirePower( UnitState *unit_state)
{
    int percent = (int)(rand() % 8)+1;
    int new_damage_factor = unit_state->damage_factor+
                            (int)((double)unit_state->damage_factor * ((double)percent/(double)100));
    unit_state->damage_factor = new_damage_factor;
}

void UnitPowerUp::powerUpSpeed( UnitState *unit_state)
{
    int percent = (int)(rand() % 10)+1;
    int new_speed_factor = unit_state->speed_factor+
                           (int)((double)unit_state->speed_factor * ((double)percent/(double)100));
    unit_state->speed_factor = new_speed_factor;
}

void UnitPowerUp::powerUpRepair( UnitState *unit_state)
{
    unit_state->hit_points = unit_state->max_hit_points;
}

void UnitPowerUp::powerUpReload( UnitState *unit_state)
{
    int percent = (int)(rand() % 6)+1;
    int new_reload_time = unit_state->reload_time-
                          (int)((double)unit_state->reload_time * ((double)percent/(double)100));
    unit_state->reload_time = new_reload_time;
    if (unit_state->reload_time < 20 ) unit_state->reload_time = 20;

}

void UnitPowerUp::powerUpDestruct( UnitID unit_id )
{
    UMesgSelfDestruct self_destruct;
    self_destruct.setHeader( unit_id, _umesg_flag_unique );
    UnitInterface::sendMessage( &self_destruct );
}

static const char * powerupTypeToString( int type )
{
    switch( type )
    {
    case _unit_powerup_hitpoints:
        return( "UNIT HITPOINTS" );

    case _unit_powerup_range:
        return( "UNIT WEAPON RANGE" );

    case _unit_powerup_firepower:
        return( "UNIT FIREPOWER" );

    case _unit_powerup_speed:
        return( "UNIT SPEED" );

    case _unit_powerup_repair:
        return( "UNIT REPAIR" );

    case _unit_powerup_reload:
        return( "UNIT RELOAD TIME" );

    case _unit_powerup_destruct:
        return( "UNIT DESTRUCT" );
    }

    return("");
}

void UnitPowerUp::onHit( UnitID unit_id )
{
    sound->playPowerUpSound();

    UnitBase* unit = UnitInterface::getUnit( unit_id );

    switch( unit_powerup_type )
    {
    case _unit_powerup_hitpoints:
        powerUpHitPoints( &(unit->unit_state) );
        break;

    case _unit_powerup_range:
        powerUpRange( &(unit->unit_state) );
        break;

    case _unit_powerup_firepower:
        powerUpFirePower( &(unit->unit_state) );
        break;

    case _unit_powerup_speed:
        powerUpSpeed( &(unit->unit_state) );
        break;

    case _unit_powerup_repair:
        powerUpRepair( &(unit->unit_state) );
        break;

    case _unit_powerup_reload:
        powerUpReload( &(unit->unit_state) );
        break;

    case _unit_powerup_destruct:
        powerUpDestruct( unit_id );
        break;
    }

    PowerUpHitMesg hit_mesg;
    hit_mesg.set(ID, unit->player->getID(), unit_powerup_type);
    SERVER->broadcastMessage(&hit_mesg, sizeof(PowerUpHitMesg));

    life_cycle_state = _power_up_lifecycle_state_inactive;

    if(unit->player == PlayerInterface::getLocalPlayer())
    {
        ConsoleInterface::postMessage(Color::unitAqua, false, 0, "YOU GOT A %s POWERUP", 
                                      powerupTypeToString( unit_powerup_type ) );
    }
}

void UnitPowerUp::onHitMessage( PowerUpHitMesg *message  )
{
    sound->playPowerUpSound();
    life_cycle_state = _power_up_lifecycle_state_inactive;

    if( PlayerInterface::getLocalPlayerIndex() == message->getPlayerID() )
    {
        ConsoleInterface::postMessage(Color::unitAqua, false, 0, "YOU GOT A %s POWERUP",
                                      powerupTypeToString( message->getUnitPowerupType() ) );
    }
}
