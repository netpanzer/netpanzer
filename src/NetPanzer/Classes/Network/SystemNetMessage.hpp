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
#ifndef _SYSTEMNETMESSAGE_HPP
#define _SYSTEMNETMESSAGE_HPP

#include <string.h>

#include "Classes/PlayerState.hpp"
#include "NetMessage.hpp"
#include "Types/iXY.hpp"

#ifdef MSVC
#pragma pack(1)
#endif

enum { _net_message_id_system_set_view,
       _net_message_id_system_reset_game_logic,
       _net_message_id_system_view_control,
       _net_message_id_system_ping_request,
       _net_message_id_system_ping_ack,
       _net_message_id_system_connect_alert,
       _net_message_id_system_enckeychange,
       _net_message_id_system_enckeychange_ack,
       _net_message_id_system_re_enckeychange_ack
     };


class SystemSetPlayerView : public NetMessage
{
private:
    Sint32 camera_loc_x;
    Sint32 camera_loc_y;

public:
    SystemSetPlayerView(Sint32 x, Sint32 y)
    {
        message_class = _net_message_class_system;
        message_id = _net_message_id_system_set_view;
        camera_loc_x = htol32(x);
        camera_loc_y = htol32(y);
    }

    Sint32 getCameraLocX() const
    {
        return ltoh32(camera_loc_x);
    }
    Sint32 getCameraLocY() const
    {
        return ltoh32(camera_loc_y);
    }
}
__attribute__((packed));


class SystemResetGameLogic : public NetMessage
{
public:
    SystemResetGameLogic()
    {
        message_class = _net_message_class_system;
        message_id = _net_message_id_system_reset_game_logic;
    }
} __attribute__((packed));

enum { _view_control_flag_visible_on  = 0x01,
       _view_control_flag_visible_off = 0x02,
       _view_control_flag_center_abs  = 0x04,
       _view_control_flag_close_all   = 0x08
     };

class SystemViewControl : public NetMessage
{
public:
    Uint8 action_flags;
    char view_name[32];

    SystemViewControl()
    {
        message_class = _net_message_class_system;
        message_id = _net_message_id_system_view_control;
        action_flags = 0;
        memset(view_name, 0, sizeof(view_name));
    }

    void set(const char *name, unsigned char flags)
    {
        snprintf(view_name, sizeof(view_name), "%s", name);
        action_flags = flags;
    }
} __attribute__((packed));

class SystemPingRequest : public NetMessage
{
private:
    PlayerID client_player_index;

public:
    SystemPingRequest(PlayerID playerIndex)
    {
        message_class = _net_message_class_system;
        message_id = _net_message_id_system_ping_request;
        client_player_index = playerIndex;
    }

    PlayerID getClientPlayerIndex() const
    {
        return client_player_index;
    }
} __attribute__((packed));

class SystemPingAcknowledge : public NetMessage
{
public:
    SystemPingAcknowledge()
    {
        message_class = _net_message_class_system;
        message_id = _net_message_id_system_ping_ack;
    }
} __attribute__((packed));


enum { _connect_alert_mesg_connect,
       _connect_alert_mesg_disconnect,
       _connect_alert_mesg_client_drop,
       _connect_alert_mesg_client_kicked
     };

class SystemConnectAlert : public NetMessage
{
private:
    PlayerID player_index;
public:
    Uint8 alert_enum;

    SystemConnectAlert()
    {
        message_class = _net_message_class_system;
        message_id = _net_message_id_system_connect_alert;
    }

    void set(const PlayerID player_idx, unsigned char alert_type)
    {
        player_index = player_idx;
        alert_enum = alert_type;
    }

    PlayerID getPlayerID() const
    {
        return player_index;
    }

} __attribute__((packed));

class SystemEnckeychange : public NetMessage
{

private: Uint8 xorkey;

public:
    SystemEnckeychange(unsigned short x_key)
    {
        message_class = _net_message_class_system;
        message_id = _net_message_id_system_enckeychange; //_6
        //xorkey = rand() % 255;
        xorkey = x_key;
    }

    unsigned short getEncKey() const
    {
    return (unsigned short)xorkey;
    }

} __attribute__((packed));

class SystemEnckeychangeAck : public NetMessage
{

private: Uint8 recvkey;
         PlayerID player_index;
public:
    SystemEnckeychangeAck(unsigned short r_key, PlayerID p_index)
    {
        message_class = _net_message_class_system;
        message_id = _net_message_id_system_enckeychange_ack; //_7
        recvkey = r_key;
        player_index = p_index;
    }

    PlayerID getPlayerID() const
    {
    return player_index;
    }

    unsigned short getEncKey() const
    {
    return (unsigned short)recvkey;
    }

} __attribute__((packed));


class SystemReEnckeychangeAck : public NetMessage
{

private: float down_ping;

public:
    SystemReEnckeychangeAck(float d_ping)
    {
        message_class = _net_message_class_system;
        message_id = _net_message_id_system_re_enckeychange_ack;
        down_ping = d_ping;

    }

    float getDownPing() const
    {
    return (float)down_ping;
    }

} __attribute__((packed));


#ifdef MSVC
#pragma pack()
#endif

#endif // ** _SYSTEMNETMESSAGE_HPP
