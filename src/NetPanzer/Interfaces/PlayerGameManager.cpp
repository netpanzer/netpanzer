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


#include <exception>

#include "Interfaces/PlayerGameManager.hpp"
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
#include "Interfaces/PlayerGameManager.hpp"
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
#include "Units/UnitGlobals.hpp"

#include "2D/Palette.hpp"
#include "Views/Components/Desktop.hpp"
#include "Views/Components/ViewGlobals.hpp"
#include "Views/MainMenu/MainMenuView.hpp"
#include "Views/MainMenu/OptionsTemplateView.hpp"
#include "Views/MainMenu/OrderingView.hpp"
#include "Views/MainMenu/SkirmishView.hpp"
#include "Views/MainMenu/HelpView.hpp"
#include "Views/MainMenu/CreditsScrollView.hpp"
#include "Views/MainMenu/CreditsView.hpp"
#include "Views/MainMenu/Multi/JoinView.hpp"
#include "Views/MainMenu/Multi/HostView.hpp"
#include "Views/MainMenu/Multi/GetSessionView.hpp"
#include "Views/MainMenu/Multi/UnitSelectionView.hpp"
#include "Views/MainMenu/Multi/UnitColorView.hpp"
#include "Views/MainMenu/Multi/HostOptionsView.hpp"
#include "Views/MainMenu/Multi/MapSelectionView.hpp"
#include "Views/MainMenu/Multi/PlayerNameView.hpp"
#include "Views/MainMenu/Multi/IPAddressView.hpp"
#include "Views/MainMenu/Multi/ServerListView.hpp"
#include "Views/MainMenu/Multi/TipsView.hpp"

#include "Views/Game/RankView.hpp"
#include "Views/Game/EndRoundView.hpp"
#include "Views/Game/GFlagSelectionView.hpp"

#include "Views/Game/UStyleSelectionView.hpp"

#include "Views/Game/VehicleSelectionView.hpp"
#include "Views/Game/CodeStatsView.hpp"
#include "Views/Game/LibView.hpp"
#include "Views/Game/HelpScrollView.hpp"
#include "Views/Game/HelpScrollViewAlt.hpp"
#include "Views/Game/ResignView.hpp"
#include "Views/Game/AreYouSureResignView.hpp"
#include "Views/Game/AreYouSureExitView.hpp"
#include "Views/Game/GameView.hpp"
#include "Views/Game/MiniMapView.hpp"
#include "Views/Game/MiniMapViewAlt.hpp"
#include "Views/Game/DisconectedView.hpp"
#include "Views/Game/LoadingView.hpp"

#include "Particles/Particle2D.hpp"
#include "Particles/ParticleSystem2D.hpp"
#include "Particles/ParticleInterface.hpp"
#include "Particles/Physics.hpp"

#include "System/Sound.hpp"
#include "System/SDLSound.hpp"
#include "System/SDLVideo.hpp"
#include "System/DummySound.hpp"
#include "System/SDLEvents.hpp"

#include "Util/TimerInterface.hpp"
#include "Util/Math.hpp"
#include "Util/Log.hpp"
#include "Util/Exception.hpp"
#include "Util/FileSystem.hpp"
#include "Util/NTimer.hpp"

#include "Bot/Bot.hpp"

#include "Scripts/ScriptManager.hpp"

PlayerGameManager::PlayerGameManager()
    : sdlVideo(0), heartbeat(0), infosocket(0)
{
    //fontManager.loadFont("fixed10", "fonts/fixed10.pcf", 10);

    showNewPanel = false;
}

//-----------------------------------------------------------------
void PlayerGameManager::initializeVideoSubSystem()
{
    LOGGER.info("Initializing video mode");
    sdlVideo = new SDLVideo();
    Screen = sdlVideo;
    initFont();
    GameManager::setVideoMode();
}

