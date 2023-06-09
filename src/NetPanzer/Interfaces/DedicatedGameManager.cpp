/*
Copyright (C) 2003 Ivo Danihelka <ivo@danihelka.net>

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

#include "DedicatedGameManager.hpp"

#include <stdio.h>
#include <fcntl.h>
#include <string>
#include <iomanip>
#include "SDL.h"

#include "Interfaces/ChatInterface.hpp"
#include "Interfaces/ConsoleInterface.hpp"
#include "Interfaces/Console.hpp"
#include "Interfaces/GameConfig.hpp"
#include "Interfaces/GameControlRulesDaemon.hpp"
#include "Interfaces/GameManager.hpp"
#include "Interfaces/MapInterface.hpp"
#include "Interfaces/MapsManager.hpp"
#include "Interfaces/Heartbeat.hpp"
#include "Interfaces/InfoSocket.hpp"
#include "Objectives/ObjectiveInterface.hpp"
#include "Interfaces/PlayerInterface.hpp"

#include "Units/UnitProfileInterface.hpp"

#include "Classes/Network/NetworkState.hpp"
#include "Classes/Network/NetworkServer.hpp"

#include "Particles/Particle2D.hpp"
#include "Particles/ParticleInterface.hpp"
#include "Particles/Physics.hpp"

#include "Util/Log.hpp"

#include "Scripts/ScriptManager.hpp"

#ifndef PACKAGE_VERSION
	#define PACKAGE_VERSION "testing"
#endif

DedicatedGameManager::DedicatedGameManager()
    : commandqueue_mutex(0), console(0), heartbeat(0), infosocket(0)
{
    commandqueue_mutex = SDL_CreateMutex();
    Console::initialize();
}

DedicatedGameManager::~DedicatedGameManager()
{
    delete console;
    if ( heartbeat )
        delete heartbeat;
    if ( infosocket )
        delete infosocket;
    Console::shutdown();

    SDL_DestroyMutex(commandqueue_mutex);
    commandqueue_mutex = 0;
}

void DedicatedGameManager::initializeVideoSubSystem()
{
    //LoadingView::setLoadingView(new ConsoleLoadingView());
}

void DedicatedGameManager::shutdownVideoSubSystem()
{
    //LoadingView::deleteLoadingView();
    delete console;
    console = 0;
}

//-----------------------------------------------------------------
void DedicatedGameManager::initializeGameConfig(const std::string& configfile)
{
    if(configfile == "")
        gameconfig = new GameConfig("/config/server.cfg");
    else
        gameconfig = new GameConfig(configfile, false);
}
//-----------------------------------------------------------------
void DedicatedGameManager::initializeInputDevices()
{
    ConsoleInterface::setStdoutPipe(true);
}
//-----------------------------------------------------------------
void DedicatedGameManager::inputLoop()
{
    // handle server commands
    SDL_mutexP(commandqueue_mutex);
    while(!commandqueue.empty()) {
        const ServerCommand& command = commandqueue.front();

        switch(command.type) {
            case ServerCommand::QUIT:
            {
                GameManager::exitNetPanzer();
                break;
            }
            case ServerCommand::CHAT:
            {
                ChatInterface::serversay(command.argument.c_str());
                break;
            }
            case ServerCommand::STATUS:
            {
                //*Console::server
                std::cout
                    << "Server " << *GameConfig::server_name
                    << " version " << PACKAGE_VERSION << " port "
                    << GameConfig::server_port << "\n"
                    << "Map: " << *GameConfig::game_map << "\n"
                    << std::setw(3) << "ID" << " "
                    << std::setw(30) << "Name" << " "
                    << std::setw(4) << "Kill" << " "
                    << std::setw(4) << "Lost" << " "
                    << std::setw(5) << "Score" << " "
                    << std::setw(21) << "IP\n";
                PlayerID i;
                for ( i = 0; i<PlayerInterface::getMaxPlayers(); ++i)
                {
                    PlayerState* playerstate = PlayerInterface::getPlayer(i);
                    if ( playerstate->isActive() )
                    {
                        //*Console::server
                        std::cout
                            << std::setw(3) << static_cast<int>(i) << " "
                            << std::setw(30) << playerstate->getName() << " "
                            << std::setw(4) << playerstate->getKills() << " "
                            << std::setw(4) << playerstate->getLosses() << " "
                            << std::setw(5) << playerstate->getTotal() << " "
                            << std::setw(21)
                            << SERVER->getIP(playerstate->getID())
                            << "\n";
                    }
                }
                //*Console::server << std::flush;
                std::cout << std::flush;
                break;
            }
            case ServerCommand::MAPCHANGE:
                if(!MapsManager::existsMap(command.argument)) {
                    std::cout << "map '" << command.argument
                        << "' doesn't exist." << std::endl;
                    break;
                }

                GameControlRulesDaemon::forceMapChange(command.argument);
                std::cout << "Preparing mapchange..." << std::endl;
                break;
            case ServerCommand::KICK:
                std::stringstream idstream(command.argument);
                PlayerID id = INVALID_PLAYER_ID;
                idstream >> (unsigned short&)id; // XXX KREMOVE
                if(id >= PlayerInterface::getMaxPlayers()) {
                    std::cout << "Unknown player." << std::endl;
                    break;
                }
                //SERVER->kickClient(SERVER->getClientSocketByPlayerIndex(id));
                SERVER->kickClient(SERVER->getClientSocketByPlayerIndex((unsigned short) id));
                break;
        }
        commandqueue.pop();
    }
    SDL_mutexV(commandqueue_mutex);
    BaseGameManager::inputLoop();
}

bool
DedicatedGameManager::mainLoop()
{
    if ( heartbeat )
        heartbeat->checkHeartbeat();

    static NTimer aktimer(10000); //every 10 sec only

    if (aktimer.isTimeOut())
    {
        aktimer.reset();
        PlayerState * player = 0;
        unsigned long max_players;
        max_players = PlayerInterface::getMaxPlayers();
        for (unsigned long i = 0; i < max_players; i++)
        {
            player = PlayerInterface::getPlayer((unsigned short) i);
            if ( player->isActive() )
            {   /*
                if ( player->checkAutokick() )
                {
                    char chat_string[140]; // was 256
                    sprintf(chat_string, "Server kicked '%s' due to inactivity",player->getName().c_str());
                    LOGGER.info("DED: %s", chat_string);
                    ChatInterface::serversay(chat_string);
                    SERVER->kickClient(SERVER->getClientSocketByPlayerIndex((unsigned short) i));
                }
                */

                if ( player->getTotal()<GameConfig::game_lowscorelimit+1 )
                {
                    char chat_string_warning_k[140];
                    sprintf(chat_string_warning_k, "Server kicked '%s' due to noobiness!",player->getName().c_str());
                    LOGGER.info("DED: %s", chat_string_warning_k);
                    ChatInterface::serversay(chat_string_warning_k);
                    SERVER->kickClient(SERVER->getClientSocketByPlayerIndex((unsigned short) i));
                }


            }

        }
    }


    static NTimer aktimer30(16000); //every 16 sec only
    if (aktimer30.isTimeOut())
    {
        aktimer30.reset();
        PlayerState * player = 0;
        unsigned long max_players2;
        max_players2 = PlayerInterface::getMaxPlayers();
        for (unsigned long i = 0; i < max_players2; i++)
        {
            player = PlayerInterface::getPlayer((unsigned short) i);
            if ( player->isActive() )
            {


                if ( player->getTotal()<GameConfig::game_lowscorelimit+6 && player->getTotal()>GameConfig::game_lowscorelimit )
                {
                    char chat_string_warning[140];
                    sprintf(chat_string_warning, "Warning '%s' - players with %i points or less get kicked!", player->getName().c_str(), GameConfig::game_lowscorelimit);
                    //ChatInterface::serversay(chat_string_warning);
                    ChatInterface::serversayTo(player->getID(), chat_string_warning);
                }


            }

        }
    }



    // houserules reminder

    static NTimer aktimerHR(600000); //every 10 mins
    if (aktimerHR.isTimeOut())
    {
        aktimerHR.reset();

        PlayerState * player = 0;
        unsigned long max_players3;
        max_players3 = PlayerInterface::getMaxPlayers();
        for (unsigned long i = 0; i < max_players3; i++)
        {
            player = PlayerInterface::getPlayer((unsigned short) i);
            if ( player->isActive() )
            {

            if ( GameConfig::server_motd->length() > 0 )
            {
                    // to be done: alternate motd for house rules
                    //ChatInterface::serversay(GameConfig::server_motd->c_str());
                    ChatInterface::serversayTo(player->getID(), GameConfig::server_motd->c_str());
            }

            }

         }


    }



    return BaseGameManager::mainLoop();
}


