/*
Copyright (C) 2003 Matthias Braun <matze@braunis.de>

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


#include "Scripts/ScriptManager.hpp"

#include "lua/lua.hpp"

#ifdef WIN32
#include <windows.h>
#else
#include <sys/types.h>
#include <unistd.h>
#endif

#include <fstream>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <ctype.h>
#include <signal.h>
#include <SDL2/SDL.h>

#include <optionmm/command_line.hpp>
#include "Util/Exception.hpp"
#include "Util/Log.hpp"
//#include "Util/Exception.hpp"
#include "Util/FileSystem.hpp"
#include "Network/NetworkManager.hpp"

#include "Interfaces/BaseGameManager.hpp"
#include "Interfaces/GameConfig.hpp"
#include "Interfaces/DedicatedGameManager.hpp"
#include "Interfaces/BotGameManager.hpp"
#include "Interfaces/PlayerGameManager.hpp"
#include "Core/NetworkGlobals.hpp"
#include "Scripts/ScriptManager.hpp"
#include "2D/Palette.hpp"

#include "Weapons/Weapon.hpp"
#include "PowerUps/PowerUpInterface.hpp"
#include "Particles/PuffParticle2D.hpp"
#include "Particles/CloudParticle2D.hpp"
#include "Particles/FlameParticle2D.hpp"
#include "Particles/FlashParticle2D.hpp"
#include "Particles/ChunkTrajectoryParticle2D.hpp"
#include "Particles/CraterParticle2D.hpp"
#include "Units/UnitProfileInterface.hpp"


#ifndef PACKAGE_VERSION
	#define PACKAGE_VERSION "testing"
#endif

//---------------------------------------------------------------------------

static std::string pidfile;

void shutdown()
{
#ifndef WIN32
    if(pidfile != "") {
        unlink(pidfile.c_str());
    }
#endif

// final cleaning
Weapon::uninit();
PowerUpInterface::uninitialize();
PuffParticle2D::unloadPAKFiles();
CloudParticle2D::uninit();
FlameParticle2D::uninit();
FlashParticle2D::uninit();
ChunkTrajectoryParticle2D::uninit();
CraterParticle2D::uninit();
//
UnitProfileSprites::clearProfiles();
UnitProfileInterface::clearProfiles();
//


    SDL_Quit();
    if(gameconfig)
        gameconfig->saveConfig();
    LOGGER.closeLogFile();
    filesystem::shutdown();
}

#ifndef WIN32
void signalhandler(int signum)
{
    // set signal handler back to default
    signal(signum, SIG_DFL);

    const char* sigtype;
    switch(signum) {
        case SIGINT: sigtype = "SIGINT"; break;
        case SIGHUP: sigtype = "SIGHUP"; break;
        case SIGTERM: sigtype = "SIGTERM"; break;
        case SIGFPE: sigtype = "SIGFPE"; break;
        case SIGILL: sigtype = "SIGILL"; break;
        case SIGSEGV: sigtype = "SIGSEGV"; break;
        case SIGBUS: sigtype = "SIGBUS"; break;
        case SIGABRT: sigtype = "SIGABRT"; break;
        case SIGTRAP: sigtype = "SIGTRAP"; break;
        case SIGSYS: sigtype = "SIGSYS"; break;
        default: sigtype = "UNKNOWN"; break;
    }

    std::cerr << "Received signal " << sigtype << "(" << signum << ")"
        << std::endl << "aborting and trying to shutdown." << std::endl;
    shutdown();
    raise(signum);
}
#endif

//-----------------------------------------------------------------
BaseGameManager *initialise(int argc, char** argv)
{
#if !defined(WIN32) && !defined(DEBUG)
    // Install signal handlers for a clean shutdown
    signal(SIGILL, signalhandler);
    signal(SIGINT, signalhandler);
    signal(SIGHUP, signalhandler);
    signal(SIGTERM, signalhandler);
    signal(SIGFPE, signalhandler);
    signal(SIGSEGV, signalhandler);
    signal(SIGBUS, signalhandler);
    signal(SIGABRT, signalhandler);
    signal(SIGTRAP, signalhandler);
    signal(SIGSYS, signalhandler);
#endif
#if !defined(WIN32)
    signal(SIGPIPE, SIG_IGN);
#endif

    // Parse commandline
    using namespace optionmm;
    command_line commandline("NetPanzer", PACKAGE_VERSION,
            "Copyright(c) 1998 Pyrosoft Inc. & NetPanzer Development Team", "",
            argc, argv);

    option<std::string, true, false> connect_option('c', "connect",
            "Connect to the specified netpanzer server", "");
    commandline.add(&connect_option);
    bool_option dedicated_option('d', "dedicated", "Run as dedicated server", false);
    commandline.add(&dedicated_option);
    option<std::string, true, false> bot_option('b', "bot", "Connect as bot to specific server (dedicated server accepts only localhost)", "");
    commandline.add(&bot_option);

    bool_option need_password(0, "password", "Ask for password when connecting, only useful on quick connect", false);
    commandline.add(&need_password);

    option<int> port_option('p', "port", "Run server on specific port", 0);
    commandline.add(&port_option);
    bool_option debug_option('g', "debug", "Enable debug output", false);
    commandline.add(&debug_option);
    option<std::string, true, false> master_server_option('\0', "master_server", "Use 'none' if you dont want to use the master server", "");
    commandline.add(&master_server_option);
    option<std::string, true, false> game_config_option(0, "game_config",
        "Which config file should be used (example '/config/[name].cfg')",
        "");
    commandline.add(&game_config_option);
    option<bool, false, false> protocol_option('\0', "protocol",
            "Show version of network protocol", false);
    commandline.add(&protocol_option);
    option<std::string, true, false> pidfile_option(0, "pidfile",
            "Create a pidfile at the specified location", "");
    commandline.add(&pidfile_option);

    if(!commandline.process() || commandline.help() || commandline.version())
        exit(0);

    if(protocol_option.value()) {
        std::cout << (int) NETPANZER_PROTOCOL_VERSION << std::endl;
        exit(0);
    }

    if (debug_option.value()) {
        LOGGER.setLogLevel(Logger::LEVEL_DEBUG);
        LOGGER.debug("debug option enabled");
    } else if (dedicated_option.value()) {
        LOGGER.setLogLevel(Logger::LEVEL_INFO);
    }

    // Initialize SDL (don't initialize video or audio for dedicated servers)
    if (dedicated_option.value() && SDL_Init(SDL_INIT_TIMER | SDL_INIT_EVENTS) < 0) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't initialize SDL: %s", SDL_GetError());
        exit(1);
    }

    // Initialize libphysfs
    try {
        filesystem::initialize(argv[0], "netpanzer", "netpanzer");
    } catch(std::exception& e) {
        fprintf(stderr, "%s", e.what());
        shutdown();
        exit(1);
    }

#ifdef NP_DATADIR
    try {
	filesystem::addToSearchPath(NP_DATADIR);
    } catch(...)
    { }
#endif

#ifdef __APPLE__
    // Mac OS X puts the data files into NetPanzer.app/Contents/Resources
    try {
      std::ostringstream dir;
      dir << PHYSFS_getBaseDir() << "/NetPanzer.app/Contents/Resources/";
      filesystem::addToSearchPath(dir.str().c_str());
    } catch(...)
    { }
#endif

    if(dedicated_option.value())
        LOGGER.openLogFile("server");
    else if(bot_option.value().size() > 0)
        LOGGER.openLogFile("bot");
    else
        LOGGER.openLogFile("netpanzer");

#ifdef WIN32
    // SDL redirects stdout and stderr to 2 textfiles, better open a new console
    // for the dedicated server and the bot
    if(dedicated_option.value() || bot_option.value().size() > 0) {
        AllocConsole();
        freopen("CON", "w", stdout);
        freopen("CON", "w", stderr);
        freopen("CON", "r", stdin);
    }
#endif

    // Initialize random number generator
    srand(time(0));
    // the STL functions in gcc3 seem to use the 48er versions of the random
    // generator instead of the default libc one, so we have todo a double srand
    // :-/
#ifdef __USE_SVID
    srand48(time(0));
#endif

    // create a pidfile
    pidfile = pidfile_option.value();
#ifndef WIN32
    if(pidfile != "") {
        std::ofstream out(pidfile.c_str());
        if(!out.good()) {
            fprintf(stderr, "Couldn't create pidfile '%s'.\n", pidfile.c_str());
            shutdown();
            exit(1);
        }
        out << getpid() << std::endl;
        out.close();
    }
#else
    if(pidfile != "") {
        fprintf(stderr, "Pidfiles not supported on win32.\n");
    }
#endif

    BaseGameManager *manager;
    // finally initialize the game objects
    try {
        if (dedicated_option.value()) {
            manager = new DedicatedGameManager();
        }
        else if (!bot_option.value().empty()) {
            manager = new BotGameManager(bot_option.value());
        } else {
            manager = new PlayerGameManager();
        }

        manager->initialize(game_config_option.value());

        // gameconfig exists now...
        if(!connect_option.value().empty()) {
            std::string connecthost = connect_option.value();
            if(connecthost.find("netpanzer://") == 0) {
                connecthost = connecthost.substr(12, connecthost.size()-12);
                std::string::size_type p = connecthost.find('/');
                if(p != std::string::npos) {
                    if ( connecthost[p+1] == 'p' )
                    {
                        gameconfig->needPassword = true;
                    }
                    connecthost = connecthost.substr(0, p);
                }
            }

            if ( need_password.value() )
            {
                gameconfig->needPassword = true;
            }

            gameconfig->serverConnect = connecthost;
            gameconfig->quickConnect = true;
        }
        if (!master_server_option.value().empty()) {
            if (master_server_option.value() == "none") {
                *GameConfig::server_masterservers = "";
            }
            else {
                *GameConfig::server_masterservers = master_server_option.value();
            }
        }
        if ( port_option.value() )
        {
            GameConfig::server_port=port_option.value();
        }

        return manager;
    } catch(std::exception& e) {
        LOGGER.warning("Couldn't initialize the game: %s", e.what());
        shutdown();
        exit(1);
    }
}

//-----------------------------------------------------------------
int netpanzer_main(int argc, char** argv)
{
    network::NetworkManager::initialize();
    ScriptManager::initialize();

    //ScriptManager::runStr("LuaInitialize","print('Lua is working just fine');");

    BaseGameManager *manager = initialise(argc, argv);

    ScriptManager::runFile("unused","scripts/initialize.lua");



    // we'll catch every exception here, to be sure the user gets at least
    // a usefull error message and SDL has a chance to shutdown...
    try {
        if (manager->launchNetPanzerGame()) {
            while(manager->mainLoop())
                ;
        }

        manager->shutdown();
        delete manager;
        LOGGER.info("successfull shutdown.");
        shutdown();
    }
// in debug mode we want the exception to abort, so that we have the original
// stack backtrace
#if !defined(DEBUG) || defined(WIN32)
    catch(std::exception& e) {
        LOGGER.warning("An unexpected exception occured: '%s'\nShutdown needed.",
                e.what());
        shutdown();
        return 1;
    } catch(...) {
        LOGGER.warning("An unexpected exception occured.\nShutdown needed.");
        shutdown();
        return 1;
    }
#else
    catch(const float dummy) {
    }
#endif

    ScriptManager::close();
    network::NetworkManager::cleanUp();
    return 0;
}

// XXX these tweaks are need for SDL on win32
extern "C" {
    int main(int argc, char** argv)
    {
        return netpanzer_main(argc, argv);
    }
}

