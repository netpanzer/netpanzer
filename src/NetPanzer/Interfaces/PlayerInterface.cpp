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

#include <stdexcept>

#include "Interfaces/PlayerInterface.hpp"
#include "Interfaces/GameConfig.hpp"
#include "Interfaces/ConsoleInterface.hpp"
#include "Units/UnitInterface.hpp"
#include "Resources/ResourceManager.hpp"
#include "Objectives/ObjectiveInterface.hpp"
#include "Objectives/Objective.hpp"

#include "Views/Components/Desktop.hpp"

#include "Classes/Network/PlayerNetMessage.hpp"
#include "Classes/Network/NetworkServer.hpp"
#include "Classes/Network/NetworkClient.hpp"
#include "Util/Log.hpp"

#include "Classes/Network/NetworkState.hpp" // needed to flag server or client state

// ** PlayerInterface Statics
PlayerState   * PlayerInterface::player_lists = 0;
PlayerID        PlayerInterface::max_players = MIN_PLAYER_ID;

static bool   * alliance_matrix = 0; // XXX ALLY
PlayerID  PlayerInterface::local_player_index = INVALID_PLAYER_ID;

PlayerID PlayerInterface::respawn_rule_player_index = INVALID_PLAYER_ID;

SDL_mutex* PlayerInterface::mutex = 0;


static void setAlliance(PlayerID by_player, PlayerID with_player )
{
    *(alliance_matrix + (by_player * PlayerInterface::getMaxPlayers()) + with_player ) = true;
}

static void clearAlliance(PlayerID by_player, PlayerID with_player )
{
    *(alliance_matrix + (by_player * PlayerInterface::getMaxPlayers()) + with_player ) = false;
}

static void disconnectedPlayerAllianceCleanup(PlayerID index )
{
    for ( PlayerID player_index = 0; player_index < PlayerInterface::getMaxPlayers(); ++player_index )
    {
        clearAlliance( index, player_index );
        clearAlliance( player_index, index );
    }
}

bool PlayerInterface::isAllied( PlayerID player, PlayerID with_player )
{
    if ( player < max_players && with_player < max_players
            && *(alliance_matrix + (with_player * max_players) + player)
            && *(alliance_matrix + (player * max_players) + with_player) )
    {
        return true;
    }
    return false;
}

bool PlayerInterface::isSingleAllied( PlayerID player, PlayerID with_player )
{
    if ( player < max_players && with_player < max_players
            && *(alliance_matrix + (player * max_players) + with_player) )
    {
        return true;
    }
    return false;
}

static void handleAllianceMessage(const int type,
                                  PlayerID by_player,
                                  PlayerID with_player)
{
    PlayerID local_player = PlayerInterface::getLocalPlayerIndex();
    PlayerState *player_state;
    if ( type == _player_make_alliance )
    {
        setAlliance(by_player, with_player);

        if ( by_player == local_player )
        {
            player_state = PlayerInterface::getPlayer(with_player);
            if ( PlayerInterface::isSingleAllied(player_state->getID(), local_player) )
            {
                ConsoleInterface::postMessage(Color::yellow, false, 0,
                                              "You accepted %s alliance request.",
                                              player_state->getName().c_str());
            }
            else
            {
                ConsoleInterface::postMessage(Color::yellow, false, 0,
                                              "You request alliance with %s.",
                                              player_state->getName().c_str());
            }
        }
        else if ( with_player == local_player )
        {
            player_state = PlayerInterface::getPlayer(by_player);
            if ( PlayerInterface::isSingleAllied( local_player, player_state->getID()) )
            {
                ConsoleInterface::postMessage(Color::yellow, false, 0,
                                           "%s accepted your alliance request.",
                                           player_state->getName().c_str());
            }
            else
            {
                if (player_state->getMute() == false) {
                ConsoleInterface::postMessage(Color::yellow, false, 0,
                                              "%s request to ally with you.",
                                              player_state->getName().c_str());

                // bot automatic ally reply mgmt
                if ( NetworkState::status == _network_state_bot && GameConfig::bot_allied == true && GameConfig::game_allowallies == true )
                {
                if (player_state->getClientType() == 2)
                {
                PlayerAllianceRequest allie_request_back;
                allie_request_back.set( local_player, player_state->getID(), _player_make_alliance);
                CLIENT->sendMessage( &allie_request_back, sizeof(PlayerAllianceRequest));
                }
                }
                //

                }
            }
        }
    }
    else
    {
        // break alliance cancels both alliances
        clearAlliance(by_player, with_player);
        if ( by_player == local_player )
        {
            player_state = PlayerInterface::getPlayer(with_player);
            if ( PlayerInterface::isSingleAllied(player_state->getID(), local_player) )
            {
                ConsoleInterface::postMessage(Color::yellow, false, 0,
                                              "You broke the alliance with %s.",
                                              player_state->getName().c_str());
            }
            else
            {
                ConsoleInterface::postMessage(Color::yellow, false, 0,
                                              "You cancel your alliance request with %s.",
                                              player_state->getName().c_str());
            }
        }
        else if ( with_player == local_player )
        {
            player_state = PlayerInterface::getPlayer(by_player);
            if ( PlayerInterface::isSingleAllied( local_player, player_state->getID()) )
            {
                ConsoleInterface::postMessage(Color::yellow, false, 0,
                                              "%s broke the alliance with you.",
                                              player_state->getName().c_str());
            }
            else
            {
                if (player_state->getMute() == false) {
                ConsoleInterface::postMessage(Color::yellow, false, 0,
                                              "%s cancelled the alliance request with you.",
                                              player_state->getName().c_str());
                }
            }
        }

        clearAlliance(with_player, by_player);
    }
}

