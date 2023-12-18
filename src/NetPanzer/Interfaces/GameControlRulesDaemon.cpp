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

#include "Interfaces/GameControlRulesDaemon.hpp"

#include "Interfaces/GameManager.hpp"
#include "Interfaces/GameConfig.hpp"
#include "Interfaces/PlayerInterface.hpp"
#include "Objectives/ObjectiveInterface.hpp"
#include "Interfaces/MapsManager.hpp"
#include "Interfaces/ConsoleInterface.hpp"
#include "Interfaces/ChatInterface.hpp"
#include "PowerUps/PowerUpInterface.hpp"
#include "Classes/Network/NetworkClient.hpp"
#include "Classes/Network/NetworkState.hpp"
#include "Classes/Network/SystemNetMessage.hpp"
#include "Classes/Network/GameControlNetMessage.hpp"
#include "Classes/Network/NetworkServer.hpp"
#include "Classes/Network/ServerConnectDaemon.hpp"
#include "Classes/Network/PlayerNetMessage.hpp"
#include "Units/UnitProfileInterface.hpp"

#include "Views/Components/Desktop.hpp"
#include "Views/Game/LoadingView.hpp"

#include "Util/Log.hpp"

#include "Particles/Particle2D.hpp"
#include "Particles/ParticleSystem2D.hpp"
#include "Particles/ParticleInterface.hpp"

#include "Bot/BotPlayer.hpp"




enum { _map_cycle_server_state_idle,
       _map_cycle_server_state_display_endgame_views,
       _map_cycle_server_state_cycle_next_map,
       _map_cycle_server_state_load_map,
       _map_cycle_server_state_wait_for_client_map_load,
       _map_cycle_server_state_load_unit_profiles,
       _map_cycle_server_state_sync_profiles,
       _map_cycle_server_state_respawn_players
     };

enum { _map_cycle_client_idle,
       _map_cycle_client_start_map_load,
       _map_cycle_client_load_map,
       _map_cycle_client_wait_for_respawn_ack
     };

enum { _execution_mode_loop_back_server,
       _execution_mode_dedicated_server
     };
enum { _game_state_idle,
       _game_state_in_progress,
       _game_state_completed
     };

int GameControlRulesDaemon::execution_mode = _execution_mode_loop_back_server;
unsigned char GameControlRulesDaemon::game_state  = _game_state_idle;
std::string GameControlRulesDaemon::nextmap = "";

#define _MAP_CYCLE_ENDGAME_WAIT_PERIOD  (20) // seconds
#define _MAP_CYCLE_MAP_LOAD_WAIT_PERIOD (7) // seconds

int GameControlRulesDaemon::map_cycle_fsm_server_state = _map_cycle_server_state_idle;
Timer GameControlRulesDaemon::map_cycle_fsm_server_endgame_timer;
Timer GameControlRulesDaemon::map_cycle_fsm_server_map_load_timer;

int GameControlRulesDaemon::map_cycle_fsm_client_state = _map_cycle_client_idle;
bool GameControlRulesDaemon::map_cycle_fsm_client_respawn_ack_flag = false;
char GameControlRulesDaemon::map_cycle_fsm_client_map_name[256];

static unsigned int sync_profile_index = 0;

//-----------------------------------------------------------------
void GameControlRulesDaemon::setStateServerInProgress()
{
    GameControlRulesDaemon::game_state = _game_state_in_progress;
}
//-----------------------------------------------------------------
void GameControlRulesDaemon::setStateServerIdle()
{
    GameControlRulesDaemon::game_state = _game_state_idle;
}
//-----------------------------------------------------------------
void GameControlRulesDaemon::setDedicatedServer()
{
    GameControlRulesDaemon::execution_mode = _execution_mode_dedicated_server;
}

