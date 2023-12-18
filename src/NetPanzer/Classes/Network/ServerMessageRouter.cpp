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

#include "ServerMessageRouter.hpp"

#include "Classes/Network/NetworkServer.hpp"
#include "Classes/Network/ServerConnectDaemon.hpp"
#include "Units/UnitInterface.hpp"
#include "Interfaces/PlayerInterface.hpp"
#include "Objectives/ObjectiveInterface.hpp"
#include "Interfaces/GameManager.hpp"
#include "Interfaces/ChatInterface.hpp"

#include "TerminalNetMesg.hpp"
#include "SystemNetMessage.hpp"
#include "ConnectNetMessage.hpp"
#include "PlayerNetMessage.hpp"
#include "Util/Log.hpp"


NetPacket ServerMessageRouter::temp_packet;
NetMessageDecoder ServerMessageRouter::message_decoder;

void ServerMessageRouter::initialize()
{
}

void ServerMessageRouter::cleanUp()
{
}

void ServerMessageRouter::processTerminalPacket(const NetPacket* packet)
{
    const NetMessage* message = packet->getNetMessage();
    switch(message->message_id) {
        case _net_message_id_term_unit_cmd:
            UnitInterface::processNetPacket(packet);
            break;

        default:
            LOGGER.warning("unnown Terminal Message (id %d, player %u)",
                    message->message_id, packet->fromPlayer);
    }
}

void ServerMessageRouter::routePacket(const NetPacket* packet)
{
    const NetMessage* message = packet->getNetMessage();
    PlayerState * player = PlayerInterface::getPlayer(packet->fromPlayer);

    switch (message->message_class) {
        case _net_message_class_terminal:
            if ( player )
            {
                player->resetAutokick();
            }
            processTerminalPacket(packet);
            break;

        case _net_message_class_objective:
            ObjectiveInterface::serverHandleNetPacket(packet);
            break;

        case _net_message_class_system:
            if ( player )
            {
                player->resetAutokick();
            }
            GameManager::processSystemMessage(message);
            break;

        case _net_message_class_chat:
            if ( player )
            {
                player->resetAutokick();
            }
            ChatInterface::processChatMessages(packet);
            break;

        case _net_message_class_connect:
            ServerConnectDaemon::processNetPacket(packet);
            break;

        case _net_message_class_player:
            if ( player )
            {
                player->resetAutokick();
            }
            PlayerInterface::processNetMessage(packet);
            break;

        default:
            LOGGER.warning("Packet contained unknown message class: %d",
                    message->message_class);
                    /*
                    char string_warning_k[140];
                    sprintf(string_warning_k, "Player '%s' is cheating (trying to use packet sniffer/editor tools)!",player->getName().c_str());
                    LOGGER.info("CHEAT: %s", string_warning_k);
                    ChatInterface::serversay(string_warning_k);
                    */
                    //player->setCheat(1);
                    unsigned char p_hits = player->getCheatHits();
                    if (p_hits < 255) {
                    player->setCheatHits(p_hits++);
                    }
                    // kicking player here may cause server to crash!?!
                    //SERVER->kickClient(SERVER->getClientSocketByPlayerIndex((unsigned short) player->getID()));


    }
}

void ServerMessageRouter::routeMessages()
{
    ServerConnectDaemon::connectProcess();
    Uint16 msg_len;
    NetMessage* mmessage;

    while(SERVER->getPacket(&temp_packet) == true)
    {
        const NetMessage* message = temp_packet.getNetMessage();
        if (message->message_class == _net_message_class_multi)
        {
            message_decoder.setDecodeMessage((const MultiMessage *) message, temp_packet.size);

            NetPacket packet;
            packet.fromPlayer = temp_packet.fromPlayer;
            packet.fromClient = temp_packet.fromClient;

            while ( (msg_len = message_decoder.decodeMessage(&mmessage)) )
            {
                memcpy(packet.data, mmessage, msg_len);
                routePacket(&packet);
            }
        }
        else
        {
            routePacket(&temp_packet);
        }
    }
}