static void resetAllianceMatrix()
{
    int matrix_size;

    matrix_size = PlayerInterface::getMaxPlayers() * PlayerInterface::getMaxPlayers();

    for ( int i = 0; i < matrix_size; i++ )
    {
        alliance_matrix[ i ] = false;
    }
}

void PlayerInterface::initialize(const unsigned int _max_players)
{
    char temp_str[64];
    PlayerID player_id;
    max_players = _max_players;

    delete[] player_lists;
    player_lists = new PlayerState[max_players];

    for ( player_id = 0; player_id < max_players; ++player_id )
    {
        player_lists[ player_id ].setID( player_id );
        player_lists[ player_id ].resetStats(false);
        player_lists[ player_id ].setStateFree();
        sprintf( temp_str, "Player %u", player_id );
        player_lists[ player_id ].setName( temp_str );
    }

    delete[] alliance_matrix;
    alliance_matrix = new bool [max_players * max_players];
    resetAllianceMatrix();

    mutex = SDL_CreateMutex();
    if(!mutex)
    {
        throw std::runtime_error("Couldn't create PlayerInterface mutex.");
    }
}

void PlayerInterface::reset()
{
    resetPlayerStats(countPlayers() > 0);
    resetAllianceMatrix(); // XXX ALLY
}

void PlayerInterface::cleanUp()
{
    delete[] player_lists;
    player_lists = 0;
    delete[] alliance_matrix;
    alliance_matrix = 0;
    max_players = 0;

    SDL_DestroyMutex(mutex);
    mutex = 0;
}

void PlayerInterface::lock()
{
    SDL_mutexP(mutex);
}

void PlayerInterface::unLock()
{
    SDL_mutexV(mutex);
}

void PlayerInterface::setKill(PlayerState* by_player, PlayerState* on_player,
        UnitType unit_type)
{
    SDL_mutexP(mutex);
    by_player->incKills( unit_type );
    on_player->incLosses( unit_type );
    SDL_mutexV(mutex);
}

void PlayerInterface::lockPlayerStats()
{
    PlayerID player_id;

    SDL_mutexP(mutex);
    for ( player_id = 0; player_id < max_players; ++player_id )
    {
        player_lists[ player_id ].lockStats();
    } // ** for
    SDL_mutexV(mutex);
}

void PlayerInterface::unlockPlayerStats()
{
    PlayerID player_id;

    SDL_mutexP(mutex);
    for ( player_id = 0; player_id < max_players; ++player_id )
    {
        player_lists[ player_id ].unlockStats();
    } // ** for
    SDL_mutexV(mutex);
}

void PlayerInterface::resetPlayerStats(bool keepAdmin)
{
    PlayerID player_id;

    SDL_mutexP(mutex);
    for ( player_id = 0; player_id < max_players; ++player_id )
    {
        player_lists[ player_id ].resetStats(keepAdmin);
    } // ** for
    SDL_mutexV(mutex);
}

