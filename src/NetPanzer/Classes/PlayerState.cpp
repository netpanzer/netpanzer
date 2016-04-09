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


#include "Util/Endian.hpp"
#include "Util/Log.hpp"
#include "Classes/PlayerState.hpp"
#include "Interfaces/PlayerInterface.hpp"
#include "Interfaces/GameConfig.hpp"
#include "Resources/ResourceManager.hpp"
#include <sstream>

PlayerID NetworkPlayerState::getPlayerIndex() const
{
    return id;
}

//If you modify this array, also modify the constant above
Uint8 *playerColorArray[] = {
	&Color::red,
	&Color::green,
	&Color::brown,
	&Color::yellow,
	&Color::cyan,
	&Color::gray,
	&Color::magenta,
	&Color::pink,
	&Color::unitAqua,
	&Color::unitYellow,
	&Color::unitRed,
	&Color::unitBlue,
	&Color::unitDarkBlue,
	&Color::unitLightGreen,
	&Color::unitGreen,
	&Color::unitBlueGray,
	&Color::unitDarkRed,
	//&Color::unitBlack, black removed from list
	&Color::unitDarkGreen,
	&Color::unitWhite,
	&Color::unitLightOrange,
	&Color::unitOrange,
	&Color::unitGray,
	&Color::unitDarkGray,
	&Color::tan,
	&Color::chartreuse,
	&Color::cobaltGreen,
	&Color::emeraldGreen,
	&Color::forestGreen,
	&Color::darkOliveGreen,
	&Color::terreVerte,
	&Color::darkBlue,
	&Color::darkBrown,
	&Color::darkCyan,
	&Color::darkGray,
	&Color::darkGreen,
	&Color::darkMagenta,
	&Color::darkOrange,
	&Color::darkPink,
	&Color::darkRed,
	&Color::darkYellow,
	&Color::lightBlue,
	&Color::lightBrown,
	&Color::lightCyan,
	&Color::lightGray,
	&Color::lightGreen,
	&Color::lightMagenta,
	&Color::lightOrange,
	&Color::lightPink,
	&Color::lightRed,
	&Color::lightYellow,
	&Color::gray32,
	&Color::gray64,
	&Color::gray96,
	&Color::gray128,
	&Color::gray160,
	&Color::gray192,
	&Color::gray224
};

static const size_t playerColorCount 
    = sizeof(playerColorArray) / sizeof(Uint8*);

Uint8
PlayerState::getColor() const
{
    assert(id < playerColorCount);
    return ( *playerColorArray[ id ] );
}

PlayerState::PlayerState()
    : status(0), kills(0), kill_points(0), losses(0),
      loss_points(0), total(0), objectives_held(0), stats_locked(false),
      admin_flag(false)
{
    autokick.reset();
}

PlayerState::PlayerState(const PlayerState& other)
    :  id(other.id), name(other.name),
      status(other.status), kills(other.kills), kill_points(other.kill_points),
      losses(other.losses), loss_points(other.loss_points),
      total(other.total), objectives_held(other.objectives_held),
      stats_locked(other.stats_locked), unit_config(other.unit_config)
{
    // nothing
}

void PlayerState::operator= (const PlayerState& other)
{
    id = other.id;
    name = other.name;
    status = other.status;
    kills = other.kills;
    kill_points = other.kill_points;
    losses = other.losses;
    loss_points = other.loss_points;
    total = other.total;
    objectives_held = other.objectives_held;
    stats_locked = other.stats_locked;
    unit_config = other.unit_config;
    admin_flag = other.admin_flag;
    autokick.reset();
}

void PlayerState::setName(const std::string& newname)
{
    if ( newname.length() > 63 )
    {
        name = newname.substr(0,63);
    }
    else
    {
        name = newname;
    }

    int namenum=1;
    bool recheck;
    do
    {
        recheck = false;
        PlayerID p;
        for ( p = 0; p<PlayerInterface::getMaxPlayers(); ++p )
        {
            if ( p == id )
                continue;
                
            PlayerState *ps=PlayerInterface::getPlayer(p);
            if ( (ps->isConnecting() || ps->isActive())
               && ps->name == name
               )
            {
                std::stringstream ssnamenum;
                ssnamenum << "(" << namenum++ << ")";
                std::string strnum=ssnamenum.str();
                
                std::string::size_type newlen = newname.length();
                if ( newlen+strnum.length() > 63 )
                {
                    newlen -= strnum.length() - (63 - newlen);  
                }
                
                name = newname.substr(0,newlen)+strnum;
                recheck=true;
                break;
            }
        }
    } while (recheck);
}

