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
#ifndef _NETWORKSTATE_HPP
#define _NETWORKSTATE_HPP

#include "Util/Timer.hpp"

enum{ _network_state_server, _network_state_client , _network_state_bot};

class NetworkState
{
public:
    static unsigned short status;

    static long  bytes_sent;
    static long  bytes_sent_last_sec;
    static float bytes_sent_per_sec;

    static long  bytes_received;
    static long  bytes_received_last_sec;
    static float bytes_received_per_sec;

    static long packets_sent;
    static long packets_received;

    static float packets_sent_per_sec;
    static long  packets_sent_last_sec;
    static long  packets_sent_time;
    static TimeStamp packets_sent_interval;

    static float packets_received_per_sec;
    static long  packets_received_last_sec;
    static long  packets_received_time;
    static TimeStamp packets_received_interval;

    static long opcodes_sent;
    static long opcodes_received;

    static float opcodes_sent_per_sec;
    static long  opcodes_sent_last_sec;
    static Timer opcodes_sent_timer;
    static float opcodes_received_per_sec;
    static long  opcodes_received_last_sec;
    static Timer opcodes_received_timer;

    static TimeStamp ping_time_stamp;
    static Timer     ping_timer;
    static float     ping_time;

    static inline void setNetworkStatus( unsigned short status_flags )
    {
        status = status_flags;
    }

    static inline unsigned short getNetworkStatus()
    {
        return( status );
    }

    static inline void incPacketsSent( long packet_size )
    {
        bytes_sent += packet_size;
        packets_sent++;
    }

    static inline void incPacketsReceived( long packet_size )
    {
        bytes_received += packet_size;
        packets_received++;
    }

    static inline void incOpcodesSent()
    {
        opcodes_sent++;
        if ( opcodes_sent_timer.count() ) {
            opcodes_sent_per_sec = (float) opcodes_sent - (float) opcodes_sent_last_sec;
            opcodes_sent_last_sec = opcodes_sent;
        }
    }

    static inline void incOpcodesReceived()
    {
        opcodes_received++;
        if ( opcodes_received_timer.count() ) {
            opcodes_received_per_sec = (float) opcodes_received - (float) opcodes_received_last_sec;
            opcodes_received_last_sec = opcodes_received;
        }
    }

    static void resetNetworkStats();

    static void updateNetworkStats();

    static void logNetworkStats();
};

#endif // ** _NETWORKSTATE_HPP
