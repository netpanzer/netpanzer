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

#include "Interfaces/GameManager.hpp"

#include <stdio.h>
#include <fcntl.h>

#include "System/Sound.hpp"
#include "System/SDLSound.hpp"
#include "System/SDLVideo.hpp"
#include "System/DummySound.hpp"

#include "Interfaces/BaseGameManager.hpp"
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
#include "Classes/Network/ServerConnectDaemon.hpp"
#include "Classes/Network/ServerMessageRouter.hpp"
#include "Classes/Network/NetworkState.hpp"
#include "Classes/Network/NetworkServer.hpp"
#include "Classes/Network/NetworkClient.hpp"
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
#include "Views/MainMenu/CreditsView.hpp"
#include "Views/MainMenu/HelpView.hpp"
#include "Views/MainMenu/Multi/JoinView.hpp"
#include "Views/MainMenu/Multi/HostView.hpp"
#include "Views/MainMenu/Multi/GetSessionView.hpp"
#include "Views/MainMenu/Multi/UnitSelectionView.hpp"
#include "Views/MainMenu/Multi/UnitColorView.hpp"
#include "Views/MainMenu/Multi/HostOptionsView.hpp"
#include "Views/MainMenu/Multi/MapSelectionView.hpp"
#include "Views/MainMenu/Multi/PlayerNameView.hpp"
#include "Views/MainMenu/Multi/IPAddressView.hpp"
#include "Views/MainMenu/Multi/TipsView.hpp"

#include "Views/Game/RankView.hpp"
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
#include "Particles/Particle2D.hpp"
#include "Particles/ParticleSystem2D.hpp"
#include "Particles/ParticleInterface.hpp"
#include "Particles/Physics.hpp"

#include "Util/TimerInterface.hpp"
#include "Util/Math.hpp"
#include "Util/Log.hpp"
#include "Util/Exception.hpp"
#include "Util/FileSystem.hpp"

#include "Bot/Bot.hpp"

#define _MAX_INITIALIZE_PROCS (10)
#define _MAX_DEDICATED_INITIALIZE_PROCS (8)


time_t GameManager::game_start_time = 0;
time_t GameManager::game_elapsed_time_offset = 0;

bool GameManager::display_frame_rate_flag = false;
bool GameManager::display_network_info_flag;

std::string GameManager::map_path;
std::string GameManager::mapstyle_path;

std::string GameManager::stmapstyle;
//std::string GameManager::ststyles;
unsigned char GameManager::ststylesnum;
std::vector<NPString> GameManager::stlist;

static Surface hostLoadSurface;

// ******************************************************************

void GameManager::drawTextCenteredOnScreen(const char *string, PIX color)
{
    Surface text;
    text.renderText(string, color, 0);
    screen->lock();

    screen->fill(0);
    int x = (screen->getWidth() / 2) - (text.getWidth() / 2);
    int y = (screen->getHeight() / 2) - (text.getHeight() / 2);
    text.blt(*screen,x,y);

    screen->unlock();
    screen->render();
}

// ******************************************************************

void GameManager::loadPalette(const std::string& palette_name)
{
    if ( Screen ) {
        Palette::init(palette_name);
        Screen->setPalette(Palette::color);
    }
}

// ******************************************************************

void GameManager::setVideoMode()
{
    iXY mode_res;
    iXY old_res = screen ? iXY(screen->getWidth(), screen->getHeight()): iXY(0,0);

    if ( GameConfig::video_width < 800 )
    {
        GameConfig::video_width = 800;
    }

    if ( GameConfig::video_height < 600 )
    {
        GameConfig::video_height = 600;
    }

    mode_res.x = GameConfig::video_width;
    mode_res.y = GameConfig::video_height;

    Screen->setVideoMode(mode_res.x, mode_res.y, 8, GameConfig::video_fullscreen);

    WorldViewInterface::setCameraSize( mode_res.x, mode_res.y);
    delete screen;
    screen = new ScreenSurface(Screen, mode_res.x, mode_res.y);

    Desktop::checkResolution(old_res, mode_res);
    Desktop::checkViewPositions(mode_res);
    loadPalette("netp");

    if(old_res == iXY(0,0)) {
        // users can get annoyed when they see a black screen for half a minute
        // so we display something here... (we're just hoping that palette1 is
        // not black)
        drawTextCenteredOnScreen("Please wait... generating palettes", 255);
    }

}