int PlayerInterface::getActivePlayerCount()
{
    PlayerID player_id;
    int count = 0;

    for ( player_id = 0; player_id < max_players; ++player_id )
    {
        player_lists[ player_id ].isActive() && count++;
    } // ** for

    return( count );
}

PlayerState* PlayerInterface::allocateLoopBackPlayer()
{
    local_player_index = 0;

    SDL_mutexP(mutex);
    player_lists[local_player_index].setStateSelectingFlag();
    player_lists[local_player_index].unit_config.initialize();
    SDL_mutexV(mutex);

    return &player_lists[local_player_index];
}

PlayerID PlayerInterface::countPlayers()
{
    PlayerID count;
    PlayerID player_id;
    for ( player_id = 0, count=0; player_id < max_players; ++player_id )
    {
        player_lists[ player_id ].isActive() && count++;
    }
    return count;
}

PlayerState * PlayerInterface::allocateNewPlayer()
{
    PlayerID player_id;
    PlayerState * res = 0;

    SDL_mutexP(mutex);
    for ( player_id = 0; player_id < max_players; ++player_id )
    {
        if ( player_lists[ player_id ].isFree() )
        {
            player_lists[ player_id ].setStateAllocated();
            player_lists[ player_id ].resetStats(false);
            player_lists[ player_id ].unit_config.initialize();
            res = &player_lists[ player_id ];
            break;
        }
    }
    SDL_mutexV(mutex);

    return( res );
}

void PlayerInterface::spawnPlayer( PlayerID player_id, const iXY &location )
{
    if ( player_id < max_players )
    {
        SDL_mutexP(mutex);
        if ( ! player_lists[player_id].isFree() )
        {
            UnitInterface::spawnPlayerUnits( location,
                                             player_id,
                                             player_lists[ player_id ].unit_config
                                           );
        } // ** if _player_state_active
        SDL_mutexV(mutex);
    }
}


bool PlayerInterface::testRuleScoreLimit( long score_limit, PlayerState ** player_state )
{
    PlayerID player_id;

    for ( player_id = 0; player_id < max_players; ++player_id )
    {
        if ( player_lists[ player_id ].getKills() >= score_limit )
        {
            *player_state = &player_lists[ player_id ];
            return( true );
        } // ** if

    } // ** for

    return( false );
}

static bool testRuleObjectiveOccupationRatio( PlayerID player_id,
                                              float precentage )
{
    ObjectiveID num_objectives = ObjectiveInterface::getObjectiveCount();

    size_t occupation_ratio = (size_t) ( ((float) num_objectives) * precentage  + 0.999);

    if (occupation_ratio == 0)
    {
        occupation_ratio = 1;
    }

    size_t occupied = 0;
    for ( int i = 0; i < num_objectives; ++i )
    {
        Objective *objective_state = ObjectiveInterface::getObjective(i);

        if ( objective_state->occupying_player != 0 )
        {
            PlayerID occuping_player_id = objective_state->occupying_player->getID();

            if ( occuping_player_id == player_id )
            {
                occupied++;
            }
            // XXX ALLY
            else if ( PlayerInterface::isAllied(occuping_player_id, player_id) )
            {
                occupied++;
            }
        }
    }

    if ( occupied >= occupation_ratio )
    {
        return true;
    }

    return false;
}

bool PlayerInterface::testRuleObjectiveRatio( float precentage, PlayerState ** player_state )
{
    PlayerID player_id;

    for ( player_id = 0; player_id < max_players; ++player_id )
    {
        if ( testRuleObjectiveOccupationRatio(player_id, precentage) )
        {
            *player_state = &player_lists[ player_id ];
            return true;
        } // ** if
    } // ** for

    return false;
}


bool PlayerInterface::testRulePlayerRespawn( bool *completed, PlayerState **player_state )
{
    if ( respawn_rule_player_index == max_players
        || respawn_rule_player_index == INVALID_PLAYER_ID )
    {
        respawn_rule_player_index = 0;
        *completed = true;
        return( false );
    }
    else
    {
        *completed = false;
    }

    if (  player_lists[ respawn_rule_player_index ].isPlaying()
       && UnitInterface::getUnitCount( respawn_rule_player_index ) == 0 )
    {
        *player_state = &player_lists[ respawn_rule_player_index ];
        ++respawn_rule_player_index;
        return( true );
    }

    ++respawn_rule_player_index;
    return( false );
}

