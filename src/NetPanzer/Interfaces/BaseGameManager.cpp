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

#include "BaseGameManager.hpp"

#include <SDL2/SDL.h>

// ** PObject netPanzer Network Includes
#include "Classes/Network/ClientMessageRouter.hpp"
#include "Classes/Network/ServerConnectDaemon.hpp"
#include "Classes/Network/ServerMessageRouter.hpp"
#include "Classes/Network/NetworkState.hpp"
#include "Classes/Network/NetworkServer.hpp"
#include "Classes/Network/NetworkClient.hpp"

#include "Util/Log.hpp"
#include "Interfaces/GameConfig.hpp"
#include "Interfaces/GameManager.hpp"
#include "System/DummySound.hpp"
#include "Interfaces/PlayerInterface.hpp"
#include "Units/UnitInterface.hpp"
#include "Units/UnitBlackBoard.hpp"
#include "Particles/Particle2D.hpp"
#include "Particles/ParticleSystem2D.hpp"
#include "Particles/ParticleInterface.hpp"
#include "PathScheduler.hpp"
#include "Weapons/ProjectileInterface.hpp"
#include "Units/UnitProfileInterface.hpp"
#include "Objectives/ObjectiveInterface.hpp"
#include "Interfaces/ConsoleInterface.hpp"
#include "MapsManager.hpp"
#include "PowerUps/PowerUpInterface.hpp"
#include "Interfaces/GameControlRulesDaemon.hpp"
#include "Util/Exception.hpp"
#include "Util/FileSystem.hpp"
#include "Resources/ResourceManager.hpp"
#include "Units/UnitGlobals.hpp"
#include "2D/ColorTable.hpp"
#include "2D/Palette.hpp"


//** Physics/Particle Stuff
#include "Particles/Physics.hpp"
#include "Util/TimerInterface.hpp"

#include "Network/NetworkManager.hpp"

#include "Scripts/ScriptManager.hpp"

BaseGameManager* gamemanager = 0;

//------------------------------------------------------------------
BaseGameManager::BaseGameManager()
    : running(true)
{
    assert(gamemanager == 0);
    gamemanager = this;
}

//------------------------------------------------------------------
BaseGameManager::~BaseGameManager()
{
    gamemanager = 0;
}

//-----------------------------------------------------------------
void BaseGameManager::initializeSoundSubSystem()
{
    sound = new DummySound();
}
//-----------------------------------------------------------------
void BaseGameManager::shutdownSoundSubSystem()
{
    if(sound) {
        delete sound;
        sound = 0;
    }
}
//-----------------------------------------------------------------
void BaseGameManager::initializeGameConfig(const std::string& configfile)
{
    if(configfile == "")
        gameconfig = new GameConfig("/config/client.cfg");
    else
        gameconfig = new GameConfig(configfile, false);
}
//-----------------------------------------------------------------
void BaseGameManager::shutdownGameConfig()
{
    if (gameconfig) {
        delete gameconfig;
        gameconfig = 0;
    }
}
//-----------------------------------------------------------------
void BaseGameManager::initializeInputDevices()
{
}
//-----------------------------------------------------------------
void BaseGameManager::shutdownInputDevices()
{}
//-----------------------------------------------------------------
void BaseGameManager::initializeGameObjects()
{
    Physics::init();
    Weapon::init();

    ConsoleInterface::initialize(25);
    PowerUpInterface::initialize();
    ParticleInterface::initParticleSystems();

    GameManager::initializeGameLogic();
}
//-----------------------------------------------------------------
void BaseGameManager::shutdownGameObjects()
{}
//-----------------------------------------------------------------
void BaseGameManager::loadGameData()
{
//    UnitProfileInterface::loadUnitProfiles();
}
//-----------------------------------------------------------------
void BaseGameManager::initializeNetworkSubSystem()
{
    SERVER = new NetworkServer();
    CLIENT = new NetworkClient();

    ServerMessageRouter::initialize();
    ClientMessageRouter::initialize();

    ServerConnectDaemon::initialize( GameConfig::game_maxplayers );

    NetworkState::setNetworkStatus( _network_state_server );
    NetworkState::resetNetworkStats();
}
//-----------------------------------------------------------------
void BaseGameManager::shutdownNetworkSubSystem()
{
    if(SERVER) {
        SERVER->closeSession();
        ServerMessageRouter::cleanUp();
        delete SERVER;
        SERVER = 0;
    }
    if(CLIENT) {
        CLIENT->partServer();
        ClientMessageRouter::cleanUp();
        delete CLIENT;
        CLIENT = 0;
    }
}
//-----------------------------------------------------------------
// boots up netPanzer; initializes all subsystems, game objects etc.
void BaseGameManager::initialize(const std::string& configfile)
{
    try {
        if(!filesystem::exists("config"))
            filesystem::mkdir("config");
        initializeGameConfig(configfile);
        ResourceManager::initialize();

        initializeSoundSubSystem();
        initializeVideoSubSystem();
        initializeGameObjects();
        initializeNetworkSubSystem();
        initializeInputDevices();
    } catch(std::exception& e) {
        LOGGER.warning("Initialisation failed:\n%s", e.what());
        shutdown();
        throw Exception("bootstrap failed.");
    }
}
//-----------------------------------------------------------------
void BaseGameManager::shutdown()
{
    shutdownSubSystems();
}
//-----------------------------------------------------------------
void BaseGameManager::shutdownSubSystems()
{
    GameManager::shutdownGameLogic();
    shutdownNetworkSubSystem();
    shutdownSoundSubSystem();
    shutdownVideoSubSystem();
    shutdownInputDevices();
    ResourceManager::finalize();
    shutdownGameConfig();
}
//-----------------------------------------------------------------
bool BaseGameManager::mainLoop()
{
    TimerInterface::start();
    inputLoop();
    graphicsLoop();
    simLoop();

    sleeping();
    TimerInterface::update();

    return running;
}
//-----------------------------------------------------------------
/**
 * Sleep to make stable FPS and happy CPU.
 */
void
BaseGameManager::sleeping()
{
    static Uint32 nextTime = 0;

    Uint32 now = SDL_GetTicks();
    if (now < nextTime) {
        SDL_Delay(nextTime - now);
    }
    nextTime += TIMEINTERVAL;
}

//-----------------------------------------------------------------
void BaseGameManager::simLoop()
{
    if ( SERVER )
        SERVER->cleanUpClientList();


    if ( NetworkState::status == _network_state_server ) {
        ServerMessageRouter::routeMessages();
    } else {
        ClientMessageRouter::routeMessages();
    }
    NetworkState::updateNetworkStats();

    UnitInterface::updateUnitStatus();

    ProjectileInterface::updateStatus();

    ObjectiveInterface::updateObjectiveStatus();
    PowerUpInterface::updateState();
    PathScheduler::run();


    Physics::sim();

    ParticleSystem2D::simAll();
    Particle2D::simAll();


    ScriptManager::setDoubleValue("game.frametime", TimerInterface::getTimeSlice());
    ScriptManager::runFunction("scriptLoop");

    GameControlRulesDaemon::updateGameControlFlow();
    if ( SERVER )
        SERVER->sendRemaining();
    if ( CLIENT )
        CLIENT->sendRemaining();
}
//-----------------------------------------------------------------
void BaseGameManager::inputLoop()
{
    network::NetworkManager::run();
}
//-----------------------------------------------------------------
void BaseGameManager::graphicsLoop()
{
}

//---------------------------------------------------------------------
void
BaseGameManager::stopMainLoop()
{
    running = false;
}
