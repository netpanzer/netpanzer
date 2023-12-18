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


#include "Util/Log.hpp"
#include "Core/NetworkGlobals.hpp"

#include "NetworkClient.hpp"

#include "Classes/Network/NetworkState.hpp"

#include "ConnectNetMessage.hpp"

#include "Interfaces/PlayerInterface.hpp"
#include "Classes/Network/ClientConnectDaemon.hpp"

#include "Interfaces/ConsoleInterface.hpp"

#include "Views/Components/Desktop.hpp"
#include "Views/Game/LoadingView.hpp"
#include "Views/Game/DisconectedView.hpp"

#include "Interfaces/GameManager.hpp"

NetworkClient *CLIENT = 0;

NetworkClient::NetworkClient( void )
        : NetworkInterface(), clientsocket(0)
{
    connection_status = _connection_status_no_connection;
}

NetworkClient::~NetworkClient()
{
    if ( clientsocket )
        delete clientsocket;
}

void
NetworkClient::onClientConnected(ClientSocket *s)
{
    (void)s;
    ClientConnectJoinRequest join_request;
    unsigned char nms;
    connection_status = _connection_status_connected;
    nms = NetworkState::status;
    join_request.setProtocolVersion(NETPANZER_PROTOCOL_VERSION);
    join_request.setNMode(nms);
    join_request.setPassword(password);

    sendMessage( &join_request, sizeof(ClientConnectJoinRequest));
}

void
NetworkClient::onClientDisconected(ClientSocket *s, const char *msg)
{
    if ( NetworkState::status == _network_state_bot ) // client bot only
    {
       //do nothing
       delete clientsocket;
       clientsocket=0;
       GameManager::exitNetPanzer();
    }
    else
    {

    (void)s;
    if ( ! Desktop::getVisible("LoadingView") )
    {
        DisconectedView *dv = (DisconectedView *)Desktop::getView("DisconectedView");
        dv->setMsg(msg);

        Desktop::setVisibility("DisconectedView", true);
    }
    else
    {
        ClientConnectDaemon::serverConnectionBroken();
    }

    delete clientsocket;
    clientsocket=0;
    }

}

bool NetworkClient::joinServer(const NPString& server_name, const NPString& password)
{
    if ( clientsocket )
        delete clientsocket;
    clientsocket = 0;
    this->password = password;

    LOG( ("Trying to join server '%s'.\n", server_name.c_str()) );
    try
    {
        clientsocket = new ClientSocket(this, server_name);
    }
    catch(std::exception& e)
    {
        LOG( ( "Couldn't connect to server:\n%s.", e.what()) );
        char text[128];
        snprintf(text, 128, "connection error: %s", e.what());
        LoadingView::append(text);
        return false;
    }

    return true;
}

void NetworkClient::partServer()
{
    if ( clientsocket )
        delete clientsocket;
    clientsocket = 0;
}

void NetworkClient::sendMessage(const NetMessage* message, size_t size)
{
    if ( !clientsocket )
    {
        EnqueueIncomingPacket( message, size, PlayerInterface::getLocalPlayerIndex(), 0);
    }
    else
    {
        clientsocket->sendMessage(message, size);
        NetworkState::incPacketsSent(size);
    }

#ifdef NETWORKDEBUG
    NetPacketDebugger::logMessage("S", message);
#endif
}

bool NetworkClient::getPacket(NetPacket *packet)
{
    if ( clientsocket && receive_queue.isReady() )
    {
        receive_queue.dequeue( packet );

#ifdef NETWORKDEBUG
        NetPacketDebugger::logMessage("R", packet->getNetMessage());
#endif

        NetworkState::incPacketsReceived(packet->size);
        return true;
    }
    return false;
}

void
NetworkClient::sendRemaining()
{
    if ( clientsocket )
        clientsocket->sendRemaining();
}
