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

#include "BonusUnitPowerUp.hpp"

#include <stdlib.h>
#include "Units/UnitTypes.hpp"
#include "Units/UnitInterface.hpp"
#include "Interfaces/PlayerInterface.hpp"
#include "Interfaces/MapInterface.hpp"
#include "Interfaces/ConsoleInterface.hpp"
#include "Units/UnitProfileInterface.hpp"

#include "Classes/Network/NetworkServer.hpp"
#include "Classes/Network/NetworkState.hpp"
#include "Classes/Network/UnitNetMessage.hpp"
#include "Classes/Network/PowerUpNetMessage.hpp"

#include "System/Sound.hpp"

BonusUnitPowerUp::BonusUnitPowerUp(iXY map_loc, int type)
        : PowerUp( map_loc, type )
{
    bonus_unit_type = rand() % UnitProfileInterface::getNumUnitTypes();
}


void BonusUnitPowerUp::onHit( UnitID unit_id )
{
    PlacementMatrix placement_matrix;

    sound->playPowerUpSound();

    PlayerID own_player = UnitInterface::getUnit(unit_id)->player->getID();

    placement_matrix.reset( map_loc );


    for( int i = 0; i < 9; i++ )
    {
        UnitBase *new_unit;
        iXY spawn_loc;

        placement_matrix.getNextEmptyLoc( &spawn_loc );
/*
    PlayerState *player_state2;
    player_state2 = PlayerInterface::getPlayer(own_player);
    unsigned char ustyle2 = player_state2->getPlayerStyle();
*/
        new_unit = UnitInterface::createUnit(bonus_unit_type,
                                             spawn_loc,
                                             own_player );

        if ( new_unit != 0 )
        {
            UnitRemoteCreate create_mesg(new_unit->player->getID(),
                    new_unit->id, spawn_loc.x, spawn_loc.y, bonus_unit_type);
            SERVER->broadcastMessage( &create_mesg, sizeof( UnitRemoteCreate ));
        }

    }

    PowerUpHitMesg hit_mesg;
    hit_mesg.set( ID, own_player );
    SERVER->broadcastMessage( &hit_mesg, sizeof( PowerUpHitMesg ));

    life_cycle_state = _power_up_lifecycle_state_inactive;

    if( PlayerInterface::isLocalPlayer(own_player) )
    {
        ConsoleInterface::postMessage(Color::unitAqua, false, 0, "YOU GOT A BONUS UNITS POWERUP" );
    }
}

void BonusUnitPowerUp::onHitMessage( PowerUpHitMesg *message  )
{
    sound->playPowerUpSound();
    life_cycle_state = _power_up_lifecycle_state_inactive;

    if( PlayerInterface::getLocalPlayerIndex() == message->getPlayerID() )
    {
        ConsoleInterface::postMessage( Color::unitAqua, false, 0, "YOU GOT A BONUS UNITS POWERUP" );
    }
}