// ******************************************************************
void GameManager::initializeGameLogic()
{
    PlayerInterface::initialize(GameConfig::game_maxplayers);
    UnitBlackBoard::initializeBlackBoard();
    UnitInterface::initialize( gameconfig->GetUnitsPerPlayer() );
    PathScheduler::initialize();
    PowerUpInterface::resetLogic();
}

// ******************************************************************
void GameManager::reinitializeGameLogic()
{
    shutdownGameLogic();
    initializeGameLogic();
}

// ******************************************************************
bool GameManager::resetGameLogic()
{
    PlayerInterface::reset();
    UnitInterface::reset();
    UnitBlackBoard::initializeBlackBoard();
    PathScheduler::initialize();
    PowerUpInterface::resetLogic();
    ProjectileInterface::resetLogic();
    startGameTimer();
    return true;
}

// ******************************************************************
bool GameManager::resetGameLogicAlt()
{
    PlayerInterface::reset();
    UnitInterface::reset();
    UnitBlackBoard::initializeBlackBoard();
    PathScheduler::initialize();
    //PowerUpInterface::resetLogic();
    ProjectileInterface::resetLogic();
    startGameTimer();
    return true;
}

// ******************************************************************
void GameManager::shutdownGameLogic()
{
    PlayerInterface::cleanUp();
    UnitInterface::cleanUp();
    PathScheduler::cleanUp();
    //ObjectiveInterface::cleanUp();
}

void GameManager::shutdownParticleSystems()
{
    ParticleSystem2D::removeAll();
    Particle2D::removeAll();
}

// ******************************************************************
void GameManager::startGameMapLoad(const char *map_file_path, const char *mapstyle_name,
                                   unsigned long partitions)
{
    map_path = "maps/";
    map_path.append(map_file_path);
    mapstyle_path = "/";
    mapstyle_path.append(mapstyle_name);

    if (!MapInterface::startMapLoad( map_path.c_str(), mapstyle_path.c_str(), true, partitions))
        throw Exception("map format error.");
}

// ******************************************************************

bool GameManager::gameMapLoad( int *percent_complete )
{
    if( MapInterface::loadMap( percent_complete ) == false ) {
        finishGameMapLoad();
        return false;
    }

    return true;
}

// ******************************************************************

void GameManager::finishGameMapLoad()
{
    std::string temp_path = map_path;
    temp_path.append(".opt");
    ObjectiveInterface::loadObjectiveList( temp_path.c_str() );

    ParticleInterface::addCloudParticle(GameConfig::game_cloudcoverage);
    Physics::wind.setVelocity(GameConfig::game_windspeed, 107);
}

// ******************************************************************

void GameManager::dedicatedLoadGameMap(const char *map_name, const char *mapstyle_name )
{

    Console::mapSwitch(map_name);
    *Console::server        << "Server Settings:\n"
        << "MaxPlayers: "   << GameConfig::game_maxplayers << "\n"
        << "MaxUnits: "     << GameConfig::game_maxunits << "\n"
        << "AutoKick: "     << GameConfig::game_autokicktime << "\n"
        << "FlagTimer: "    << GameConfig::game_changeflagtime << "\n"
        << "GameType: "     << gameconfig->getGameTypeString() << "\n"
        << "ObjectivePercentage: " <<
            GameConfig::game_occupationpercentage << "\n"
        << "TimeLimit: "    << GameConfig::game_timelimit << "\n"
        << "FragLimit: "    << GameConfig::game_fraglimit << "\n"
        << "RespawnType: "  << gameconfig->getRespawnTypeString() << "\n"
        << "Map: "          << *GameConfig::game_map << "\n"
        << "MapCycle: "     << *GameConfig::game_mapcycle << "\n"
        << "MapStyle: "     << *GameConfig::game_mapstyle << "\n"
        << "UnitsStyles: "  << *GameConfig::game_units_styles << "\n"
        << "Powerups: "     << (GameConfig::game_powerups ? "yes" : "no") << "\n"
        << "AllowAllies: "  << (GameConfig::game_allowallies ? "yes" : "no") << "\n"
        << "CloudCoverage: " << GameConfig::game_cloudcoverage << " (Windspeed " << GameConfig::game_windspeed << ")" << std::endl;

    map_path = "maps/";
    map_path.append(map_name);


    mapstyle_path = "/";
    mapstyle_path.append(mapstyle_name);

    MapInterface::startMapLoad( map_path.c_str(), mapstyle_path.c_str(), false, 0 );

    map_path.append(".opt");
    ObjectiveInterface::loadObjectiveList( map_path.c_str() );

    //ParticleInterface::initParticleSystems(); // Warning repetition! breaks surfaces load sequence and causes memory leaks
    Particle2D::setCreateParticles(false);
}

