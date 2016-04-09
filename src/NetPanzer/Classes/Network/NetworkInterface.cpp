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


#include <assert.h>
#include "Util/Log.hpp"
#include "NetworkInterface.hpp"

NetPacketQueue NetworkInterface::receive_queue;

void EnqueueIncomingPacket( const void *data, Uint16 size,
                            PlayerID fromPlayer, ClientSocket *fromClient)
{
    static NetPacket TEMP_PACKET;

    TEMP_PACKET.fromPlayer = fromPlayer;
    TEMP_PACKET.fromClient = fromClient;
    TEMP_PACKET.size = size;
    assert(size <= _MAX_NET_PACKET_SIZE);

    memcpy(TEMP_PACKET.data, data, size);
    NetworkInterface::receive_queue.enqueue( TEMP_PACKET );
}


NetworkInterface::NetworkInterface()
{
    receive_queue.initialize( 200 );
}

NetworkInterface::~NetworkInterface()
{
    receive_queue.deallocate();
}