//-----------------------------------------------------------------
void GameControlRulesDaemon::mapCycleFsmClient()
{
    switch( map_cycle_fsm_client_state ) {
        case _map_cycle_client_idle :
            return;

        case _map_cycle_client_start_map_load : {
                LoadingView::show();

                GameManager::shutdownParticleSystems();
                ObjectiveInterface::resetLogic();

                GameConfig::game_map->assign( map_cycle_fsm_client_map_name );

                char buf[256];
                snprintf(buf, sizeof(buf), "Next Map '%s'.",
                        GameConfig::game_map->c_str());
                LoadingView::append( buf);
                LoadingView::append( "Loading Game Map ..." );

                try {
                    const char* nustyle = GameManager::stmapstyle.c_str();
                    GameManager::startGameMapLoad(GameConfig::game_map->c_str(), nustyle, 20);
                } catch(std::exception& e) {
                    LoadingView::append("Error while loading map or mapstyle:");
                    LoadingView::append(e.what());
                    map_cycle_fsm_client_state = _map_cycle_client_idle;
                    return;
                }

                GameManager::resetGameLogic();
                map_cycle_fsm_client_state = _map_cycle_client_load_map;
                return;
            }
            break;

        case _map_cycle_client_load_map : {
                int percent_complete;
                char str_buf[128];

                if ( GameManager::gameMapLoad( &percent_complete ) == false ) {
                    map_cycle_fsm_client_state = _map_cycle_client_wait_for_respawn_ack;

                    sprintf( str_buf, "Loading Game Map ... (%d%%)", percent_complete);
                    LoadingView::update( str_buf );

                    LoadingView::append( "Waiting to respawn ..." );
                } else {
                    sprintf( str_buf, "Loading Game Map ... (%d%%)", percent_complete);
                    LoadingView::update( str_buf );
                }

                return;
            }

        case _map_cycle_client_wait_for_respawn_ack : {
                if( map_cycle_fsm_client_respawn_ack_flag == true ) {
                    LoadingView::loadFinish();
                    map_cycle_fsm_client_respawn_ack_flag = false;
                    map_cycle_fsm_client_state = _map_cycle_client_idle;
                }

                return;
            }
            break;

    } // ** switch
}

///////////////////////////////////////////////

void GameControlRulesDaemon::mapCycleFsmBot()
{
    switch( map_cycle_fsm_client_state ) {
        case _map_cycle_client_idle :
            return;

        case _map_cycle_client_start_map_load : {
                //LoadingView::show();

                GameManager::shutdownParticleSystems();
                ObjectiveInterface::resetLogic();

                GameConfig::game_map->assign( map_cycle_fsm_client_map_name );

                char buf[256];
                snprintf(buf, sizeof(buf), "Next Map '%s'.",
                        GameConfig::game_map->c_str());
                //LoadingView::append( buf);
                //LoadingView::append( "Loading Game Map ..." );

                try {
                    const char* nustyle = GameManager::stmapstyle.c_str();
                    GameManager::startGameMapLoad(GameConfig::game_map->c_str(), nustyle, 20);
                } catch(std::exception& e) {
                    //LoadingView::append("Error while loading map:");
                    //LoadingView::append(e.what());
                    map_cycle_fsm_client_state = _map_cycle_client_idle;
                    return;
                }

                GameManager::resetGameLogic();
                map_cycle_fsm_client_state = _map_cycle_client_load_map;
                return;
            }
            break;

        case _map_cycle_client_load_map : {
                int percent_complete;
                char str_buf[128];

                if ( GameManager::gameMapLoad( &percent_complete ) == false ) {
                    map_cycle_fsm_client_state = _map_cycle_client_wait_for_respawn_ack;

                    sprintf( str_buf, "Loading Game Map ... (%d%%)", percent_complete);
                    //LoadingView::update( str_buf );

                    //LoadingView::append( "Waiting to respawn ..." );
                } else {
                    sprintf( str_buf, "Loading Game Map ... (%d%%)", percent_complete);
                    //LoadingView::update( str_buf );
                }

                return;
            }

        case _map_cycle_client_wait_for_respawn_ack : {
                if( map_cycle_fsm_client_respawn_ack_flag == true ) {

                // resetting some bot stuff
                BotPlayer::bot_objs = 0;
                BotPlayer::idle_units = true;
                BotPlayer::bot_status = 0;
                BotPlayer::state_change = 0;
                BotPlayer::threat_level = 0;

                BotPlayer::inrange = false;
                BotPlayer::touch_flag = false;


                // bots automatic ally request renew mgmt
        if (NetworkState::status == _network_state_bot && GameConfig::bot_allied == true && GameConfig::game_allowallies == true) {

        PlayerState * playerx = 0;
        unsigned int localplayer = PlayerInterface::getLocalPlayerIndex();
        unsigned long max_players;
        max_players = PlayerInterface::getMaxPlayers();
        for (unsigned long i = 0; i < max_players; i++)
        {
            playerx = PlayerInterface::getPlayer((unsigned short) i);

            if (playerx->isActive()) {

            if ( playerx->getID() != localplayer && playerx->getClientType() == 2)
            {
            PlayerAllianceRequest allie_request;
            allie_request.set( localplayer, playerx->getID(), _player_make_alliance);
            CLIENT->sendMessage( &allie_request, sizeof(PlayerAllianceRequest));
            }

            }
        }

        }
        //
                    //LoadingView::loadFinish();
                    map_cycle_fsm_client_respawn_ack_flag = false;
                    map_cycle_fsm_client_state = _map_cycle_client_idle;

                }

                return;
            }
            break;

    } // ** switch
}