//-----------------------------------------------------------------
void PlayerGameManager::shutdownVideoSubSystem()
{
    delete sdlVideo;
    sdlVideo = nullptr;
    Screen = nullptr;

}
//-----------------------------------------------------------------
void PlayerGameManager::initializeSoundSubSystem()
{
    delete sound;
    sound = nullptr;

    LOGGER.info("Initializing sound system.");
    try {
        if ( GameConfig::sound_enable )
            sound = new SDLSound();
    } catch(std::exception& e) {
        LOGGER.warning("Couldn't initialize sound: %s", e.what());
    }

    if(sound == nullptr)
        sound = new DummySound();

    sound->setSoundVolume(GameConfig::sound_effectsvol);

    // start some music
    if ( GameConfig::sound_music )
    {
        sound->playMusic("sound/music/");
        sound->setMusicVolume(GameConfig::sound_musicvol);
    }
}
//-----------------------------------------------------------------
void PlayerGameManager::initializeInputDevices()
{
    MouseInterface::initialize();
}
//-----------------------------------------------------------------
void PlayerGameManager::initializeWindowSubSystem()
{
    Desktop::add(new GameView());
    Desktop::add(new RankView());
    Desktop::add(new EndRoundView());
    Desktop::add(new GFlagSelectionView());
    Desktop::add(new UStyleSelectionView());
    Desktop::add(new VehicleSelectionView());
    Desktop::add(new MiniMapView() );
    Desktop::add(new MiniMapViewAlt() );
    Desktop::add(new CodeStatsView());
    Desktop::add(new LibView());
    Desktop::add(new HelpScrollView());
    Desktop::add(new HelpScrollViewAlt());
    LoadingView *lv = new LoadingView();
    Desktop::add(lv);


    Desktop::add(new MapSelectionView());
    Desktop::add(new MainMenuView());
    Desktop::add(new HelpView());
    Desktop::add(new CreditsScrollView());
    Desktop::add(new CreditsView());
    Desktop::add(new JoinView());
    Desktop::add(new HostView());
    Desktop::add(new GetSessionView());
    Desktop::add(new OptionsTemplateView());
    Desktop::add(new OrderingView());


    Desktop::add(new HostOptionsView());
    Desktop::add(new PlayerNameView());
    Desktop::add(new ResignView());
    Desktop::add(new AreYouSureResignView());
    Desktop::add(new AreYouSureExitView());
    Desktop::add(new DisconectedView());
    Desktop::add(new TipsView());
    Desktop::add(new IPAddressView());
    Desktop::add(new ServerListView());

    Desktop::setVisibilityAllWindows(false);
    Desktop::setVisibility("MainView", true);

    Desktop::checkResolution(iXY(800,600), iXY(screen->getWidth(),screen->getHeight()));
    Desktop::checkViewPositions(iXY(screen->getWidth(),screen->getHeight()));


    if ( gameconfig->quickConnect && gameconfig->needPassword )
    {
        lv->setNeedPassword(true);
    }

    //Test for new UI
    //testpanel = new Panels::TestPanel(iXY(30, 60), &fontManager);
}
//-----------------------------------------------------------------
void PlayerGameManager::inputLoop()
{
    processSystemKeys();
    MouseInterface::manageClickTimer();
    Desktop::manage(MouseInterface::getMouseX(),
               MouseInterface::getMouseY(), MouseInterface::getButtonMask());

    COMMAND_PROCESSOR.updateScrollStatus( MouseInterface::getMousePosition() );

    BaseGameManager::inputLoop();
}
//-----------------------------------------------------------------
void PlayerGameManager::graphicsLoop()
{
    screen->lock();

    Desktop::draw(*screen);

    MouseInterface::draw(*screen);

    screen->unlock();
    screen->render();
}
//-----------------------------------------------------------------
bool PlayerGameManager::launchNetPanzerGame()
{
    initializeWindowSubSystem();
    return true;
}
//-----------------------------------------------------------------
void PlayerGameManager::launchMultiPlayerGame()
{
    if( gameconfig->hostorjoin == _game_session_host ) {
        hostMultiPlayerGame();
    } else {
        joinMultiPlayerGame();
    }
}

//-----------------------------------------------------------------
void PlayerGameManager::shutdownNetworkSubSystem()
{
    if ( infosocket ) {
        delete infosocket;
        infosocket = nullptr;
    }
    if ( heartbeat ) {
        delete heartbeat;
        heartbeat = nullptr;
    }
    BaseGameManager::shutdownNetworkSubSystem();
}

