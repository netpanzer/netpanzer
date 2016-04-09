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

#include "ClientMessageRouter.hpp"

#include "NetworkClient.hpp"

#include "Classes/Network/ClientConnectDaemon.hpp"

#include "Interfaces/GameManager.hpp"

#include "Classes/Network/UnitNetMessage.hpp"
#include "PlayerNetMessage.hpp"
#include "SystemNetMessage.hpp"
#include "ConnectNetMessage.hpp"

#include "Objectives/ObjectiveInterface.hpp"
#include "Units/UnitInterface.hpp"
#include "Units/UnitProfileInterface.hpp"
#include "Interfaces/PlayerInterface.hpp"
#include "Interfaces/ChatInterface.hpp"
#include "Interfaces/GameControlRulesDaemon.hpp"
#include "PowerUps/PowerUpInterface.hpp"
#include "Util/Log.hpp"

#include "Classes/Network/NetworkState.hpp"

void ClientMessageRouter::initialize()
{
    // nothing
}

void ClientMessageRouter::cleanUp()
{
    // nothing
}

void
ClientMessageRouter::routeMessage(const NetPacket* packet)
{
    const NetMessage* message = packet->getNetMessage();
    switch (message->message_class)
    {
        case _net_message_class_system:
            GameManager::processSystemMessage(message);
            break;

        case _net_message_class_connect:
            ClientConnectDaemon::processNetMessage(message);
            break;

        case _net_message_class_player:
            PlayerInterface::processNetMessage(packet);
            break;

        case _net_message_class_unit:
            UnitInterface::processNetMessage(message, packet->size);
            break;

        case _net_message_class_unit_profile:
            UnitProfileInterface::processNetMessage(message, packet->size);
            if ( ! ClientConnectDaemon::isIdle() )
            {
                ConnectMesgClientSendNextUnit msg;
                CLIENT->sendMessage(&msg, sizeof(msg));
//                CLIENT->sendRemaining();
            }
            break;

        case _net_message_class_objective:
            ObjectiveInterface::clientHandleNetMessage(message);
            break;

        case _net_message_class_game_control:
            GameControlRulesDaemon::processNetMessage(message);
            break;

        case _net_message_class_powerup:
            PowerUpInterface::processNetMessages(message);
            break;

        case _net_message_class_chat:
            ChatInterface::clientHandleChatMessage(message, packet->size);
            break;

        default:
            LOGGER.warning("Message has unknown message class %d.",
                    message->message_class);
            break;
    }       
}


void
ClientMessageRouter::routeMessages()
{
    ClientConnectDaemon::connectProcess( );

    if( NetworkState::ping_timer.count() ) {
        //GameManager::requestNetworkPing();
    }

    NetPacket packet;

    Uint16 msg_len;
    NetMessage* mmessage;

    while ( CLIENT->getPacket(&packet) )
    {
        const NetMessage* message = packet.getNetMessage();
        if (message->message_class == _net_message_class_multi)
        {
            NetMessageDecoder message_decoder;
            message_decoder.setDecodeMessage( (const MultiMessage *) message, packet.size );

            NetPacket pp;
            pp.fromPlayer = packet.fromPlayer;
            pp.fromClient = packet.fromClient;

            while( (msg_len = message_decoder.decodeMessage(&mmessage)) )
            {
                memcpy(pp.data, mmessage, msg_len);
                routeMessage(&pp);
            }
        } else {
            routeMessage(&packet);
        }
    }

}