void PlayerState::resetStats(bool keepAdmin)
{
    kills  = 0;
    kill_points = 0;
    losses = 0;
    loss_points = 0;
    total = 0;
    objectives_held = 0;
    stats_locked = false;
    if ( ! keepAdmin ) admin_flag = false;
}

void PlayerState::resetAutokick()
{
    autokick.reset();
}

bool PlayerState::checkAutokick()
{
    return GameConfig::game_autokicktime > 0 && autokick.checkWithTimeOut(GameConfig::game_autokicktime * 60000);
}

const std::string& PlayerState::getName() const
{
    return name;
}

void PlayerState::lockStats()
{
    stats_locked = true;
}

void PlayerState::unlockStats()
{
    stats_locked = false;
}

short PlayerState::getKills() const
{
    return kills;
}

short PlayerState::getLosses() const
{
    return losses;
}

void PlayerState::incKills(UnitType unit_type)
{
    if ( stats_locked == true )
        return;
    kills++;
    //kill_points += 2 * unit_config.getUnitPointValue(unit_type);
}

void PlayerState::decKills(UnitType unit_type)
{
    (void) unit_type;
    if ( stats_locked == true )
        return;
    kills--;
}

void PlayerState::incLosses(UnitType unit_type)
{
    if ( stats_locked == true )
        return;
    losses++;
    //loss_points += unit_config.getUnitPointValue(unit_type);
}

void PlayerState::decLosses(UnitType unit_type)
{
    (void) unit_type;
    if ( stats_locked == true )
        return;
    losses--;
}

void PlayerState::incObjectivesHeld()
{
    if ( stats_locked == true )
        return;
    objectives_held++;
}

void PlayerState::decObjectivesHeld()
{
    if ( stats_locked == true )
        return;
    objectives_held--;
}

short PlayerState::getObjectivesHeld() const
{
    return objectives_held;
}

void PlayerState::setObjectivesHeld( short objectives )
{
    if ( stats_locked == true )
        return;
    objectives_held = objectives;
}

void PlayerState::setID( PlayerID id )
{
    this->id = id;
}

FlagID PlayerState::getFlag() const
{
    return id;
}

// this "getTotal" is used to calculate the frags.
short PlayerState::getTotal() const
{
    return kills - (losses/2);
}

NetworkPlayerState PlayerState::getNetworkPlayerState() const
{
    NetworkPlayerState state;

    state.id = id;
    memset(state.name, 0, sizeof(state.name));
    strncpy(state.name, name.c_str(), sizeof(state.name)-1);
    state.status = status;
    state.kills = htol16(kills);
    state.kill_points = htol16(kill_points);
    state.losses = htol16(losses);
    state.loss_points = htol16(loss_points);
    state.total = htol16(total);
    state.objectives_held = htol16(objectives_held);
    
    return state;
}

void PlayerState::getNetworkPlayerState(NetworkPlayerState& state) const
{
    state.id = id;
    memset(state.name, 0, sizeof(state.name));
    strncpy(state.name, name.c_str(), sizeof(state.name)-1);
    state.status = status;
    state.kills = htol16(kills);
    state.kill_points = htol16(kill_points);
    state.losses = htol16(losses);
    state.loss_points = htol16(loss_points);
    state.total = htol16(total);
    state.objectives_held = htol16(objectives_held);
}

void PlayerState::setFromNetworkPlayerState(const NetworkPlayerState* state)
{
    char tmp[64];
    memcpy(tmp, state->name, 64); 
    tmp[63] = 0;
    name = tmp;
    id = state->id;
    status = state->status;
    kills = ltoh16(state->kills);
    kill_points = ltoh16(state->kill_points);
    losses = ltoh16(state->losses);
    loss_points = ltoh16(state->loss_points);
    total = ltoh16(state->total);
    objectives_held = ltoh16(state->objectives_held);
}
