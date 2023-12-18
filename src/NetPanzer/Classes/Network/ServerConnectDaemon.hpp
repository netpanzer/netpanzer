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
#ifndef _SERVERCONNECTDAEMON_HPP
#define _SERVERCONNECTDAEMON_HPP

class ClientSocket;
class NetPacket;

class ServerConnectDaemon
{
private:
    static unsigned char nss;

public:
    static void initialize( unsigned long max_players );

    static void startConnectDaemon( unsigned long max_players );

    static void shutdownConnectDaemon();

    static bool inConnectQueue( ClientSocket *client );

    static void connectProcess();

    static void processNetPacket(const NetPacket* message);

    static void lockConnectProcess();
    static void unlockConnectProcess();
    static bool getConnectLockStatus();

    static bool isConnecting();
    static void removeClientFromQueue(ClientSocket *client);
};

#endif // ** _SERVERCONNECTDAEMON_HPP