//-----------------------------------------------------------------
void DedicatedGameManager::pushCommand(const ServerCommand& command)
{
    SDL_mutexP(commandqueue_mutex);
    commandqueue.push(command);
    SDL_mutexV(commandqueue_mutex);
}

//-----------------------------------------------------------------
bool DedicatedGameManager::launchNetPanzerGame()
{
    *Console::server << "starting dedicated netPanzer server\n";

    ScriptManager::runFile("server_commands_load","scripts/servercommands.lua");
    ScriptManager::runFile("user_commands_load","scripts/usercommands.lua");



    GameConfig::game_map->assign(MapsManager::getNextMap(""));

    GameManager::dedicatedLoadGameMap(GameConfig::game_map->c_str());

    UnitProfileInterface::loadUnitProfiles();
    ParticleInterface::rebuildUnitParticleData();

    GameManager::reinitializeGameLogic();

    SERVER->openSession();
    SERVER->hostSession();

    GameControlRulesDaemon::setStateServerInProgress();
    GameControlRulesDaemon::setDedicatedServer();
    NetworkState::setNetworkStatus( _network_state_server );

    GameManager::setNetPanzerGameOptions();

    if (GameConfig::game_lowscorelimit > -15)
    {
    GameConfig::game_lowscorelimit = -15;
    }

    if (GameConfig::game_lowscorelimit < -100)
    {
    GameConfig::game_lowscorelimit = -100;
    }

    gameconfig->hostorjoin=_game_session_host;

    Particle2D::setCreateParticles(false);

    *Console::server << "contacting masterserver." << std::endl;
    if( GameConfig::server_public
        && *GameConfig::server_masterservers != "") {
        try {
            if ( infosocket ) {
                delete infosocket;
                infosocket = 0;
            }
            infosocket = new InfoSocket(GameConfig::server_port);
            if ( heartbeat ) {
                delete heartbeat;
                heartbeat = 0;
            }
            heartbeat = new Heartbeat();
        } catch(std::exception& e) {
            LOGGER.warning("heartbeats disabled: %s", e.what());
            if ( infosocket ) {
                delete infosocket;
                infosocket = 0;
            }
            if ( heartbeat ) {
                delete heartbeat;
                heartbeat = 0;
            }
        }
    }

    *Console::server << "game started." << std::endl;

    console = new ServerConsole(this);
    console->startThread();

    GameManager::startGameTimer();
    return true;
}

//---------------------------------------------------------------------------
void
DedicatedGameManager::initializeNetworkSubSystem()
{
    BaseGameManager::initializeNetworkSubSystem();
}

void
DedicatedGameManager::shutdownNetworkSubSystem()
{
    if ( infosocket ) {
        delete infosocket;
        infosocket = 0;
    }

    if ( heartbeat ) {
        delete heartbeat;
        heartbeat=0;
    }
    BaseGameManager::shutdownNetworkSubSystem();
}
