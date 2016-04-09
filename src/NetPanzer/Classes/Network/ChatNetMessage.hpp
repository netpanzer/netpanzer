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
#ifndef _CHATNETMESSAGE_HPP
#define _CHATNETMESSAGE_HPP

#include <string.h>
#include <assert.h>
#include "NetMessage.hpp"

enum { _net_message_id_chat_mesg_req,
       _net_message_id_chat_mesg
};

enum { _chat_mesg_scope_player_set,
       _chat_mesg_scope_alliance,
       _chat_mesg_scope_enemies,
       _chat_mesg_scope_all,
       _chat_mesg_scope_server
};

class ChatMesgRequest : public NetMessage
{
public:
    Uint8 message_scope;
    char player_set[32];

private:
    Uint16 source_player_index;
public:
    char message_text[150];

    ChatMesgRequest()
    {
        reset();
    }
    
    void reset()
    {
        message_class = _net_message_class_chat;
        message_id = _net_message_id_chat_mesg_req;
        memset(player_set, 0, sizeof(player_set));
        message_scope = _chat_mesg_scope_all;
    }

    void setPlayerSet(Uint16 player_index)
    {
        assert(player_index < sizeof(player_set));
        
        unsigned long index;
        unsigned char shift;
        unsigned char mask = 1;

        index = player_index;
        shift = (unsigned char) ( 7 - (index & (unsigned long) 7 ) ); // 7 - (index % 8)
        index = index >> 3;
        // index / 8
        mask = mask << shift;
            
        player_set[index] = player_set[index] | mask;
    }
    
    void clearPlayerSet(Uint16 player_index)
    {
        assert(player_index < sizeof(player_set));
        
        unsigned long index;
        unsigned char shift;
        unsigned char mask = 1;

        index = (player_index);
        shift = (unsigned char) ( 7 - (index & (unsigned long) 7 ) );
        index = index >> 3;
        mask = ~(mask << shift);
        
        player_set[ index ] = player_set[index] & mask;
    }
    Uint16 getSourcePlayerIndex() const
    {
        return ltoh16(source_player_index);
    }
    void setSourcePlayerIndex(Uint16 playerIndex)
    {
        source_player_index = htol16(playerIndex);
    }
} __attribute__((packed));


class ChatMesg: public NetMessage
{
public:
    unsigned char  message_scope;
private:
    Uint16 source_player_index;
public:
    char message_text[150];

    ChatMesg()
    {
        message_class = _net_message_class_chat;
        message_id = _net_message_id_chat_mesg;
        memset(message_text, 0, sizeof(message_text));
    }

    Uint16 getSourcePlayerIndex() const
    {
        return ltoh16(source_player_index);
    }
    void setSourcePlayerIndex(Uint16 playerIndex)
    {
        source_player_index = htol16(playerIndex);
    }
} __attribute__((packed));

#endif

