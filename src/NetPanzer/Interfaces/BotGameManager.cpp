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



#include "BotGameManager.hpp"

#include "Util/Exception.hpp"
#include <ctype.h>
/*
###
*/

#include <exception>

#include "Interfaces/BotGameManager.hpp"
#include "Interfaces/BaseGameManager.hpp"
#include "Interfaces/GameManager.hpp"
#include "Interfaces/ChatInterface.hpp"
#include "Interfaces/Console.hpp"
#include "Interfaces/ConsoleInterface.hpp"
#include "Interfaces/MouseInterface.hpp"
#include "Interfaces/KeyboardInterface.hpp"
#include "Interfaces/GameConfig.hpp"
#include "Interfaces/MapInterface.hpp"
#include "Interfaces/MapsManager.hpp"
#include "Objectives/ObjectiveInterface.hpp"
#include "Interfaces/PathScheduler.hpp"
#include "Interfaces/BotGameManager.hpp"
#include "Interfaces/PlayerInterface.hpp"
#include "PowerUps/PowerUpInterface.hpp"
#include "Weapons/ProjectileInterface.hpp"
#include "Interfaces/TileInterface.hpp"
#include "Units/UnitInterface.hpp"
#include "Units/UnitProfileInterface.hpp"
#include "Interfaces/WorldViewInterface.hpp"

#include "Classes/ScreenSurface.hpp"
#include "Units/UnitBlackBoard.hpp"
#include "Classes/WorldInputCmdProcessor.hpp"
#include "Classes/SpriteSorter.hpp"
#include "Classes/Network/ClientMessageRouter.hpp"
#include "Classes/Network/ClientConnectDaemon.hpp"
#include "Classes/Network/ServerMessageRouter.hpp"
#include "Classes/Network/NetworkServer.hpp"
#include "Classes/Network/NetworkClient.hpp"
#include "Classes/Network/NetworkState.hpp"
#include "Classes/Network/SystemNetMessage.hpp"
#include "Classes/Network/ConnectNetMessage.hpp"
#include "Classes/Network/PlayerNetMessage.hpp"
#include "Units/UnitGlobals.hpp"


#include "Util/TimerInterface.hpp"
#include "Util/Math.hpp"
#include "Util/Log.hpp"
#include "Util/Exception.hpp"
#include "Util/FileSystem.hpp"
#include "Util/NTimer.hpp"

#include "Bot/Bot.hpp"
#include "Bot/BotPlayer.hpp"
//#include "Bot/BotTask.hpp"
//#include "Bot/BotTaskList.hpp"

#include "Scripts/ScriptManager.hpp"


BotGameManager::BotGameManager(const std::string &serverHost)
    : m_serverHost(serverHost)
{
    /* empty */
}



/**
 * Bot uses name = playername + "-" + rand() % 1000
 */
void BotGameManager::initializeGameConfig(const std::string& configfile)
{
    if(configfile == "")
        gameconfig = new GameConfig("/config/bot.cfg");
    else
        gameconfig = new GameConfig(configfile, false);
}












void BotGameManager::initializeVideoSubSystem()
{
//LoadingView::setLoadingView(new ConsoleLoadingView());
}

void BotGameManager::shutdownVideoSubSystem()
{
 //LoadingView::deleteLoadingView();
}
/*
void BotGameManager::initializeInputDevices()
{

}

void BotGameManager::shutdownInputDevices()
{

}
*/


void BotGameManager::initializeInputDevices()
{
    Bot::initialize(new BotPlayer());
}
//-----------------------------------------------------------------
void BotGameManager::shutdownInputDevices()
{
    Bot::shutdown();
}
//-----------------------------------------------------------------





//-----------------------------------------------------------------


void BotGameManager::inputLoop()
{
    if (GameConfig::bot_class < 1 || GameConfig::bot_class > 9) {
        GameConfig::bot_class = 1;
    }

    switch (GameConfig::bot_class) {

    case 1: {
            Bot::bot()->processEvents();
        }
        break;
    case 2: {
            Bot::bot()->processEvents2();
        }
        break;
    case 3: {
            Bot::bot()->processEvents3();
        }
        break;
    case 4: {
            Bot::bot()->processEvents4();
        }
        break;
    case 5: {
            Bot::bot()->processEvents5();
        }
        break;
    case 6: {
            Bot::bot()->processEvents6();
        }
        break;
    case 7: {
            Bot::bot()->processEvents7();
        }
        break;
    case 8: {
            Bot::bot()->processEvents8();
        }
        break;
    case 9: {
            Bot::bot()->processEvents9();
        }
        break;

    }

     //return true;
     BaseGameManager::inputLoop();
}


//-----------------------------------------------------------------
bool BotGameManager::launchNetPanzerGame()
{



    GameManager::setNetPanzerGameOptions();
    //reinitializeGameLogic();
    NetworkState::setNetworkStatus( _network_state_bot );



       // CLIENT->joinServer(gameconfig->serverConnect, "");
       // ClientConnectDaemon::startConnectionProcess();

    if (!CLIENT->joinServer(m_serverHost.c_str(), "")) {
        return false;
    }
/*
//LOGGER.info("serverhost = %s", m_serverHost.c_str());
NPString::size_type the_end = m_serverHost.find_first_of(":", 0);
NPString nh = m_serverHost.substr(0, the_end);
//LOGGER.info("serverhost = %s", nh.c_str());

    if (nh != "127.0.0.1") {
        return false;
    }
*/

    ClientConnectDaemon::startConnectionProcess();


    //Bot::bot()->processEvents();
    //BotPlayer::processEvents();
    //BotGameManager::botinputLoop();




    //ScriptManager::runFile("user_commands_load","scripts/usercommands.lua");

    return true;
}


//-----------------------------------------------------------------