// ******************************************************************
void GameManager::spawnPlayer( PlayerID player )
{
    sound->stopTankIdle();

    // ** Get a new spawn point and spawn the player **
    iXY spawn_point = MapInterface::getFreeSpawnPoint();
    PlayerInterface::spawnPlayer( player, spawn_point );

    //** Change the location of the view camera to the spawn point **
    iXY world_loc;
    MapInterface::mapXYtoPointXY( spawn_point, world_loc );
    if ( PlayerInterface::getLocalPlayerIndex() == player )
    {
        WorldViewInterface::setCameraPosition( world_loc );
    }
    else
    {
        SystemSetPlayerView set_view(world_loc.x, world_loc.y);
        SERVER->sendMessage(player, &set_view, sizeof(SystemSetPlayerView));
    }

    sound->playTankIdle();
}

// ******************************************************************

void GameManager::netMessageSetView(const NetMessage* message)
{
    const SystemSetPlayerView* set_view
        = (const SystemSetPlayerView*) message;

    WorldViewInterface::setCameraPosition(
            iXY(set_view->getCameraLocX(), set_view->getCameraLocY()) );
}

// ******************************************************************

void GameManager::netMessageViewControl(const NetMessage* message)
{
    const SystemViewControl *view_control
        = (const SystemViewControl*) message;

    if ( view_control->action_flags & _view_control_flag_close_all ) {
        Desktop::setVisibilityAllWindows(false);
        Desktop::setVisibility("GameView", true);
    }

    if ( view_control->action_flags & _view_control_flag_visible_on ) {
        Desktop::setVisibility( view_control->view_name, true );
    }

    if ( view_control->action_flags & _view_control_flag_visible_off ) {
        Desktop::setVisibility( view_control->view_name, false );
    }
}

// ******************************************************************
void GameManager::netMessageConnectAlert(const NetMessage* message)
{
    const SystemConnectAlert *connect_alert
        = (const SystemConnectAlert*) message;
    PlayerState *player_state = 0;

    if(connect_alert->getPlayerID() >= PlayerInterface::getMaxPlayers()) {
        LOGGER.warning("Malformed connect alert message.");
        return;
    }

    player_state = PlayerInterface::getPlayer( connect_alert->getPlayerID() );

    switch (connect_alert->alert_enum) {
        case _connect_alert_mesg_connect:
            if (MapInterface::chat_color_scheme == 0) {
            ConsoleInterface::postMessage(Color::cyan, true, player_state->getFlag(),
                                          "%s has joined the game.",
                    player_state->getName().c_str()); } else {
                    ConsoleInterface::postMessage(Color::darkBlue, true, player_state->getFlag(),
                                          "%s has joined the game.",
                    player_state->getName().c_str());
                    }

            break;

        case _connect_alert_mesg_disconnect:
            if (MapInterface::chat_color_scheme == 0) {
            ConsoleInterface::postMessage(Color::cyan, true, player_state->getFlag(),
                                          "%s has left the game.",
                    player_state->getName().c_str()); } else {
                    ConsoleInterface::postMessage(Color::darkBlue, true, player_state->getFlag(),
                                          "%s has left the game.",
                    player_state->getName().c_str());
                    }
            break;

        case _connect_alert_mesg_client_drop:
            if (MapInterface::chat_color_scheme == 0) {
            ConsoleInterface::postMessage(Color::cyan, true, player_state->getFlag(),
                    "Connection to %s has been unexpectedly broken.",
                    player_state->getName().c_str()); } else {
                    ConsoleInterface::postMessage(Color::darkBlue, true, player_state->getFlag(),
                    "Connection to %s has been unexpectedly broken.",
                    player_state->getName().c_str());
                    }
            break;

        case _connect_alert_mesg_client_kicked:
            if (MapInterface::chat_color_scheme == 0) {
            ConsoleInterface::postMessage(Color::cyan, true, player_state->getFlag(),
                "Player %s was kicked.",
                player_state->getName().c_str()); } else {
                ConsoleInterface::postMessage(Color::darkBlue, true, player_state->getFlag(),
                "Player %s was kicked.",
                player_state->getName().c_str());
                }
            break;

        default:
            LOGGER.warning("invalid connection_alert enum found.");
    }
}

