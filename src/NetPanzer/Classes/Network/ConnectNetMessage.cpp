/*
Copyright (C) 2004 Hollis Blanchard

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


#include <string.h>

#include "ConnectNetMessage.hpp"
#include "Util/Endian.hpp"

ClientConnectJoinRequest::ClientConnectJoinRequest()
{
    message_class = _net_message_class_connect;
    message_id = _net_message_id_connect_join_game_request;
}

Uint32 ClientConnectJoinRequest::getProtocolVersion() const
{
    return ltoh32(protocol_version);
}

void ClientConnectJoinRequest::setProtocolVersion(Uint32 version)
{
    protocol_version = htol32(version);
}

Uint8 ClientConnectJoinRequest::getNMode() const
{
    return Uint8(n_mode);
}

void ClientConnectJoinRequest::setNMode(Uint8 nm)
{
    n_mode = Uint8(nm);
}

void ClientConnectJoinRequest::setPassword(const NPString& password)
{
    int copylen = std::min(password.size(), sizeof(this->password)-1);

    if ( copylen > 0 )
    {
        password.copy(this->password,copylen);
    }

    this->password[copylen] = 0;
}

void ClientConnectJoinRequest::getPassword(NPString& password) const
{
    char pw[sizeof(this->password)];
    memcpy(pw, this->password, sizeof(this->password));
    pw[sizeof(pw)-1] = 0;
    password.assign(pw);
}

ClientConnectJoinRequestAck::ClientConnectJoinRequestAck()
{
    message_class = _net_message_class_connect;
    message_id = _net_message_id_connect_join_game_request_ack;
}

Uint32 ClientConnectJoinRequestAck::getServerProtocolVersion() const
{
    return ltoh32(server_protocol_version);
}

void ClientConnectJoinRequestAck::setServerProtocolVersion(Uint32 protocol_version)
{
    server_protocol_version = htol32(protocol_version);
}


ConnectProcessUpdate::ConnectProcessUpdate()
{
    message_class = _net_message_class_connect;
    message_id = _net_message_id_client_connect_process_update;
}

ConnectProcessStateMessage::ConnectProcessStateMessage()
{
    message_class = _net_message_class_connect;
    message_id = _net_message_id_client_connect_process_state_mesg;
}

ConnectClientSettings::ConnectClientSettings()
{
    message_class = _net_message_class_connect;
    message_id = _net_message_id_connect_client_settings;
    memset(&player_name, 0, sizeof(player_name));
}

Uint8 ConnectClientSettings::getNStatus() const
{
    return Uint8(nstatus);
}

void ConnectClientSettings::setNStatus(Uint8 ns)
{
    nstatus = Uint8(ns);
}

void ConnectClientSettings::set(const char *player_name)
{
    strncpy(ConnectClientSettings::player_name, player_name, 21);
    ConnectClientSettings::player_name[20] = '\0';
}


ConnectMesgServerGameSettings::ConnectMesgServerGameSettings()
{
    message_class = _net_message_class_connect;
    message_id = _net_message_id_connect_server_game_setup;
    memset(map_name, 0, sizeof(map_name));
    memset(map_style, 0, sizeof(map_style));
}

Uint16 ConnectMesgServerGameSettings::getMaxPlayers() const
{
    return ltoh16(max_players);
}

void ConnectMesgServerGameSettings::setMaxPlayers(Uint16 maxPlayers)
{
    max_players = htol16(maxPlayers);
}

Uint16 ConnectMesgServerGameSettings::getMaxUnits() const
{
    return ltoh16(max_units);
}

void ConnectMesgServerGameSettings::setMaxUnits(Uint16 maxUnits)
{
    max_units = htol16(maxUnits);
}

Sint32 ConnectMesgServerGameSettings::getCloudCoverage() const
{
    return ltoh32(cloud_coverage);
}

void ConnectMesgServerGameSettings::setCloudCoverage(Sint32 cloudCoverage)
{
    cloud_coverage = htol32(cloudCoverage);
}

float ConnectMesgServerGameSettings::getWindSpeed() const
{
    return (float)ltoh32((Uint32)wind_speed);
}

void ConnectMesgServerGameSettings::setWindSpeed(float windSpeed)
{
    wind_speed = (float)htol32((Uint32)windSpeed);
}

Sint32 ConnectMesgServerGameSettings::getGameType() const
{
    return ltoh32(game_type);
}

void ConnectMesgServerGameSettings::setGameType(Sint32 gameType)
{
    game_type = htol32(gameType);
}

Sint32 ConnectMesgServerGameSettings::getFragLimit() const
{
    return ltoh32(frag_limit);
}

void ConnectMesgServerGameSettings::setFragLimit(Sint32 fragLimit)
{
    frag_limit = htol32(fragLimit);
}

Sint32 ConnectMesgServerGameSettings::getTimeLimit() const
{
    return ltoh32(time_limit);
}

void ConnectMesgServerGameSettings::setTimeLimit(Sint32 timeLimit)
{
    time_limit = htol32(timeLimit);
}

time_t ConnectMesgServerGameSettings::getElapsedTime() const
{
    return (time_t)ltoh32((Uint32)elapsed_time);
}

void ConnectMesgServerGameSettings::setElapsedTime(time_t elapsedTime)
{
    elapsed_time = (time_t)htol32((Uint32)elapsedTime);
}

Sint32 ConnectMesgServerGameSettings::getFlagTime() const
{
    return ltoh32(flag_time);
}

void ConnectMesgServerGameSettings::setFlagTime(Sint32 flagtime)
{
    flag_time = htol32(flagtime);
}

Sint32 ConnectMesgServerGameSettings::getLowScoreLimit() const
{
    return ltoh32(low_score_limit);
}

void ConnectMesgServerGameSettings::setLowScoreLimit(Sint32 lowScoreLimit)
{
    low_score_limit = htol32(lowScoreLimit);
}