////////////////////////////////////////////////////

void GameControlRulesDaemon::mapCycleFsmServer()
{
    switch( map_cycle_fsm_server_state ) {
        case _map_cycle_server_state_idle:
            break;

        case _map_cycle_server_state_display_endgame_views: {
                ChatInterface::serversay("Round is over");

                SystemViewControl view_control;

                ServerConnectDaemon::lockConnectProcess();

                view_control.set("EndRoundView", _view_control_flag_visible_on | _view_control_flag_close_all );

                if ( GameControlRulesDaemon::execution_mode == _execution_mode_loop_back_server ) {
                    Desktop::setVisibility("GameView", true);
                    Desktop::setVisibility("EndRoundView", true );
                }

                SERVER->broadcastMessage(&view_control, sizeof(SystemViewControl));

                map_cycle_fsm_server_endgame_timer.changePeriod( _MAP_CYCLE_ENDGAME_WAIT_PERIOD );
                map_cycle_fsm_server_endgame_timer.reset();

                map_cycle_fsm_server_state = _map_cycle_server_state_cycle_next_map;
            }
            break;

        case _map_cycle_server_state_cycle_next_map : {
                if ( map_cycle_fsm_server_endgame_timer.count() &&
                        (ServerConnectDaemon::isConnecting() == false)
                   ) {
                    GameManager::shutdownParticleSystems();

                    if(nextmap != "") {
                        GameConfig::game_map->assign( nextmap );
                        nextmap = "";
                    } else {
                        GameConfig::game_map->assign( MapsManager::getNextMap( *GameConfig::game_map ) );
                    }

                    ConsoleInterface::postMessage(Color::white, false, 0, "loading map '%s'.",
                            GameConfig::game_map->c_str());

                    GameControlCycleMap cycle_map_mesg;
                    cycle_map_mesg.set( GameConfig::game_map->c_str() );

                    SERVER->broadcastMessage( &cycle_map_mesg, sizeof( GameControlCycleMap ));

                    if ( GameControlRulesDaemon::execution_mode == _execution_mode_dedicated_server ) {
                        ObjectiveInterface::resetLogic();

                        GameManager::dedicatedLoadGameMap(
                                GameConfig::game_map->c_str(), GameConfig::game_mapstyle->c_str() );

                        GameManager::resetGameLogicAlt();

                        map_cycle_fsm_server_map_load_timer.changePeriod(_MAP_CYCLE_MAP_LOAD_WAIT_PERIOD);
                        map_cycle_fsm_server_map_load_timer.reset();
                        map_cycle_fsm_server_state = _map_cycle_server_state_wait_for_client_map_load;

                        return;
                    } else {
                        LoadingView::show();

                        LoadingView::append( "Loading Game Map ..." );
                        ObjectiveInterface::resetLogic();

                        try {
                            GameManager::startGameMapLoad
                                (GameConfig::game_map->c_str(), GameConfig::game_mapstyle->c_str(), 20);
                        } catch(std::exception& e) {
                            LoadingView::append(
                                    "Error while loading map or mapstyle:");
                            LoadingView::append(e.what());
                            map_cycle_fsm_server_state = _map_cycle_server_state_idle;
                            return;
                        }

                        GameManager::resetGameLogic();
                        map_cycle_fsm_server_state = _map_cycle_server_state_load_map;
                        return;
                    }

                }
            }
            break;

        case _map_cycle_server_state_load_map : {
                int percent_complete;
                char str_buf[128];

                if ( GameManager::gameMapLoad( &percent_complete ) == false ) {
                    map_cycle_fsm_server_state = _map_cycle_server_state_load_unit_profiles;
                }

                sprintf( str_buf, "Loading Game Map ... (%d%%)", percent_complete);
                LoadingView::update( str_buf );
            }
            break;

        case _map_cycle_server_state_wait_for_client_map_load : {
                if ( map_cycle_fsm_server_map_load_timer.count() ) {
                    ConsoleInterface::postMessage(Color::white, false, 0, "game started.");
                    map_cycle_fsm_server_state = _map_cycle_server_state_load_unit_profiles;

                }
            }
            break;

        case _map_cycle_server_state_load_unit_profiles: {
                UnitProfileInterface::cleaning();
                UnitProfileSprites::clearProfiles();
                UnitProfileInterface::clearProfiles();
                UnitProfileInterface::loadUnitProfiles();
                Uint8 data[ _MAX_NET_PACKET_SIZE ];
                NetMessage *pmsg = (NetMessage*)&data;
                int len = UnitProfileInterface::fillProfileResetMessage(pmsg);
                SERVER->broadcastMessage(pmsg, len);
                sync_profile_index = 0;
                map_cycle_fsm_server_state = _map_cycle_server_state_sync_profiles;
            }
            break;

        case _map_cycle_server_state_sync_profiles : {
                if ( sync_profile_index <= UnitProfileInterface::getNumUnitTypes() )
                {
                    Uint8 data[ _MAX_NET_PACKET_SIZE ];
                    NetMessage *pmsg = (NetMessage*)&data;
                    int len = UnitProfileInterface::fillProfileSyncMessage(pmsg, sync_profile_index);
                    SERVER->broadcastMessage(pmsg, len);
                    sync_profile_index++;
                }
                if ( sync_profile_index >= UnitProfileInterface::getNumUnitTypes() )
                {

                    map_cycle_fsm_server_state = _map_cycle_server_state_respawn_players;
                }
            }
            break;


        case _map_cycle_server_state_respawn_players : {

                SystemResetGameLogic reset_game_logic_mesg;
                GameManager::resetGameLogic();

                SERVER->broadcastMessage( &reset_game_logic_mesg, sizeof(SystemResetGameLogic));

                PowerUpInterface::syncPowerUpsBC();

                PlayerInterface::unlockPlayerStats();
                GameControlRulesDaemon::game_state = _game_state_in_progress;

                if ( GameControlRulesDaemon::execution_mode == _execution_mode_dedicated_server )
                {
                LoadingView::loadFinish_d(); // to prevent crash on restart
                }
                else
                {
                LoadingView::loadFinish();
                }

                GameControlCycleRespawnAck respawn_ack_mesg;
                SERVER->broadcastMessage( &respawn_ack_mesg, sizeof(GameControlCycleRespawnAck));

                map_cycle_fsm_server_state = _map_cycle_server_state_idle;

                ServerConnectDaemon::unlockConnectProcess();
            }
            break;

    } // ** switch
}

