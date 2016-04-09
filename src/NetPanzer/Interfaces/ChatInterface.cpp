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

#include "2D/Color.hpp"

#include "Interfaces/ChatInterface.hpp"
#include "Interfaces/ConsoleInterface.hpp"
#include "Interfaces/PlayerInterface.hpp"
#include "Classes/Network/NetworkState.hpp"
#include "Classes/Network/NetworkServer.hpp"
#include "Classes/Network/NetworkClient.hpp"
#include "Util/Log.hpp"

#include "Scripts/ScriptManager.hpp"

enum { _net_message_id_chat_mesg_req,
       _net_message_id_chat_mesg
};

enum { _chat_mesg_scope_player_set,
       _chat_mesg_scope_alliance,
       _chat_mesg_scope_enemies,
       _chat_mesg_scope_all,
       _chat_mesg_scope_server
};

#define CHATREQUEST_HEADER_LEN (sizeof(NetMessage) + sizeof(Uint8))
#define CHATMESG_HEADER_LEN (sizeof(NetMessage) + sizeof(Uint8) + sizeof(PlayerID))
#define MAX_CHAT_MSG_LEN (_MAX_NET_PACKET_SIZE - CHATMESG_HEADER_LEN)

class ChatMesgRequest : public NetMessage
{
public:
    Uint8 message_scope;
    char message_text[MAX_CHAT_MSG_LEN];

    ChatMesgRequest()
    {
        reset();
    }

    void reset()
    {
        message_class = _net_message_class_chat;
        message_id = _net_message_id_chat_mesg_req;
        message_scope = _chat_mesg_scope_all;
    }

    int getTextLen(size_t size) const
    {
        return size - CHATREQUEST_HEADER_LEN;
    }

} __attribute__((packed));


class ChatMesg: public NetMessage
{
public:
    Uint8  message_scope;
private:
    PlayerID source_player_index;
public:
    char message_text[MAX_CHAT_MSG_LEN];

    ChatMesg()
    {
        message_class = _net_message_class_chat;
        message_id = _net_message_id_chat_mesg;
        memset(message_text, 0, sizeof(message_text));
    }

    int getTextLen(size_t size) const
    {
        return size - CHATMESG_HEADER_LEN;
    }

    PlayerID getSourcePlayerIndex() const
    {
        return source_player_index;
    }

    void setSourcePlayerIndex(PlayerID playerIndex)
    {
        source_player_index = playerIndex;
    }
} __attribute__((packed));

static void chatMessageRequest(const NetPacket* packet)
{
    const ChatMesgRequest* chat_request = (const ChatMesgRequest*) packet->getNetMessage();

    if (   chat_request->message_scope == _chat_mesg_scope_server
        && NetworkState::getNetworkStatus() == _network_state_server
        && packet->fromClient )
    {
        LOGGER.warning("CI_CMR CHEAT Player %u tried to chat as server", packet->fromPlayer);
        return;
    }

    bool post_on_server = false;
    ChatMesg chat_mesg;
    int text_len = chat_request->getTextLen(packet->size);
    const NPString text(chat_request->message_text, 0, text_len);

    chat_mesg.setSourcePlayerIndex(packet->fromPlayer);
    chat_mesg.message_scope = chat_request->message_scope;
    text.copy(chat_mesg.message_text, text_len);

    if ( chat_request->message_scope == _chat_mesg_scope_all )
    {
        if ( text[0] != '/' || ! ScriptManager::runServerCommand(text.substr(1), packet->fromPlayer) )
        {
            SERVER->broadcastMessage(&chat_mesg, CHATMESG_HEADER_LEN + text_len);
            post_on_server = true;
        }
    }
        // XXX ALLY
    else if (chat_request->message_scope == _chat_mesg_scope_alliance)
    {
        PlayerID max_players;
        PlayerID local_player_index;

        local_player_index = PlayerInterface::getLocalPlayerIndex();

        max_players = PlayerInterface::getMaxPlayers();
        for (PlayerID i = 0; i < max_players; ++i)
        {
            if ( PlayerInterface::isAllied( packet->fromPlayer, i) == true )
            {
                if (local_player_index != i)
                {
                    SERVER->sendMessage(i, &chat_mesg, CHATMESG_HEADER_LEN + text_len);
                }
                else
                {
                    post_on_server = true;
                }
            }
        }

        if ( packet->fromPlayer == PlayerInterface::getLocalPlayerIndex() )
        {
            post_on_server = true;
        }
        else
        {
            SERVER->sendMessage(packet->fromPlayer,
                                &chat_mesg, CHATMESG_HEADER_LEN + text_len);
        }
    }
    else if ( chat_request->message_scope == _chat_mesg_scope_server )
    {
        SERVER->broadcastMessage(&chat_mesg, CHATMESG_HEADER_LEN + text_len);
        ConsoleInterface::postMessage(Color::unitAqua, false, 0, "Server: %s", text.c_str());
        return;
    }

    if ( post_on_server == true )
    {
        PlayerState *player_state;

        player_state = PlayerInterface::getPlayer(packet->fromPlayer);

        PIX color = Color::white;

        switch (chat_request->message_scope)
        {
        case _chat_mesg_scope_all:
                color = Color::white;
                break;

        case _chat_mesg_scope_alliance:
                color = Color::yellow;
                break;

        case _chat_mesg_scope_server:
                color = Color::unitAqua;
                break;

        } // ** switch

        // TODO add unitcolor
        ConsoleInterface::postMessage(color, true, player_state->getFlag(),
                        "%s: %s", player_state->getName().c_str(), text.c_str());
    }
}

