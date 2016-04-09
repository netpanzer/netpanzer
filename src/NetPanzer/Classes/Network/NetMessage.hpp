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
#ifndef __NETMESSAGE_HPP__
#define __NETMESSAGE_HPP__

#include "Util/Endian.hpp"
#include "Core/CoreTypes.hpp"

enum { _net_message_class_multi = 0,
       _net_message_class_system,
       _net_message_class_connect,
       _net_message_class_player,
       _net_message_class_unit,
       _net_message_class_unit_profile,
       _net_message_class_terminal,
       _net_message_class_objective,
       _net_message_class_game_control,
       _net_message_class_powerup,
       _net_message_class_chat
     };

#ifdef MSVC
#pragma pack(1)
#endif

class NetMessage
{
public:
    Uint8  message_class;
    Uint8  message_id;
} __attribute__((packed));

#ifdef MSVC
#pragma pack()
#endif

#endif

