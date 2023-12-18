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
#ifndef _GAMEMANAGER_HPP
#define _GAMEMANAGER_HPP

#include <time.h>
#include <string>

#include "Classes/PlayerState.hpp"
#include "Classes/Network/NetPacket.hpp"
#include "Classes/Network/ClientConnectDaemon.hpp"
#include "Interfaces/GameControlRulesDaemon.hpp"
#include "Types/iXY.hpp"

enum { _mapload_result_success,
       _mapload_result_no_map_file,
       _mapload_result_no_wad_file
     };

class ConnectMesgServerGameSettings;

class GameManager
{
private:
    static std::string map_path;
    static std::string mapstyle_path;


protected:
    static time_t game_start_time;
    static time_t game_elapsed_time_offset;

    static bool display_frame_rate_flag;
    static bool display_network_info_flag;

protected:
    static void finishGameMapLoad();

    // ** Network Message Handlers
    static void netMessageSetView(const NetMessage* message);
    static void netMessageViewControl(const  NetMessage* message);
    static void netMessageClientGameSetup(const NetMessage* message);
    static void netMessagePingRequest(const NetMessage* message);
    static void netMessagePingAcknowledge(const NetMessage* message);
    static void netMessageConnectAlert(const NetMessage* message);
    static void netMessageResetGameLogic(const NetMessage* message);

    static void sendAckEnckeychange(const NetMessage* message);
    static void recvAckEnckeychange(const NetMessage* message);
    static void recvReAckEnckeychange(const NetMessage* message);

public:
    static std::string stmapstyle;
    static unsigned char ststylesnum;
    static std::vector<NPString> stlist;
    //static std::string ststyles;
    static void dedicatedLoadGameMap(const char *map_file_path, const char *mapstyle_name );
    static void startGameMapLoad(const char *map_file_path, const char *map_style_name,
                                 unsigned long partitions);
    static bool gameMapLoad( int *percent_complete );

    // ** Game Rules Methods
    static void spawnPlayer( PlayerID player );

    static void initializeGameLogic();
    static void reinitializeGameLogic();
    static bool resetGameLogic();
    static bool resetGameLogicAlt();
    static void shutdownGameLogic();

    static void shutdownParticleSystems();

    static bool startClientGameSetup(const NetMessage* message, int *result_code);
    static bool clientGameSetup( int *percent_complete );
    static ConnectMesgServerGameSettings* getServerGameSetup();

    static void cleaning();


public:
    static void exitNetPanzer();

    static void quitNetPanzerGame();

    static void setNetPanzerGameOptions();

    static void requestNetworkPing();

    static void processSystemMessage(const NetMessage* message);

    static void setVideoMode();

    static void loadPalette(const std::string& palette_path );

    static void drawTextCenteredOnScreen(const char *string, unsigned char color);

    static void   startGameTimer();
    static time_t getGameTime();
};

#endif