void ChatInterface::clientHandleChatMessage(const NetMessage* message, size_t size)
{
    if ( message->message_id != _net_message_id_chat_mesg )
    {
        LOGGER.warning("CI_CHCM Received wrong message type: %u",
                       message->message_id);
        return;
    }

    const ChatMesg *chat_mesg = (const ChatMesg*) message;

    if ( chat_mesg->message_scope != _chat_mesg_scope_server
        && chat_mesg->getSourcePlayerIndex() >= PlayerInterface::getMaxPlayers())
    {
            LOGGER.warning("CI_CHCM Received message, incorrect player value: %u, max is %u.",
                            chat_mesg->getSourcePlayerIndex(),
                            PlayerInterface::getMaxPlayers());
            return;
    }

    int text_len = chat_mesg->getTextLen(size);
    const NPString text(chat_mesg->message_text, text_len);

    if ( chat_mesg->message_scope == _chat_mesg_scope_server )
    {
        ConsoleInterface::postMessage(Color::unitAqua, false, 0, "Server: %s",
                                      text.c_str());
        return;
    }

    PlayerState *player_state;
    player_state = PlayerInterface::getPlayer(chat_mesg->getSourcePlayerIndex());

    PIX color = Color::white;

    switch (chat_mesg->message_scope)
    {
    case _chat_mesg_scope_all:
            color = Color::white;
            break;

    case _chat_mesg_scope_alliance:
            color = Color::yellow;
            break;

    case _chat_mesg_scope_server:
            color = Color::unitAqua;
            break;

    } // ** switch

    ConsoleInterface::postMessage(color, true, player_state->getFlag(), "%s: %s",
                                  player_state->getName().c_str(), text.c_str());
}

void ChatInterface::processChatMessages(const NetPacket* packet)
{
    switch (packet->getNetMessage()->message_id)
    {
        case _net_message_id_chat_mesg_req:
            chatMessageRequest(packet);
            break;

        default:
            LOGGER.warning("CI_PCM Received unknown chat message (id %d-%d)",
                            packet->getNetMessage()->message_class,
                            packet->getNetMessage()->message_id);
    }
}

static void sendScopedMessage(const NPString& message, Uint8 scope)
{
    unsigned int text_len = std::min(message.length(), MAX_CHAT_MSG_LEN);
    ChatMesgRequest cmsg;

    message.copy(cmsg.message_text, text_len);
    cmsg.message_scope = scope;

    if (NetworkState::status == _network_state_client)
    {
        CLIENT->sendMessage(&cmsg, CHATREQUEST_HEADER_LEN + text_len);
    }
    else
    {
        EnqueueIncomingPacket(&cmsg, CHATREQUEST_HEADER_LEN + text_len,
                              PlayerInterface::getLocalPlayerIndex(), 0);
    }
}

void ChatInterface::say(const NPString& message)
{
    sendScopedMessage(message, _chat_mesg_scope_all);
}

void ChatInterface::teamsay(const NPString& message)
{
    sendScopedMessage(message, _chat_mesg_scope_alliance);
}

void ChatInterface::serversay(const NPString& message)
{
    sendScopedMessage(message, _chat_mesg_scope_server);
}

void ChatInterface::serversayTo(const PlayerID player, const NPString& message)
{
    if ( ! PlayerInterface::isValidPlayerID(player) )
    {
        LOGGER.warning("CI_SST CHEAT Try to say something to unexisting player: %u",
                       player);
    }

    if ( ! PlayerInterface::isPlayerActive(player) )
    {
        LOGGER.warning("CI_SST CHEAT Try to say something to inactive player: %u",
                       player);
    }

    if ( PlayerInterface::isLocalPlayer(player) )
    {
        ConsoleInterface::postMessage(Color::unitAqua, false, 0, "Server: %s",
                                      message.c_str());
    }
    else
    {
        unsigned int text_len = std::min(message.length(), MAX_CHAT_MSG_LEN);
        ChatMesg cmsg;

        message.copy(cmsg.message_text, text_len);
        cmsg.setSourcePlayerIndex(PlayerInterface::getLocalPlayerIndex());
        cmsg.message_scope = _chat_mesg_scope_server;

        SERVER->sendMessage(player, &cmsg, CHATMESG_HEADER_LEN + text_len);
    }
}