void PlayerInterface::netMessageConnectID(const NetMessage* message)
{
    if ( NetworkState::status == _network_state_client || NetworkState::status == _network_state_bot) // client only (security fix)
    {
    const PlayerConnectID *connect_mesg
        = (const PlayerConnectID *) message;

    local_player_index = connect_mesg->player_id;
    if(local_player_index >= max_players) {
        LOGGER.warning("Invalide netMessageConnectID Message");
        return;
    }

//    SDL_mutexP(mutex);
//    player_lists[local_player_index].setFromNetworkPlayerState
//        (&connect_mesg->connect_state);
//    SDL_mutexV(mutex);
    }
}

void PlayerInterface::netMessageSyncState(const NetMessage* message)
{
    if ( NetworkState::status != _network_state_server ) // client or bot only (security fix)
    {
    const PlayerStateSync *sync_mesg
        = (const PlayerStateSync *) message;
    PlayerID player_id = sync_mesg->player_state.getPlayerIndex();

    if(player_id >= max_players) {
        LOGGER.warning("Malformed MessageSyncState message");
        return;
    }

    SDL_mutexP(mutex);
        player_lists[player_id].setFromNetworkPlayerState(&sync_mesg->player_state);
        // XXX ALLY
        if ( player_lists[player_id].isFree() )
        {
            disconnectedPlayerAllianceCleanup(player_id);
        }
        if ( player_id == local_player_index && player_lists[player_id].isSelectingFlag() )
        {
            //Desktop::setVisibility("UStyleSelectionView", true);
            Desktop::setVisibility("GFlagSelectionView", true);
        }
    SDL_mutexV(mutex);
    }
}

void PlayerInterface::netMessageScoreUpdate(const NetMessage *message)
{
    if ( NetworkState::status == _network_state_client || NetworkState::status == _network_state_bot ) // client only (security fix)
    {
    const PlayerScoreUpdate* score_update
        = (const PlayerScoreUpdate *) message;

    if(score_update->getKillByPlayerIndex() >= PlayerInterface::getMaxPlayers()
            || score_update->getKillOnPlayerIndex()
            >= PlayerInterface::getMaxPlayers())
    {
        LOGGER.warning("Malformed score update packet.");
        return;
    }

    PlayerState* player1 = getPlayer(score_update->getKillByPlayerIndex());
    PlayerState* player2 = getPlayer(score_update->getKillOnPlayerIndex());
    setKill(player1, player2, (UnitType) score_update->unit_type );
    }
}

void PlayerInterface::netMessageAllianceRequest(const NetMessage *message)
{
    if ( GameConfig::game_allowallies == false )
    {
        return;
    }

    const PlayerAllianceRequest *allie_request
        = (const PlayerAllianceRequest *) message;

    if(allie_request->getAllieByPlayerIndex() >= max_players
       || allie_request->getAllieWithPlayerIndex() >= max_players)
    {
        LOGGER.warning("Invalid alliance request message");
        return;
    }

    SDL_mutexP(mutex);
    handleAllianceMessage(allie_request->alliance_request_type,
                          allie_request->getAllieByPlayerIndex(),
                          allie_request->getAllieWithPlayerIndex());
    SDL_mutexV(mutex);

    PlayerAllianceUpdate allie_update;
    allie_update.set(allie_request->getAllieByPlayerIndex(),
                     allie_request->getAllieWithPlayerIndex(),
                     allie_request->alliance_request_type);
//    SERVER->broadcastMessage(&allie_update, sizeof(PlayerAllianceUpdate));
    if ( allie_request->getAllieByPlayerIndex() != local_player_index )
    {
        SERVER->sendMessage(allie_request->getAllieByPlayerIndex(),
                            &allie_update, sizeof(PlayerAllianceUpdate));
    }
    if ( allie_request->getAllieWithPlayerIndex() != local_player_index )
    {
        SERVER->sendMessage(allie_request->getAllieWithPlayerIndex(),
                            &allie_update, sizeof(PlayerAllianceUpdate));
    }
}