// ******************************************************************

void GameManager::netMessageResetGameLogic(const NetMessage* )
{
    resetGameLogic();
}

// ******************************************************************

ConnectMesgServerGameSettings* GameManager::getServerGameSetup()
{
    ConnectMesgServerGameSettings* game_setup
        = new ConnectMesgServerGameSettings();

    game_setup->setMaxPlayers(GameConfig::game_maxplayers);
    game_setup->setMaxUnits(GameConfig::game_maxunits);
    snprintf(game_setup->map_name, 32, "%s", GameConfig::game_map->c_str());
    snprintf(game_setup->map_style, 17, "%s", GameConfig::game_mapstyle->c_str());
    snprintf(game_setup->tank_styles, 176, "%s", GameConfig::game_units_styles->c_str());
    game_setup->setCloudCoverage(GameConfig::game_cloudcoverage);
    game_setup->setWindSpeed(GameConfig::game_windspeed);
    game_setup->setGameType(GameConfig::game_gametype);
    game_setup->powerup_state = GameConfig::game_powerups;
    game_setup->setFragLimit(GameConfig::game_fraglimit);
    game_setup->setTimeLimit(GameConfig::game_timelimit);
    game_setup->setElapsedTime(getGameTime());
    game_setup->setFlagTime(GameConfig::game_changeflagtime);
    game_setup->occupation_percentage = GameConfig::game_occupationpercentage;
    game_setup->enable_bases = GameConfig::game_enable_bases;
    game_setup->base_capture_mode = GameConfig::game_base_capture_mode;
    game_setup->base_limit = GameConfig::game_base_limit;
    game_setup->allowmultiip = GameConfig::game_allowmultiip;
    game_setup->allowallies = GameConfig::game_allowallies;
    game_setup->respawntype = GameConfig::game_respawntype;
    game_setup->setLowScoreLimit(GameConfig::game_lowscorelimit);

    return game_setup;
}

// ******************************************************************

void GameManager::netMessagePingRequest(const NetMessage* message)
{ /*
    const SystemPingRequest *ping_request
        = (const SystemPingRequest*) message;

    if(ping_request->getClientPlayerIndex() >= PlayerInterface::getMaxPlayers())
    {
        LOGGER.warning("Invalid pingRequest message");
        return;
    }

    PlayerState * player;
    player = PlayerInterface::getPlayer( ping_request->getClientPlayerIndex() );

    SystemPingAcknowledge ping_ack;

    if ( player->isActive() )
    {
        SERVER->sendMessage(player->getID(), &ping_ack, sizeof(SystemPingAcknowledge));
    }
    */
}


// ******************************************************************

void GameManager::sendAckEnckeychange(const NetMessage* message)
{
        if ( NetworkState::status != _network_state_server ) // clients only
        {
        const SystemEnckeychange *enc_key = (const SystemEnckeychange*) message;
        unsigned short r_key = enc_key->getEncKey();

        SystemEnckeychangeAck sea(r_key,PlayerInterface::getLocalPlayerIndex());
        //LOGGER.info("Got key %d and sending it back!", r_key);
        NetworkState::ping_time_stamp = now();
        CLIENT->sendMessage( &sea, sizeof(SystemEnckeychangeAck) );
        //requestNetworkPing(); // switch on up_ping
        }
}

// ******************************************************************

void GameManager::netMessagePingAcknowledge(const NetMessage* message)
{
    /*
    NetworkState::ping_time = (now() - NetworkState::ping_time_stamp)*1000;
    //LOGGER.info("Ping is %.3f ms", NetworkState::ping_time);
    PlayerState* player_state;
    player_state = PlayerInterface::getPlayer( PlayerInterface::getLocalPlayerIndex() );

    player_state->setUpLastPing(NetworkState::ping_time);
    if (player_state->getUpAvgPing() > 0) {
    player_state->setUpAvgPing((NetworkState::ping_time+player_state->getUpAvgPing())/2);
    } else {
    player_state->setUpAvgPing(NetworkState::ping_time);
    }
    //LOGGER.info("Player %s last up-ping is %.3f ms", player_state->getName().c_str(), player_state->getUpLastPing());
    //LOGGER.info("Player %s avg up-ping is %.3f ms", player_state->getName().c_str(), player_state->getUpAvgPing());
    */
}
// ******************************************************************