//-----------------------------------------------------------------
void PlayerGameManager::hostMultiPlayerGame()
{
    PlayerState *player_state;
    Timer wait;

    LoadingView::show();
    // refresh the view in each append
    LoadingView::append( "Launching Server ..." );

    ScriptManager::runFile("server_commands_load","scripts/servercommands.lua");
    ScriptManager::runFile("user_commands_load","scripts/usercommands.lua");

    try {
    	if (CLIENT) {
		delete CLIENT;
		CLIENT = nullptr;
	}
	CLIENT = new NetworkClient();
        SERVER->hostSession();

        if ( GameConfig::server_public
             && !GameConfig::server_masterservers->empty() )
        {
            try {
                if ( infosocket ) {
                    delete infosocket;
                    infosocket = nullptr;
                }
                infosocket = new InfoSocket(GameConfig::server_port);
                if ( heartbeat ) {
                    delete heartbeat;
                    heartbeat = nullptr;
                }
                heartbeat = new Heartbeat();
            } catch(std::exception& e) {
                LOGGER.warning("heartbeats disabled: %s", e.what());
                if ( infosocket ) {
                    delete infosocket;
                    infosocket = nullptr;
                }
                if ( heartbeat ) {
                    delete heartbeat;
                    heartbeat = nullptr;
                }
            }
        }
    } catch(std::exception& e) {
        LoadingView::append( "SERVER LAUNCH FAILED" );
        LoadingView::append(e.what());
        wait.changePeriod( 4 );
        while( !wait.count() );

        LoadingView::loadError();
        return;
    }

    // refresh views
    LoadingView::update( "Launching Server ... (100%) " );
    graphicsLoop();

    GameControlRulesDaemon::setStateServerInProgress();
    NetworkState::setNetworkStatus( _network_state_server );

    LoadingView::append( "Loading Game Data ..." );
    graphicsLoop();

    GameConfig::game_map->assign( MapsManager::getNextMap("") );
    const char* mapname = GameConfig::game_map->c_str();
    const char* mapstyle = GameConfig::game_mapstyle->c_str();

    try {
        GameManager::startGameMapLoad(mapname, mapstyle, 20);
    } catch(std::exception& e) {
        LOGGER.warning("Error while loading map '%s': %s", mapname, e.what());
        LoadingView::loadError();
        return;
    }

    int percent_complete;
    char strbuf[256];

    ObjectiveInterface::resetLogic();

    while( GameManager::gameMapLoad( &percent_complete ) == true ) {
        sprintf( strbuf, "Loading Game Data ... (%d%%)", percent_complete);
        LoadingView::update( strbuf );
        graphicsLoop();
    }

    UnitProfileInterface::loadUnitProfiles();
    if ( UnitProfileInterface::getNumUnitTypes() == 0 )
    {
        LoadingView::append( "ERROR loading unit profiles, check your configuration" );
        LoadingView::append( "game.unit_profiles is: " + *GameConfig::game_unit_profiles );
        graphicsLoop();
        SDL_Delay(5000);
        if ( infosocket )
        {
            delete infosocket;
            infosocket = nullptr;
        }
        if ( heartbeat )
        {
            delete heartbeat;
            heartbeat = nullptr;
        }
        if (CLIENT)
        {
            delete CLIENT;
            CLIENT=nullptr;
        }

        SERVER->closeSession();

        GameControlRulesDaemon::setStateServerIdle();
        LoadingView::loadError();
        return;
    }

    sprintf( strbuf, "Loading Game Data ... (%d%%)", percent_complete);
    LoadingView::update( strbuf );
    graphicsLoop();


    LoadingView::append( "Initializing Game Logic ..." );
    graphicsLoop();
    GameManager::reinitializeGameLogic();

    LoadingView::update( "Initializing Game Logic ... (100%) " );
    graphicsLoop();

    LoadingView::append( "Spawning Player ..." );
    graphicsLoop();

    player_state = PlayerInterface::allocateLoopBackPlayer();
    const char* playername = GameConfig::player_name->c_str();
    player_state->setName(playername);
    //temp fix
    player_state->setPlayerStyle(0);
    //
    LoadingView::update( "Spawning Player ... (100%)" );

    graphicsLoop();

    wait.changePeriod( 3 );
    //while( !wait.count() );

    GameManager::startGameTimer();

    LoadingView::loadFinish();
}

void PlayerGameManager::quitGame()
{
    if ( infosocket ) {
        delete infosocket;
        infosocket = nullptr;
    }
    if ( heartbeat ) {
        delete heartbeat;
        heartbeat = nullptr;
    }
}

