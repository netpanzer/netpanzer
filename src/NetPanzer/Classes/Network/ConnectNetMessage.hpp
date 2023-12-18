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
#ifndef _CONNECTNETMESSAGE_HPP
#define _CONNECTNETMESSAGE_HPP

#include <time.h>
#include "NetMessage.hpp"
#include "Classes/PlayerState.hpp"
#include "Core/CoreTypes.hpp"

enum { _net_message_id_connect_join_game_request,
       _net_message_id_connect_join_game_request_ack,
       _net_message_id_client_start_connect,

       _net_message_id_connect_server_full,
       _net_message_id_connect_server_busy,

       _net_message_id_client_connect_request,
       _net_message_id_client_connect_result,
       _net_message_id_connect_client_settings,
       _net_message_id_client_connect_process_update,
       _net_message_id_client_connect_process_state_mesg,
       _net_message_id_connect_server_game_setup,
       _net_message_id_connect_client_game_setup_ping,
       _net_message_id_connect_client_game_setup_ack,
       _net_message_id_connect_client_send_next_unit_profile,
       _net_message_id_connect_client_abort,
       _net_message_id_connect_netPanzer_client_disconnect,
       _net_message_id_connect_netPanzer_server_disconnect
     };

#ifdef MSVC
#pragma pack(1)
#endif

class ClientConnectJoinRequest : public NetMessage
{
private:
    Uint32 protocol_version;
    char password[32];
    Uint8 n_mode;

public:
    ClientConnectJoinRequest();
    Uint32 getProtocolVersion() const;
    void setProtocolVersion(Uint32 version);
    void setPassword( const NPString& password );
    void getPassword(NPString& password) const;
    void setNMode(unsigned char nm);
    unsigned char getNMode() const;

} __attribute__((packed));

enum { _join_request_result_success,
       _join_request_result_invalid_protocol,
       _join_request_result_server_busy,
       _join_request_result_already_connected,
       _join_request_result_wrong_password,
       _join_request_result_banned, // XXX to be done
       _join_request_result_access_denied,
       _join_request_result_authentication_failed
     };

class ClientConnectJoinRequestAck : public NetMessage
{
private:
    Uint8  result_code;
    Uint32 server_protocol_version;

public:
    ClientConnectJoinRequestAck();

    Uint8 getResultCode() const { return result_code; }
    void setResultCode(Uint8 result) { result_code = result; }
    Uint32 getServerProtocolVersion() const;
    void setServerProtocolVersion(Uint32 protocol_version);
} __attribute__((packed));

class ClientConnectStartConnect : public NetMessage
{
public:
    ClientConnectStartConnect()
    {
        message_class = _net_message_class_connect;
        message_id = _net_message_id_client_start_connect;
    }
} __attribute__((packed));

class ClientConnectRequest : public NetMessage
{
public:
    ClientConnectRequest()
    {
        message_class = _net_message_class_connect;
        message_id = _net_message_id_client_connect_request;
    }
} __attribute__((packed));

enum { _connect_result_success,
       _connect_result_server_busy,
       _connect_result_server_full,
       _connect_result_server_already_connected
     };

class ClientConnectResult : public NetMessage
{
public:
    Uint8 result_code;

    ClientConnectResult()
    {
        message_class = _net_message_class_connect;
        message_id = _net_message_id_client_connect_result;
    }
} __attribute__((packed));


class ConnectProcessUpdate : public NetMessage
{
private:
    Uint8 queue_position;

public:
    ConnectProcessUpdate();
    Uint8 getQueuePosition() const { return queue_position; }
    void setQueuePosition(Uint8 position) { queue_position = position; }
} __attribute__((packed));

enum { _connect_state_message_load_game_data,
       _connect_state_message_sync_unit_profiles,
       _connect_state_message_sync_player_info,
       _connect_state_message_sync_player_info_percent,
       _connect_state_message_sync_units,
       _connect_state_message_sync_units_percent,
       _connect_state_sync_complete
     };