void GameManager::recvAckEnckeychange(const NetMessage* message)
{
        if ( NetworkState::status == _network_state_server ) // server only
        {
        const SystemEnckeychangeAck *ack_enc_key = (const SystemEnckeychangeAck*) message;
        unsigned short r_key = ack_enc_key->getEncKey();
        PlayerID p_id = ack_enc_key->getPlayerID();
        PlayerState * player = 0;
        player = PlayerInterface::getPlayer(p_id);

        if ( p_id < PlayerInterface::getMaxPlayers() && r_key == player->getLastEncKey() ) {

        float downping = (now() - player->getTempTime())*1000;
        player->setDownLastPing(downping);

        if (player->getDownAvgPing() > 0) {
        player->setDownAvgPing( (player->getDownAvgPing()+downping)/2 );
        } else {
        player->setDownAvgPing(downping);
        }

        SystemReEnckeychangeAck srea(downping);
        if ( player->isActive() )
        {
        SERVER->sendMessage(player->getID(), &srea, sizeof(SystemReEnckeychangeAck)); // final reply to allow client to get his own ping and (included in mesg) the server one too
        }
        //LOGGER.info("Player %s last down-ping is %.3f ms", player->getName().c_str(), downping);
        //LOGGER.info("Player %s avg down-ping is %.3f ms", player->getName().c_str(), player->getDownAvgPing());

        } else {
        // network cheating
        unsigned char p_hits = player->getCheatHits();
        if (p_hits < 255) {
        player->setCheatHits(p_hits++);
        }

        }

        }
}

// ******************************************************************

void GameManager::recvReAckEnckeychange(const NetMessage* message)
{
        if ( NetworkState::status != _network_state_server ) // client only
        {
        const SystemReEnckeychangeAck *final_reply = (const SystemReEnckeychangeAck*) message;
        float d_ping = final_reply->getDownPing();

        NetworkState::ping_time = (now() - NetworkState::ping_time_stamp)*1000;

        PlayerState* player_state;
        player_state = PlayerInterface::getPlayer( PlayerInterface::getLocalPlayerIndex() );

        player_state->setUpLastPing(NetworkState::ping_time);
        if (player_state->getUpAvgPing() > 0) {
        player_state->setUpAvgPing((NetworkState::ping_time+player_state->getUpAvgPing())/2);
        } else {
        player_state->setUpAvgPing(NetworkState::ping_time);
        }

        player_state->setDownLastPing(d_ping);
        if (player_state->getDownAvgPing() > 0) {
        player_state->setDownAvgPing( (player_state->getDownAvgPing()+d_ping)/2 );
        } else {
        player_state->setDownAvgPing(d_ping);
        }
        //LOGGER.info("Player %s last up-ping is %.3f ms", player_state->getName().c_str(), player_state->getUpLastPing());
        //LOGGER.info("Player %s avg up-ping is %.3f ms", player_state->getName().c_str(), player_state->getUpAvgPing());
        //LOGGER.info("Player %s last down-ping is %.3f ms", player_state->getName().c_str(), player_state->getDownLastPing());
        //LOGGER.info("Player %s avg down-ping is %.3f ms", player_state->getName().c_str(), player_state->getDownAvgPing());

        }
}
// ******************************************************************