void PlayerInterface::netMessageAllianceUpdate(const NetMessage* message)
{
    const PlayerAllianceUpdate* allie_update
        = (const PlayerAllianceUpdate *) message;

    if(allie_update->getAllieByPlayerIndex() >= max_players
       || allie_update->getAllieWithPlayerIndex() >= max_players) {
        LOGGER.warning("Invalid alliance update message");
        return;
    }

    SDL_mutexP(mutex);
    handleAllianceMessage(allie_update->alliance_update_type,
                          allie_update->getAllieByPlayerIndex(),
                          allie_update->getAllieWithPlayerIndex());
    SDL_mutexV(mutex);
}

void PlayerInterface::netMessageStyleUpdate(const NetMessage* message, PlayerID playid)
{

    const UpdatePlayerUnitStyle* upus = (const UpdatePlayerUnitStyle*)message;

    unsigned char nustyle = upus->player_unit_style;

    player_lists[playid].setPlayerStyle((unsigned char)nustyle);

    UnitStyleSync uss;

    uss.player_id = playid;

    uss.player_unit_style = nustyle;

    SERVER->broadcastMessage(&uss, sizeof(uss));

}

void PlayerInterface::netMessageStyleSync(const NetMessage* message)
{
    if ( NetworkState::status == _network_state_client || NetworkState::status == _network_state_bot )
    {
    const UnitStyleSync* uss = (const UnitStyleSync*)message;

    unsigned char nustyle = uss->player_unit_style;

    if (player_lists[uss->player_id].isSelectingFlag()) {
    player_lists[uss->player_id].setPlayerStyle((unsigned char)nustyle);
    } //no tricks!
    } //no cheat!
}



void PlayerInterface::processNetMessage(const NetPacket* packet)
{
    const NetMessage* message = packet->getNetMessage();
    switch(message->message_id) {
        case _net_message_id_player_connect_id :
            netMessageConnectID(message);
            break;

        case _net_message_id_player_sync_flag:
            {
                const PlayerFlagSync* pfs = (const PlayerFlagSync*)message;
                ResourceManager::updateFlagData(pfs->player_id,
                                                pfs->player_flag,
                                                sizeof(pfs->player_flag) );

                player_lists[pfs->player_id].setStateActive();
            }
            break;

        case _net_message_id_player_update_flag:
            {
                const UpdatePlayerFlag* upf = (const UpdatePlayerFlag*)message;
                ResourceManager::updateFlagData(packet->fromPlayer,
                                                upf->player_flag,
                                                sizeof(upf->player_flag) );

                PlayerFlagSync pfs;
                pfs.player_id = packet->fromPlayer;
                memcpy(pfs.player_flag, upf->player_flag, sizeof(pfs.player_flag));

                player_lists[packet->fromPlayer].setStateActive();

                SERVER->broadcastMessage(&pfs, sizeof(pfs));
            }
            break;

        case _net_message_id_player_sync_state :
            netMessageSyncState(message);
            break;

        case _net_message_id_player_score_update :
            netMessageScoreUpdate(message);
            break;

        case _net_message_id_player_alliance_request :
            netMessageAllianceRequest(message);
            break;

        case _net_message_id_player_alliance_update :
            netMessageAllianceUpdate(message);
            break;

        case _net_message_id_player_unit_style_update :
            netMessageStyleUpdate(message, packet->fromPlayer);
            break;

        case _net_message_id_player_unit_style_sync :
            netMessageStyleSync(message);
            break;

        case _net_message_id_player_flagtimer_update :
                const PlayerFlagTimerUpdate* pftu = (const PlayerFlagTimerUpdate*)message;
                gameconfig->game_changeflagtime = pftu->getflagtimer();
            break;

    }
}

void PlayerInterface::disconnectPlayerCleanup( PlayerID player_id )
{
    PlayerState *player_state = getPlayer( player_id );
    if ( player_state )
    {
        SDL_mutexP(mutex);

        // XXX ALLY
        disconnectedPlayerAllianceCleanup(player_id);

        player_state->setStateFree();
        player_state->setAdmin(false);

        PlayerStateSync player_state_update(player_state->getNetworkPlayerState());

        SDL_mutexV(mutex);

        SERVER->broadcastMessage(&player_state_update, sizeof(PlayerStateSync));
    }
}

void PlayerInterface::SyncFlagTimer()
{
    PlayerFlagTimerUpdate player_flagtimer_update(gameconfig->game_changeflagtime);

    SERVER->broadcastMessage(&player_flagtimer_update, sizeof(PlayerFlagTimerUpdate));
}
