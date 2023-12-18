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

// for trim function
#include <iostream>
#include <string>

std::string trim(const std::string& str)
{
    size_t first = str.find_first_not_of(' ');
    if (std::string::npos == first)
    {
        return str;
    }
    size_t last = str.find_last_not_of(' ');
    return str.substr(first, (last - first + 1));
}
//










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
      loss_points(0), total(0), objectives_held(0), units_style(0),
      stats_locked(false), admin_flag(false),
      cheating(0), cheating_hits(0), client_type(0), lastenckey(0),
      up_last_ping(0), up_avg_ping(0),
      down_last_ping(0), down_avg_ping(0), temp_time(0)
{
    autokick.reset();
}

PlayerState::PlayerState(const PlayerState& other)
    :  id(other.id), name(other.name),
      status(other.status), kills(other.kills), kill_points(other.kill_points),
      losses(other.losses), loss_points(other.loss_points),
      total(other.total), objectives_held(other.objectives_held), units_style(other.units_style),
      stats_locked(other.stats_locked),
      cheating(other.cheating), cheating_hits(other.cheating_hits), client_type(other.client_type), lastenckey(other.lastenckey),
      up_last_ping(other.up_last_ping), up_avg_ping(other.up_avg_ping),
      down_last_ping(other.down_last_ping), down_avg_ping(other.down_avg_ping), temp_time(other.temp_time),
      unit_config(other.unit_config)
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
    units_style = other.units_style;
    stats_locked = other.stats_locked;
    unit_config = other.unit_config;
    admin_flag = other.admin_flag;
    autokick.reset();
    muted = other.muted;
    cheating = other.cheating;
    cheating_hits = other.cheating_hits;
    client_type = other.client_type;
    lastenckey = other.lastenckey;
    up_last_ping = other.up_last_ping;
    up_avg_ping = other.up_avg_ping;
    down_last_ping = other.down_last_ping;
    down_avg_ping = other.down_avg_ping;
    temp_time = other.temp_time;
}



void PlayerState::setName(const std::string& newname)
{
    if ( newname.length() > 20 )
    {
        name = newname.substr(0,20); //was 63
    }
    else
    {
        name = newname;
    }

    nameb = name;

    //
    // strip & trim

    // replacing non printable chars

    std::stringstream nodbname;

    for ( int ac=0; ac < (int)name.length(); ac++ ) {

        if ((int)name[ac] == 32) {
        nodbname << name[ac];
        }

        if (!( (int)name[ac] == 34 || (int)name[ac] == 35 || (int)name[ac] == 37 || (int)name[ac] == 39
        || (int)name[ac] == 44 || (int)name[ac] == 94 || (int)name[ac] == 96 || (int)name[ac] == 126
        || (int)name[ac] == 47 || (int)name[ac] == 92 || (int)name[ac] == 124 || (int)name[ac] < 32
        || (int)name[ac] > 126 )) {
        nodbname << name[ac];
        }

     }

    name = nodbname.str();

    // trimming
    name = trim(name);

    // stripping multiple spaces
    std::stringstream nodbname2;

    for ( int ac2=0; ac2 < (int)name.length(); ac2++ ) {

        if (name[ac2] != 32) {
            nodbname2 << name[ac2];
        }

        if (ac2 > 0 && name[ac2] == 32 && name[ac2-1] != 32) {
            nodbname2 << name[ac2];
        }

    }

    name = nodbname2.str();

    // blank string?
    if ((int)name.length()==0) {
        std::stringstream default_player;
        default_player << "Player" << (rand()%1000);
        name = default_player.str();
    }

    nameb = name;

    // end

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

                std::string::size_type newlen = nameb.length();
                if ( newlen+strnum.length() > 20 )
                {
                    newlen -= strnum.length() - (20 - newlen);
                }

                name = nameb.substr(0,newlen)+strnum;
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
    muted = false;
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

void PlayerState::setPlayerStyle(unsigned char nustyle)
{
    units_style = nustyle;
}

unsigned char PlayerState::getPlayerStyle()
{
    return units_style;
}

void PlayerState::setMute(bool mute)
{
    muted = mute;
}

bool PlayerState::getMute() const
{
    return muted;
}

void PlayerState::setCheat(unsigned char cheat_type)
{
    cheating = cheat_type;
}

unsigned char PlayerState::getCheat() const
{
    return cheating;
}

void PlayerState::setCheatHits(unsigned char cheat_hits)
{
    cheating_hits = cheat_hits;
}

unsigned char PlayerState::getCheatHits() const
{
    return cheating_hits;
}

void PlayerState::setClientType(unsigned char cl_type)
{
    client_type = cl_type;
}

unsigned char PlayerState::getClientType() const
{
    return client_type;
}

void PlayerState::setLastEncKey(unsigned char enc_key)
{
    lastenckey = enc_key;
}

unsigned char PlayerState::getLastEncKey() const
{
    return lastenckey;
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
    state.units_style = units_style;
    state.client_type = client_type;


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
    state.units_style = units_style;
    state.client_type = client_type;
}

void PlayerState::setFromNetworkPlayerState(const NetworkPlayerState* state)
{
    char tmp[21];
    memcpy(tmp, state->name, 21);
    tmp[20] = 0;
    name = tmp;
    id = state->id;
    status = state->status;
    kills = ltoh16(state->kills);
    kill_points = ltoh16(state->kill_points);
    losses = ltoh16(state->losses);
    loss_points = ltoh16(state->loss_points);
    total = ltoh16(state->total);
    objectives_held = ltoh16(state->objectives_held);
    units_style = state->units_style;
    client_type = state->client_type;
}
