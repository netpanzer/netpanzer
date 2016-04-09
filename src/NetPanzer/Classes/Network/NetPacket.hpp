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
#ifndef _NETPACKET_HPP
#define _NETPACKET_HPP

#include <string.h>

#include "Util/Endian.hpp"
#include "Core/CoreTypes.hpp"
#include "NetMessage.hpp"

static const Uint16 _MAX_NET_PACKET_SIZE=512;
class ClientSocket;

#ifdef MSVC
#pragma pack(1)
#endif
class NetPacket
{
public:
    PlayerID fromPlayer;
    ClientSocket *fromClient;
    int size;

    Uint8  data[ _MAX_NET_PACKET_SIZE ];

    const NetMessage* getNetMessage() const
    {
        return (const NetMessage*) data;
    }

    size_t getSize() const
    {
        return sizeof(ClientSocket *) 
                + sizeof(PlayerID)
                + sizeof(int)
                + size;
    }
}
__attribute__((packed));

#define _MULTI_PACKET_LIMIT (_MAX_NET_PACKET_SIZE - sizeof(NetMessage))

class MultiMessage : public NetMessage
{
public:
    Uint8  data[ _MULTI_PACKET_LIMIT ];
} __attribute__((packed));

#ifdef MSVC
#pragma pack()
#endif

#endif