void GameControlRulesDaemon::onTimelimitGameCompleted()
{
    PlayerInterface::lockPlayerStats();

    map_cycle_fsm_server_state = _map_cycle_server_state_display_endgame_views;
    GameControlRulesDaemon::game_state = _game_state_completed;
}

void GameControlRulesDaemon::onFraglimitGameCompleted()
{
    PlayerInterface::lockPlayerStats();

    map_cycle_fsm_server_state = _map_cycle_server_state_display_endgame_views;
    GameControlRulesDaemon::game_state = _game_state_completed;
}

void GameControlRulesDaemon::onObjectiveGameCompleted()
{
    PlayerInterface::lockPlayerStats();

    map_cycle_fsm_server_state = _map_cycle_server_state_display_endgame_views;
    GameControlRulesDaemon::game_state = _game_state_completed;
}

void GameControlRulesDaemon::onObjectiveANDFraglimitGameCompleted()
{
    PlayerInterface::lockPlayerStats();

    map_cycle_fsm_server_state = _map_cycle_server_state_display_endgame_views;
    GameControlRulesDaemon::game_state = _game_state_completed;
}

void GameControlRulesDaemon::onFraglimitORTimelimitGameCompleted()
{
    PlayerInterface::lockPlayerStats();

    map_cycle_fsm_server_state = _map_cycle_server_state_display_endgame_views;
    GameControlRulesDaemon::game_state = _game_state_completed;
}

void GameControlRulesDaemon::forceMapChange(std::string _nextmap)
{
    nextmap = _nextmap;

    PlayerInterface::lockPlayerStats();

    map_cycle_fsm_server_state = _map_cycle_server_state_display_endgame_views;
    GameControlRulesDaemon::game_state = _game_state_completed;
}