bool GameManager::startClientGameSetup(const NetMessage* message, int *result_code)
{
    const ConnectMesgServerGameSettings* game_setup
        = (const ConnectMesgServerGameSettings*) message;

    GameConfig::game_maxplayers = game_setup->getMaxPlayers();
    GameConfig::game_maxunits = game_setup->getMaxUnits();
    GameConfig::game_cloudcoverage = game_setup->getCloudCoverage();
    GameConfig::game_windspeed = (int) game_setup->getWindSpeed();
    GameConfig::game_powerups = game_setup->powerup_state;
    GameConfig::game_gametype = game_setup->getGameType();
    GameConfig::game_fraglimit = game_setup->getFragLimit();
    GameConfig::game_timelimit = game_setup->getTimeLimit();
    GameConfig::game_changeflagtime = game_setup->getFlagTime();
    GameConfig::game_occupationpercentage = game_setup->occupation_percentage;

    // ready to use in client if needed:
    GameConfig::game_enable_bases = game_setup->enable_bases;
    GameConfig::game_base_capture_mode = game_setup->base_capture_mode;
    GameConfig::game_base_limit = game_setup->base_limit;
    //GameConfig::game_allowmultiip = game_setup->allowmultiip;
    GameConfig::game_allowallies = game_setup->allowallies;
    GameConfig::game_respawntype = game_setup->respawntype;
    GameConfig::game_lowscorelimit = game_setup->getLowScoreLimit();


    stmapstyle = game_setup->map_style;


    NPString stl = game_setup->tank_styles;
    string_to_params(stl, stlist);
    ststylesnum = stlist.size();
    //LOGGER.info("Unit styles received = %d", ststylesnum);

    startGameTimer();
    game_elapsed_time_offset = game_setup->getElapsedTime();

    try {
        startGameMapLoad( game_setup->map_name, game_setup->map_style, 20);
    } catch(std::exception& e) {
        LOGGER.warning("Error while loading map '%s': %s",
                game_setup->map_name, e.what());
        *result_code = _mapload_result_no_map_file;
        return false;
    }

    *result_code = _mapload_result_success;
    return true;
}


// ******************************************************************

bool GameManager::clientGameSetup( int *percent_complete )
{
    if( gameMapLoad( percent_complete ) == false ) {
        reinitializeGameLogic();
        return false;
    }

    return true;
}

// ******************************************************************
void GameManager::processSystemMessage(const NetMessage* message)
{
    switch(message->message_id) {
        case _net_message_id_system_set_view:
            netMessageSetView( message );
            break;

        case _net_message_id_system_view_control:
            netMessageViewControl( message );
            break;

        case _net_message_id_system_reset_game_logic:
            netMessageResetGameLogic( message );
            break;

        case _net_message_id_system_ping_request:
            netMessagePingRequest( message );
            break;

        case _net_message_id_system_ping_ack:
            netMessagePingAcknowledge( message );
            break;

        case _net_message_id_system_connect_alert:
            netMessageConnectAlert( message );
            break;

        case _net_message_id_system_enckeychange:
            sendAckEnckeychange( message );
            break;

        case _net_message_id_system_enckeychange_ack:
            recvAckEnckeychange( message );
            break;

        case _net_message_id_system_re_enckeychange_ack:
            recvReAckEnckeychange( message );
            break;

        default:
            LOGGER.warning("Found unknown SystemNetworkPacket (id %d)",
                    message->message_id);
    }
}

// ******************************************************************
void GameManager::requestNetworkPing()
{
    /*
    SystemPingRequest ping_request(PlayerInterface::getLocalPlayerIndex());

    NetworkState::ping_time_stamp = now();
    CLIENT->sendMessage( &ping_request, sizeof(SystemPingRequest));
    */
}

void GameManager::setNetPanzerGameOptions()
{
    // nothing
}

// ******************************************************************
void GameManager::exitNetPanzer()
{
    quitNetPanzerGame();

    gamemanager->stopMainLoop();
}

void GameManager::quitNetPanzerGame()
{
    if ( NetworkState::status == _network_state_client ) {
        ClientConnectDaemon::shutdownConnectDaemon();
        CLIENT->partServer();
    } else {
        ServerConnectDaemon::shutdownConnectDaemon();
        SERVER->closeSession();

        // hacky...
        PlayerGameManager* playerGameManager
            = dynamic_cast<PlayerGameManager*>(gamemanager);
        if(playerGameManager)
            playerGameManager->quitGame();
    }

    ParticleSystem2D::removeAll();
    Particle2D::removeAll();
    sound->stopTankIdle();
    UnitInterface::reset();
    PlayerInterface::reset();
    ObjectiveInterface::resetLogic();
    GameControlRulesDaemon::setStateServerIdle();
}

void GameManager::startGameTimer()
{
    game_elapsed_time_offset = 0;
    time( &game_start_time );
}

time_t GameManager::getGameTime()
{
    time_t current_time;

    time( &current_time );

    return( (current_time - game_start_time) + game_elapsed_time_offset );
}

void
GameManager::cleaning()
{
stlist.clear();
}

