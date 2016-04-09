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
#ifndef _NETPACKETQUEUES_HPP
#define _NETPACKETQUEUES_HPP

#include <string.h>
#include "Classes/Network/NetPacket.hpp"
#include "ArrayUtil/QueueTemplate.hpp"
#include "ArrayUtil/ArrayTemplate.hpp"
#include "Util/Timer.hpp"

class NetPacketQueue : public QueueTemplate< NetPacket >
{
public:
    void add(NetPacket *object, unsigned long index)
    {
        memcpy(&array[index], object, sizeof(NetPacket));
    }

    void dequeue(NetPacket *object)
    {
        assert( front != rear );
        front = ( front + 1 ) % size;
        memcpy(object, &array[ front ], sizeof(NetPacket));
    }

    void enqueue(NetPacket &object)
    {
        add(&object, (rear + 1) % size);
        rear = (rear + 1) % size;
    }
};

#endif // ** _NETPACKETQUEUES_HPP
