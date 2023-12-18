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
#ifndef _GAME_CONTROL_NET_MESSAGE_HPP
#define _GAME_CONTROL_NET_MESSAGE_HPP

#include <stdio.h>
#include <string.h>

#include "NetMessage.hpp"

enum { _net_message_id_game_control_cycle_map,
       _net_message_id_game_control_cycle_respawn_ack
     };

#ifdef MSVC
#pragma pack(1)
#endif

class GameControlCycleMap : public NetMessage
{
public:
    char map_name[128];

    void set(const char* newmap_name)
    {
        snprintf(map_name, sizeof(map_name), "%s", newmap_name);
    }

    GameControlCycleMap()
    {
        message_class = _net_message_class_game_control;
        message_id = _net_message_id_game_control_cycle_map;
        memset(map_name, 0, sizeof(map_name));
    }

}
__attribute__((packed));

class GameControlCycleRespawnAck : public NetMessage
{
public:
    GameControlCycleRespawnAck()
    {
        message_class = _net_message_class_game_control;
        message_id = _net_message_id_game_control_cycle_respawn_ack;
    }
}
__attribute__((packed));

#ifdef MSVC
#pragma pack()
#endif

#endif // ** _GAME_CONTROL_NET_MESSAGE_HPP