class ConnectProcessStateMessage : public NetMessage
{
private:
    Uint8 message_enum;
    Uint8 percent_complete;

public:
    ConnectProcessStateMessage();
    Uint8 getMessageEnum() const { return message_enum; }
    void setMessageEnum(Uint8 message) { message_enum = message; }
    Uint8 getPercentComplete() const { return percent_complete; }
    void setPercentComplete(Uint8 percent) { percent_complete = percent; }
} __attribute__((packed));

class ConnectClientSettings : public NetMessage
{
private:
    Uint8 nstatus;

public:
    char player_name[21];
    Uint8 player_flag[FLAG_WIDTH*FLAG_HEIGHT];
    ConnectClientSettings();
    void set(const char *player_name);
    void setNStatus(unsigned char ns);
    unsigned char getNStatus() const;
} __attribute__((packed));

class ConnectMesgServerGameSettings : public NetMessage
{
private:
    Uint16 max_players;
    Uint16 max_units;
public:
    char     map_name[32];
    char     map_style[17];
    char     tank_styles[176];
private:
    Sint32  cloud_coverage;
    float    wind_speed;
    Sint32  game_type;
public:
    Uint8  powerup_state;
    Uint8  occupation_percentage;
    Uint8  enable_bases;
    Uint8  base_capture_mode;
    Uint8  base_limit;
    Uint8  allowmultiip;
    Uint8  allowallies;
    Uint8  respawntype;
private:
    Sint32  frag_limit;
    Sint32  time_limit;
    time_t   elapsed_time;
    Sint32  flag_time;
    Sint32  low_score_limit;


public:
    ConnectMesgServerGameSettings();
    Uint16 getMaxPlayers() const;
    void setMaxPlayers(Uint16 maxPlayers);
    Uint16 getMaxUnits() const;
    void setMaxUnits(Uint16 maxUnits);
    Sint32 getCloudCoverage() const;
    void setCloudCoverage(Sint32 cloudCoverage);
    float getWindSpeed() const;
    void setWindSpeed(float windSpeed);
    Sint32 getGameType() const;
    void setGameType(Sint32 gameType);
    Sint32 getFragLimit() const;
    void setFragLimit(Sint32 fragLimit);
    Sint32 getTimeLimit() const;
    void setTimeLimit(Sint32 timeLimit);
    time_t getElapsedTime() const;
    void setElapsedTime(time_t elapsedTime);
    Sint32 getFlagTime() const;
    void setFlagTime(Sint32 flagtime);
    Sint32 getLowScoreLimit() const;
    void setLowScoreLimit(Sint32 lowScoreLimit);
} __attribute__((packed));

class ConnectMesgClientGameSetupAck : public NetMessage
{
public:
    ConnectMesgClientGameSetupAck()
    {
        message_class = _net_message_class_connect;
        message_id = _net_message_id_connect_client_game_setup_ack;
    }
} __attribute__((packed));

class ConnectMesgClientSendNextUnit : public NetMessage
{
public:
    ConnectMesgClientSendNextUnit()
    {
        message_class = _net_message_class_connect;
        message_id = _net_message_id_connect_client_send_next_unit_profile;
    }
} __attribute__((packed));



class ConnectMesgClientGameSetupPing : public NetMessage
{
public:
    ConnectMesgClientGameSetupPing()
    {
        message_class = _net_message_class_connect;
        message_id = _net_message_id_connect_client_game_setup_ping;
    }
} __attribute__((packed));

class ConnectMesgNetPanzerClientDisconnect : public NetMessage
{
private:
    PlayerID player_id;

public:
    ConnectMesgNetPanzerClientDisconnect()
    {
        message_class = _net_message_class_connect;
        message_id = _net_message_id_connect_netPanzer_client_disconnect;
    }

    void setPlayerID(PlayerID id)
    {
        player_id = id;
    }
    PlayerID getPlayerID() const
    {
        return player_id;
    }
} __attribute__((packed));

class ConnectMesgNetPanzerServerDisconnect : public NetMessage
{
public:
    ConnectMesgNetPanzerServerDisconnect()
    {
        message_class = _net_message_class_connect;
        message_id = _net_message_id_connect_netPanzer_server_disconnect;
    }
} __attribute__((packed));

#ifdef MSVC
#pragma pack()
#endif

#endif // ** _CONNECTNETMESSAGE_HPP