//-----------------------------------------------------------------
void PlayerGameManager::joinMultiPlayerGame()
{
    GameManager::setNetPanzerGameOptions();
    //reinitializeGameLogic();
    NetworkState::setNetworkStatus( _network_state_client );

    LoadingView::show();
    LoadingView *lv = static_cast<LoadingView*>(Desktop::getView("LoadingView"));
    if ( ! lv->doesNeedPassword() )
    {
        CLIENT->joinServer(gameconfig->serverConnect, "");
        ClientConnectDaemon::startConnectionProcess();
        sound->playTankIdle();
    }

    ScriptManager::runFile("user_commands_load","scripts/usercommands.lua");
}

bool PlayerGameManager::mainLoop()
{
    // this is a bit ugly...
    static bool firstrun = true;
    if(firstrun) {
        firstrun = false;
        if(gameconfig->quickConnect == true) {
            LoadingView::show();
            launchMultiPlayerGame();
        }
    }

    if ( heartbeat )
        heartbeat->checkHeartbeat();

    // handle SDL Events
    if(handleSDLEvents())
    {
        // We got SDL_Quit, lets disconnect nicely.
        GameManager::exitNetPanzer();
        return false;
    }

    if ( NetworkState::getNetworkStatus() == _network_state_server )
    {
        static NTimer aktimer(10000); //all 10 sec only
        if (aktimer.isTimeOut())
        {
            aktimer.reset();
            PlayerState * player = nullptr;
            unsigned long max_players;
            max_players = PlayerInterface::getMaxPlayers();
            for (unsigned long i = 0; i < max_players; i++)
            {
                if ( i != PlayerInterface::getLocalPlayerIndex() )
                {
                    player = PlayerInterface::getPlayer((unsigned short) i);
                    if ( player->isActive() )
                    {
                        if ( player->checkAutokick() )
                        {
                            char chat_string[256];
                            sprintf(chat_string, "Server kicked '%s' due to inactivity",player->getName().c_str());
                            LOGGER.info("PSE: %s", chat_string);
                            ChatInterface::serversay(chat_string);
                            SERVER->kickClient(SERVER->getClientSocketByPlayerIndex((unsigned short) i));
                        }
                    }
                }
            }
        }
    }

    return BaseGameManager::mainLoop();
}

//-----------------------------------------------------------------
void PlayerGameManager::processSystemKeys()
{
    if (Desktop::getVisible("GameView"))
    {

        if (KeyboardInterface::getKeyState( SDLK_LALT ) ||
                KeyboardInterface::getKeyState( SDLK_RALT )) {
            if (KeyboardInterface::getKeyPressed(SDLK_RETURN)) {
                GameConfig::video_fullscreen = !GameConfig::video_fullscreen;
                GameManager::setVideoMode();
            }
        } // ** LFT_ALT or RGT_ALT pressed

        if ( KeyboardInterface::getKeyState(SDLK_F9) )
        {
            Screen->doScreenshot();
        }

        if (KeyboardInterface::getKeyPressed(SDLK_ESCAPE)) {
            if (!Desktop::getView("OptionsView")->getVisible())
            {
                if ( Desktop::getVisible("HelpScrollViewAlt") )
                {
                    Desktop::toggleVisibility("HelpScrollViewAlt");
                }
                else if ( Desktop::getVisible("GFlagSelectionView") )
                {
                    Desktop::toggleVisibility("GFlagSelectionView");
                }
                else if ( Desktop::getVisible("UStyleSelectionView") )
                {
                    Desktop::toggleVisibility("UStyleSelectionView");
                }
                else
                {
                    View *v = Desktop::getView("OptionsView");
                    assert(v != 0);
                    ((OptionsTemplateView *)v)->initButtons();
                    ((OptionsTemplateView *)v)->setAlwaysOnBottom(false);

                    Desktop::setVisibility("OptionsView", true);
                    Desktop::setActiveView("OptionsView");
                }
            }
            else
            {
                View *v = Desktop::getView("OptionsView");
                assert(v != 0);
                ((OptionsTemplateView *)v)->setAlwaysOnBottom(true);
                ((OptionsTemplateView *)v)->setVisible(false);
            }
        }
    }
}
