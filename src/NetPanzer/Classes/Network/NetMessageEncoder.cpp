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

#include "Classes/Network/NetMessageEncoder.hpp"

#include <string.h>

#include "Classes/Network/NetworkState.hpp"
#include "Classes/Network/NetworkServer.hpp"
#include "NetworkClient.hpp"

NetMessageEncoder::NetMessageEncoder()
{
    resetEncoder();
}

NetMessageEncoder::~NetMessageEncoder()
{
}

void NetMessageEncoder::resetEncoder()
{
    encode_message.message_class = _net_message_class_multi ;
    encode_message.message_id = 0;
    memset(encode_message.data, 0, sizeof(encode_message.data));
    offset = 0;
}

bool NetMessageEncoder::encodeMessage(NetMessage *message, size_t size)
{
    if (offset+size >= _MULTI_PACKET_LIMIT )
    {
        return false;
    }

    Uint16* mlen = (Uint16*)(encode_message.data + offset);
    *mlen = htol16(size);

    memcpy(mlen+1, message, size);

    offset += size + 2;
    return true;
}
