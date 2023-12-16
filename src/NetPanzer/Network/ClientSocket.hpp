/*
Copyright (C) 2003 Matthias Braun <matze@braunis.de>

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
#ifndef __UILIB_NETWORK_CLIENTSOCKET_HPP__
#define __UILIB_NETWORK_CLIENTSOCKET_HPP__

#include <string>

#include "Classes/Network/NetPacket.hpp"
#include "ProxyServer.hpp"
#include "Network/TCPSocket.hpp"
//#include "Util/NTimer.hpp"
class ClientSocket;
#define SEND_BUFFER_LEN 32768

#define ANTI_SPAM_LIMIT 600


class ClientSocketObserver
{
public:
    ClientSocketObserver(){};
    virtual ~ClientSocketObserver(){};
protected:
    friend class ClientSocket;
    virtual void onClientConnected(ClientSocket *cso) = 0;
    virtual void onClientDisconected(ClientSocket *cso, const char * msg) = 0;
};

class ClientSocket : public network::TCPSocketObserver
{
public:
    ClientSocket(ClientSocketObserver *o, const std::string& serveraddress);
    ClientSocket(ClientSocketObserver *o);
    ~ClientSocket();

    //void read();
    void sendMessage(const void* data, size_t datasize);
    void sendRemaining();
    ProxyServer proxy;


    int getId() { return id; };
    std::string getFullIPAddress();
    std::string getIPAddress();

    PlayerID getPlayerIndex()
    {
        return player_id;
    }

    void hardClose()
    {
        if ( socket )
        {
            socket->destroy();
        }
    }

protected:
    void onDataReceived(network::TCPSocket *so, const char *data, const int len);
    void onConnected(network::TCPSocket *so);
    void onDisconnected(network::TCPSocket *so);
    void onSocketError(network::TCPSocket *so);

private:
    friend class ServerConnectDaemon;
    friend class NetworkClient;
    void initId();
    ClientSocketObserver * observer;
    network::TCPSocket* socket;
    //NTimer conn_pause;
    char sendbuffer[SEND_BUFFER_LEN];
    unsigned int sendpos;
    char tempbuffer[_MAX_NET_PACKET_SIZE];
    Uint16 tempoffset;
    int id;
public:    PlayerID player_id;
    // fu added
    unsigned int lastPActTime2;
    unsigned int lastPActTime1;
    unsigned int lastPActTime0;
    unsigned int packetDelta0;
    unsigned int packetDelta;
    unsigned int currentPActTime;
    unsigned int mydatastrtime;
    unsigned int mydatastrtime0;
    unsigned int burstTime;
    unsigned int burstTime0;
    unsigned int burstDelta;
    unsigned int commandBurst;
    unsigned int commandBurstLimit;
    int mydatastrc;
    unsigned short int conn_end;
    unsigned short int pre_conn_end;
    unsigned short int packets_count;
    //unsigned short int slowdown;
    int opc_0;
    int opc_1;
    int opc_2;

    Uint8 encryptKeySend;
    Uint8 encryptKeyRecv;
    Uint8 popc_0;
    Uint8 popc_0b;
    Uint8 popc_t;
    Uint8 popc_1b;
    Uint8 popc_t1;
    int opcv_0;
    int opcv_1;
    Uint8 key2;
    Uint8 key0;
    Uint8 opcv_2;
    Uint8 encrypted;
    Uint8 encryptedb;
    Uint8 encrypted2;
    Uint8 encryptedb2;
/*
    Uint8 popc_0;
    Uint8 popc_0b;
    Uint8 popc_t;
    int opcv_0;
    int opcv_1;
    Uint8 key2;
    Uint8 key0;
    Uint8 opcv_2;
    Uint8 encrypted;
    Uint8 encryptedb;
    Uint8 encrypted2;
    Uint8 encryptKeySend;
    Uint8 encryptKeyRecv;
    Uint8 encryptKeySendS;
    Uint8 encryptKeyRecvS;
    bool recvf;
*/

};

#endif
