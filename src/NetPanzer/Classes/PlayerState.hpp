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
#ifndef _PLAYERSTATE_HPP
#define _PLAYERSTATE_HPP

#include <string>
#include <string.h>
#include "Core/CoreTypes.hpp"
#include "Classes/PlayerUnitConfig.hpp"
#include "2D/Palette.hpp"
#include "Util/NTimer.hpp"

enum { _player_state_free,
       _player_state_allocated,
       _player_state_connecting,
       _player_state_selecting_flag,
       _player_state_active,
       _player_state_kicked
     };

#ifdef MSVC
#pragma pack(1)
#endif

class NetworkPlayerState
{
public:
    PlayerID getPlayerIndex() const;

private:
    NetworkPlayerState() 
    {}
    friend class PlayerState;    
    friend class PlayerStateSync;
    
    PlayerID id;
    char name[64];
    Uint8 status;
    Sint16 kills;
    Sint16 kill_points;
    Sint16 losses;
    Sint16 loss_points;
    Sint16 total;
    Sint16 objectives_held;
}__attribute__((packed));

#ifdef MSVC
#pragma pack()
#endif

class PlayerState
{
private:
    PlayerID id;
    std::string name;
    unsigned char status;
    short kills;
    short kill_points;
    short losses;
    short loss_points;
    short total;
    short objectives_held;
    bool stats_locked;
    bool admin_flag;
    NTimer autokick;

public:
    PlayerUnitConfig unit_config;

    PlayerState();
    PlayerState(const PlayerState& other);

    void operator= (const PlayerState& other);

    void setName(const std::string& newname);
    const std::string& getName() const;

    PlayerID getID() const { return id; }

    void resetAutokick();
    bool checkAutokick();
    void resetStats(bool keepAdmin);
    void lockStats();
    void unlockStats();
    short getKills() const;
    short getLosses() const;
    void incKills(UnitType unit_type);
    void decKills(UnitType unit_type);
    void incLosses(UnitType unit_type);
    void decLosses(UnitType unit_type);
    void incObjectivesHeld();
    void decObjectivesHeld();
    short getObjectivesHeld() const;
    void setObjectivesHeld( short objectives );
    void setID( PlayerID id );
    FlagID getFlag() const;
    short getTotal() const;
    NetworkPlayerState getNetworkPlayerState() const;
    void getNetworkPlayerState(NetworkPlayerState& state) const;
    void setFromNetworkPlayerState(const NetworkPlayerState* state);
    Uint8 getColor() const;

    void setAdmin(bool flag) { admin_flag = flag; }
    bool isAdmin()     const { return admin_flag; }

    void setStateFree()       { status = _player_state_free; }
    void setStateAllocated()  { status = _player_state_allocated; }
    void setStateConnecting() { status = _player_state_connecting; }
    void setStateSelectingFlag() { status = _player_state_selecting_flag; }
    void setStateActive()     { status = _player_state_active; }
    void setStateKicked()     { status = _player_state_kicked; }

    bool isFree()       const { return status == _player_state_free; }
    bool isAllocated()  const { return status == _player_state_allocated; }
    bool isConnecting() const { return status == _player_state_connecting; }
    bool isSelectingFlag() const { return status == _player_state_selecting_flag; }
    bool isActive()     const { return status == _player_state_active || isSelectingFlag(); }
    bool isPlaying()    const { return status == _player_state_active; }
    bool isKicked()     const { return status == _player_state_kicked; }
};

#endif // ** _PLAYERSTATE_HPP
