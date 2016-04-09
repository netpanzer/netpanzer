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


#include <stdio.h>
#include "Classes/Network/NetworkState.hpp"

unsigned short NetworkState::status;

long NetworkState::bytes_sent;
long  NetworkState::bytes_sent_last_sec;
float NetworkState::bytes_sent_per_sec;

long NetworkState::bytes_received;
long  NetworkState::bytes_received_last_sec;
float NetworkState::bytes_received_per_sec;

long NetworkState::packets_sent;
long NetworkState::packets_received;

float NetworkState::packets_sent_per_sec;
long  NetworkState::packets_sent_last_sec;
long  NetworkState::packets_sent_time;
TimeStamp NetworkState::packets_sent_interval;

float NetworkState::packets_received_per_sec;
long  NetworkState::packets_received_last_sec;
long  NetworkState::packets_received_time;
TimeStamp NetworkState::packets_received_interval;

long NetworkState::opcodes_sent;
long NetworkState::opcodes_received;

float NetworkState::opcodes_sent_per_sec;
long  NetworkState::opcodes_sent_last_sec;
Timer NetworkState::opcodes_sent_timer;
float NetworkState::opcodes_received_per_sec;
long  NetworkState::opcodes_received_last_sec;
Timer NetworkState::opcodes_received_timer;

Timer     NetworkState::ping_timer;
TimeStamp NetworkState::ping_time_stamp;
float     NetworkState::ping_time = 0;


void NetworkState::resetNetworkStats( void )
{
    bytes_sent = 0;
    bytes_sent_last_sec = 0;
    bytes_sent_per_sec = 0.0f;

    bytes_received = 0;
    bytes_received_last_sec = 0;
    bytes_received_per_sec = 0.0f;

    packets_sent = 0;
    packets_received = 0;

    packets_sent_per_sec = 0;
    packets_sent_time = 0;
    packets_sent_interval = now();

    packets_received_per_sec = 0;
    packets_received_time = 0;
    packets_received_interval = now();

    packets_sent_last_sec = 0;
    packets_received_last_sec = 0;

    opcodes_sent = 0;
    opcodes_received = 0;

    opcodes_sent_per_sec = 0;
    opcodes_sent_timer.changePeriod( 1 );
    opcodes_sent_timer.reset();

    opcodes_received_per_sec = 0;
    opcodes_received_timer.changePeriod( 1 );
    opcodes_received_timer.reset();

    opcodes_sent_last_sec = 0;
    opcodes_received_last_sec = 0;

    ping_timer.changeRate(1);
}

void NetworkState::updateNetworkStats( void )
{
    TimeStamp stamp;
    float seconds;

    stamp = now();
    seconds = (stamp - packets_received_interval);

    if( seconds > 1.0f ) {
        packets_received_per_sec = (float) packets_received - (float) packets_received_last_sec;
        bytes_received_per_sec = (float) bytes_received - (float) bytes_received_last_sec;
        bytes_received_last_sec = bytes_received;
        packets_received_last_sec = packets_received;
        packets_received_interval = stamp;
        packets_received_time += long(seconds);


        packets_sent_per_sec = (float) packets_sent - (float) packets_sent_last_sec;
        bytes_sent_per_sec = (float) bytes_sent - (float) bytes_sent_last_sec;
        bytes_sent_last_sec = bytes_sent;
        packets_sent_last_sec = packets_sent;
        packets_sent_interval = stamp;
        packets_sent_time += long(seconds);
    }
}

void NetworkState::logNetworkStats()
{}

