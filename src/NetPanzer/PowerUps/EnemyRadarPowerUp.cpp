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

#include "EnemyRadarPowerUp.hpp"

#include <stdlib.h>
#include "Units/UnitTypes.hpp"
#include "Units/UnitInterface.hpp"
#include "Interfaces/PlayerInterface.hpp"
#include "Interfaces/MapInterface.hpp"
#include "Interfaces/ConsoleInterface.hpp"

#include "Classes/Network/NetworkServer.hpp"
#include "Classes/Network/NetworkState.hpp"
#include "Classes/Network/UnitNetMessage.hpp"
#include "Classes/Network/PowerUpNetMessage.hpp"

#include "System/Sound.hpp"
#include "Util/NTimer.hpp"

static NTimer radarTimer(180000);
static bool radarActive = true; // was false, now always on

void
ActivateRadar()
{
    radarActive = true;
    radarTimer.reset();
    ConsoleInterface::postMessage(Color::unitAqua, false, 0, "YOU GOT AN ENEMY RADAR POWERUP" );
}

bool
EnemyRadarPowerUp::isRadarActive()
{
    if ( radarActive && radarTimer.isTimeOut() )
    {
        radarActive = true; // was false, now always on
    }
    return radarActive;
}

EnemyRadarPowerUp::EnemyRadarPowerUp(iXY map_loc, int type)
        : PowerUp( map_loc, type )
{
}

void
EnemyRadarPowerUp::onHit(UnitID unit_id)
{
    sound->playPowerUpSound();

    UnitBase* unit = UnitInterface::getUnit(unit_id);

    if(unit->player == PlayerInterface::getLocalPlayer())
    {
        ActivateRadar();
    }

    PowerUpHitMesg hit_mesg;
    hit_mesg.set( ID, unit->player->getID());
    SERVER->broadcastMessage( &hit_mesg, sizeof( PowerUpHitMesg ));

    life_cycle_state = _power_up_lifecycle_state_inactive;
}

void
EnemyRadarPowerUp::onHitMessage( PowerUpHitMesg *message  )
{
    sound->playPowerUpSound();

    if( PlayerInterface::getLocalPlayerIndex() == message->getPlayerID() )
    {
        ActivateRadar();
    }

    life_cycle_state = _power_up_lifecycle_state_inactive;
}