void GameControlRulesDaemon::checkGameRules()
{
    PlayerState *player_state;

    if ( (GameControlRulesDaemon::game_state == _game_state_in_progress) &&
            (NetworkState::status == _network_state_server)
       )
    {
        unsigned char game_type;
        game_type = GameConfig::game_gametype;

        switch( game_type )
        {
            case  _gametype_timelimit:
            {
                int game_minutes = GameManager::getGameTime() / 60;
                if( game_minutes >= GameConfig::game_timelimit )
                {
                    onTimelimitGameCompleted();
                }
                break;
            }
            case _gametype_fraglimit:
            {
                if ( PlayerInterface::testRuleScoreLimit( GameConfig::game_fraglimit, &player_state ) == true )
                {
                    onFraglimitGameCompleted();
                }
                break;
            }
            case _gametype_objective:
            {
                float ratio = (float) GameConfig::game_occupationpercentage / 100.0;
                if (PlayerInterface::testRuleObjectiveRatio( ratio, &player_state))
                {
                    onObjectiveGameCompleted( );
                }
                break;
            }
            case _gametype_objectiveANDfraglimit:
            {
                float ratio = (float) GameConfig::game_occupationpercentage / 100.0;
                if ( (PlayerInterface::testRuleObjectiveRatio( ratio, &player_state)) &&
                    (PlayerInterface::testRuleScoreLimit( GameConfig::game_fraglimit, &player_state ) == true) )
                {
                    onObjectiveANDFraglimitGameCompleted( );
                }
                break;
            }
            case _gametype_fraglimitORtimelimit:
            {
                int game_minutes = GameManager::getGameTime() / 60;
                if ( ( game_minutes >= GameConfig::game_timelimit ) ||
                    (PlayerInterface::testRuleScoreLimit( GameConfig::game_fraglimit, &player_state ) == true) )
                {
                    onFraglimitORTimelimitGameCompleted( );
                }
                break;
            }
            default:
                // nothing
                ;
        }

        // ** Check for Player Respawns **
        bool respawn_rule_complete = false;
        while( respawn_rule_complete == false )
        {
            if ( PlayerInterface::testRulePlayerRespawn( &respawn_rule_complete, &player_state ) )
            {
                PlayerInterface::resetPlayerUnitConfig( player_state->getID() );
                GameManager::spawnPlayer( player_state->getID() );
            }
        }

    }

}

void GameControlRulesDaemon::netMessageCycleMap(const NetMessage* message)
{
    if ( NetworkState::status == _network_state_client || NetworkState::status == _network_state_bot) // client only (security fix)
    {
    GameControlCycleMap *cycle_map_mesg;

    cycle_map_mesg = (GameControlCycleMap *) message;

    snprintf(map_cycle_fsm_client_map_name, 256, "%s", cycle_map_mesg->map_name);
    map_cycle_fsm_client_state = _map_cycle_client_start_map_load;
    }
}

void GameControlRulesDaemon::netMessageCycleRespawnAck(const NetMessage* )
{
    map_cycle_fsm_client_respawn_ack_flag = true;
}

void GameControlRulesDaemon::processNetMessage(const NetMessage* message)
{
    switch(message->message_id) {
        case _net_message_id_game_control_cycle_map :
            netMessageCycleMap(message);
            break;

        case _net_message_id_game_control_cycle_respawn_ack :
            netMessageCycleRespawnAck(message);
            break;

        default:
            LOGGER.warning(
                    "Received GameControlRulesMessage with unknown id (%d-%d)",
                    message->message_class, message->message_id);
            break;
    }
}

void GameControlRulesDaemon::updateGameControlFlow()
{
    if ( NetworkState::status == _network_state_server )
    {
     mapCycleFsmServer();
    }
    else if ( NetworkState::status == _network_state_client)
    {
     mapCycleFsmClient();
    }
    else
    {
     mapCycleFsmBot();
    }

    checkGameRules();
}

void GameControlRulesDaemon::mapLoadFailureResponse(int result_code, const char *map_name)
{
    char str_buf[128];

    if( result_code == _mapload_result_no_map_file ) {
        sprintf( str_buf, "MAP %s NOT FOUND!", map_name );
        LoadingView::append( str_buf);
    } else if( result_code == _mapload_result_no_wad_file ) {
            LoadingView::append( "MAP TILE SET NOT FOUND!" );
    }
}
