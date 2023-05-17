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

#include <algorithm>
#include "ServerConnectDaemon.hpp"
#include "NetworkServer.hpp"
#include "NetMessage.hpp"
#include "NetMessageEncoder.hpp"

#include "Classes/PlayerState.hpp"
#include "PlayerNetMessage.hpp"
#include "ConnectNetMessage.hpp"
#include "SystemNetMessage.hpp"

#include "Objectives/ObjectiveInterface.hpp"
#include "Resources/ResourceManager.hpp"
#include "PowerUps/PowerUpInterface.hpp"
#include "Units/UnitInterface.hpp"
#include "Units/UnitProfileInterface.hpp"
#include "Core/NetworkGlobals.hpp"
#include "Util/Log.hpp"

#include "Interfaces/ConsoleInterface.hpp"
#include "Interfaces/ChatInterface.hpp"
#include "Interfaces/PlayerInterface.hpp"
#include "Interfaces/MapInterface.hpp"
#include "Interfaces/GameManager.hpp"
#include "Interfaces/GameConfig.hpp"

enum ConnectionState
{
    connect_state_idle,
    connect_state_wait_for_connect_request,
    connect_state_wait_for_client_settings,
    connect_state_wait_for_client_game_setup_ack,
    connect_state_unit_profile_sync,
    connect_state_player_state_sync,
    connect_state_sync_flags,
    connect_state_unit_sync,
    connect_state_finish_connect,
    connect_state_drop_bad_connecting_client,
    connect_state_drop_bad_client
};

bool                connection_lock_state = false;
ClientSocket      * connect_client;
Timer		    time_out_timer;
int	            time_out_counter;
Timer               sendunitpercent_timer;
int                 sync_count = 0;
int                 sync_end = 0;
int                 sync_total = 0;
int                 sync_done = 0;

unsigned char nss = 3; //unknown client type

typedef std::list<ClientSocket *> ConnectQueue;
typedef ConnectQueue::iterator ConnectQueueIterator;
static ConnectQueue connect_queue;


#define _SERVER_CONNECT_TIME_OUT_TIME (20.0)
#define _SERVER_CONNECT_RETRY_LIMIT   (5)
#define _SENDUNITPERCENT_TIME (1)

static void sendConnectionAlert(ClientSocket * client)
{
    SystemConnectAlert connect_alert;

    PlayerState *player_state = 0;

    player_state = PlayerInterface::getPlayer( client->getPlayerIndex() );

    connect_alert.set( client->getPlayerIndex(), _connect_alert_mesg_connect );

    if (MapInterface::chat_color_scheme == 0) {
    ConsoleInterface::postMessage(Color::cyan, true, player_state->getFlag(),
                                  "'%s' [%s] has joined the game.",
                                  player_state->getName().c_str(),
                                  client->getFullIPAddress().c_str() );
    } else {
    ConsoleInterface::postMessage(Color::darkBlue, true, player_state->getFlag(),
                                  "'%s' [%s] has joined the game.",
                                  player_state->getName().c_str(),
                                  client->getFullIPAddress().c_str() );
    }

    player_state->resetAutokick();

    if ( GameConfig::server_motd->length() > 0 )
    {
        ChatInterface::serversayTo(client->getPlayerIndex(), GameConfig::server_motd->c_str());
    }

    SERVER->broadcastMessage( &connect_alert, sizeof(SystemConnectAlert));
}

template<typename T>
class State
{
public:
    T state;
    State() : state((T)0) {};
    virtual ~State() {}

    virtual void enter() {}
    virtual T process() { return state; }
    virtual T message(const NetMessage *msg) { (void)msg; return state;}
    virtual void leave() {}
};

template<typename T>
class StateMachine
{
private:
    std::vector<State<T> *> states;
    State<T> * current;
    T state;

public:
    StateMachine() : current(0) {}
    ~StateMachine()
    {
        for (int i = 0; i < states.size(); i++)
        {
            delete states[i];
        }
        states.clear();
    }

    bool isState(T state) { return this->state == state; }

    void addState(State<T> * st)
    {
        st->state = (T)states.size();
        states.push_back(st);
    }

    void enterState(T state)
    {
        this->state = state;
        current = states[state];
        current->enter();
    }

    void switchState(T state)
    {
        current->leave();
        enterState(state);
    }

    void process()
    {
        T new_state = current->process();
        if ( new_state != state )
        {
            switchState(new_state);
        }
    }

    void message(const NetMessage* msg)
    {
        T new_state = current->message(msg);
        if ( new_state != state )
        {
            switchState(new_state);
        }
    }
};

template<typename T>
class StateIdle : public State<T>
{
public:
    virtual T process()
    {
        if ( !connect_queue.empty() && connection_lock_state == false )
        {
            connect_client = connect_queue.front();
            connect_queue.pop_front();

            ClientConnectStartConnect start_connect;

            connect_client->sendMessage( &start_connect,
                                         sizeof(ClientConnectStartConnect));

            connect_client->sendRemaining();

            return connect_state_wait_for_connect_request;
        }
        return this->state;
    }

    virtual void leave()
    {
        unsigned long queue_position = 1;

        ConnectProcessUpdate process_update;

        ConnectQueueIterator i;
        for(i = connect_queue.begin(); i != connect_queue.end(); ++i)
        {
            process_update.setQueuePosition(queue_position);

            (*i)->sendMessage( &process_update, sizeof(ConnectProcessUpdate));
            (*i)->sendRemaining();
            queue_position++;
        }
    }
};

template<typename T>
class StateWaitConnectRequest : public State<T>
{
public:
    virtual void enter() { time_out_timer.reset(); }

    virtual T process()
    {
        return time_out_timer.count() ? connect_state_drop_bad_client : State<T>::state;
    }

    virtual T message(const NetMessage* msg)
    {
        T new_state = connect_state_drop_bad_connecting_client;
        if ( msg->message_id == _net_message_id_client_connect_request )
        {
            ClientConnectResult connect_result;

            if ((GameConfig::game_allowmultiip == false) &&
               (SERVER->isAlreadyConnected(connect_client) == true))
            {
                connect_result.result_code = _connect_result_server_already_connected;
                LOGGER.warning("Client already connected!!!!, die");
            }
            else
            {
                PlayerState * player = PlayerInterface::allocateNewPlayer();
                if ( player == 0 )
                {
                    connect_result.result_code = _connect_result_server_full;
                }
                else
                {
                    connect_result.result_code = _connect_result_success;
                    connect_client->player_id = player->getID();
                    new_state = connect_state_wait_for_client_settings;
                }
            }

            connect_client->sendMessage( &connect_result,
                                         sizeof(ClientConnectResult));
            connect_client->sendRemaining();
        }

        return new_state;
    }
};

template<typename T>
class StateWaitClientSettings : public State<T>
{
private:
    PlayerState * player;
public:
    virtual void enter()
    {
        player = PlayerInterface::getPlayer(connect_client->getPlayerIndex());
        time_out_timer.reset();
    }

    virtual T process()
    {
        if ( time_out_timer.count() )
        {
            player->setStateFree();
            return connect_state_drop_bad_connecting_client;
        }
        return State<T>::state;
    }

    virtual T message(const NetMessage* msg)
    {
        if ( msg->message_id == _net_message_id_connect_client_settings )
        {
            ConnectClientSettings *client_setting;

            client_setting = (ConnectClientSettings *) msg;

            player->setName( client_setting->player_name );

            if (client_setting->getNStatus() != nss) {
                LOGGER.warning("Client contradicts itself!!!!");
            }
            player->setClientType(nss);
            //temp style
            //player->setPlayerStyle(0);

            ResourceManager::updateFlagData(player->getID(),
                                            client_setting->player_flag,
                                            sizeof(client_setting->player_flag) );

            player->setStateConnecting();

            // ** send server game setting map, units, player, etc.
            ConnectMesgServerGameSettings* server_game_setup
                = GameManager::getServerGameSetup();

            connect_client->sendMessage( server_game_setup,
                                         sizeof(ConnectMesgServerGameSettings));
            connect_client->sendRemaining();
            delete server_game_setup;

            return connect_state_wait_for_client_game_setup_ack;
        }
        return State<T>::state;
    }
};

template<typename T>
class StateWaitClientGameSetupAck : public State<T>
{
private:
    PlayerState * player;
public:
    virtual void enter()
    {
        player = PlayerInterface::getPlayer(connect_client->getPlayerIndex());
        time_out_timer.reset();
    }

    virtual T process()
    {
        if ( time_out_timer.count() )
        {
            player->setStateFree();
            return connect_state_drop_bad_connecting_client;
        }
        return State<T>::state;
    }

    virtual T message(const NetMessage* msg)
    {
        if ( msg->message_id == _net_message_id_connect_client_game_setup_ack )
        {
            PlayerConnectID player_connect_mesg(player->getID());
            connect_client->sendMessage( &player_connect_mesg,
                                         sizeof(PlayerConnectID));

            return connect_state_unit_profile_sync;
        }
        else if ( msg->message_id == _net_message_id_connect_client_game_setup_ping )
        {
            time_out_timer.reset();
        }
        return State<T>::state;
    }
};

template<typename T>
class StateUnitProfileSync : public State<T>
{
private:
    PlayerState * player;
    int sync_count;
    int sync_end;

    bool sendNextProfile()
    {
        Uint8 data[ _MAX_NET_PACKET_SIZE ];
        NetMessage *pmsg = (NetMessage*)&data;
        int len = UnitProfileInterface::fillProfileSyncMessage(pmsg, sync_count++);
        connect_client->sendMessage(data, len);
        connect_client->sendRemaining();

        return sync_count == sync_end;
    }

public:
    virtual void enter()
    {
        player = PlayerInterface::getPlayer(connect_client->getPlayerIndex());

        ConnectProcessStateMessage state_mesg;
        state_mesg.setMessageEnum(_connect_state_message_sync_unit_profiles);
        state_mesg.setPercentComplete(0);
        connect_client->sendMessage( &state_mesg,
                                     sizeof(ConnectProcessStateMessage));
        connect_client->sendRemaining();

        sync_count = 0;
        sync_end = UnitProfileInterface::getNumUnitTypes();

        time_out_timer.reset();
    }

    virtual T process()
    {
        if ( sync_count == 0 && sendNextProfile() )
        {
            return connect_state_player_state_sync;
        }
        else if ( time_out_timer.count() )
        {
            player->setStateFree();
            return connect_state_drop_bad_connecting_client;
        }
        return State<T>::state;
    }

    virtual T message(const NetMessage* msg)
    {
        if ( msg->message_id == _net_message_id_connect_client_send_next_unit_profile )
        {
            if ( sendNextProfile() )
            {
                return connect_state_player_state_sync;
            }
        }
        else if ( msg->message_id == _net_message_id_connect_client_game_setup_ping )
        {
            time_out_timer.reset();
        }
        return State<T>::state;
    }
};

template<typename T>
class StatePlayerSync : public State<T>
{
private:
    PlayerState * player;
    int sync_count;
    int sync_end;

    bool sendNextPlayers()
    {
        NetMessageEncoder encoder;
        PlayerStateSync pss;

        do
        {
            PlayerInterface::getPlayer(sync_count)->getNetworkPlayerState(pss.player_state);
            if ( ! encoder.encodeMessage(&pss, sizeof(pss)) )
            {
                break;
            }
        } while ( ++sync_count < sync_end );

        connect_client->sendMessage(encoder.getEncodedMessage(),
                                    encoder.getEncodedLen());

        sendStatePercent();

        connect_client->sendRemaining();

        return sync_count == sync_end;
    }

    void sendStatePercent()
    {
        ConnectProcessStateMessage state_mesg;

        state_mesg.setMessageEnum(_connect_state_message_sync_player_info_percent);
        state_mesg.setPercentComplete((sync_count*100)/sync_end);

        connect_client->sendMessage( &state_mesg, sizeof(state_mesg));
    }

public:
    virtual void enter()
    {
        player = PlayerInterface::getPlayer(connect_client->getPlayerIndex());

        ConnectProcessStateMessage state_mesg;
        state_mesg.setMessageEnum(_connect_state_message_sync_player_info);
        state_mesg.setPercentComplete(0);
        connect_client->sendMessage( &state_mesg,
                                     sizeof(ConnectProcessStateMessage));
        connect_client->sendRemaining();

        sync_count = 0;
        sync_end = PlayerInterface::getMaxPlayers();

        time_out_timer.reset();
    }

    virtual T process()
    {
        if ( sendNextPlayers() )
        {
            return connect_state_sync_flags;
        }
        else if ( time_out_timer.count() )
        {
            player->setStateFree();
            return connect_state_drop_bad_connecting_client;
        }
        return State<T>::state;
    }

    virtual T message(const NetMessage* msg)
    {
        if ( msg->message_id == _net_message_id_connect_client_game_setup_ping )
        {
            time_out_timer.reset();
        }
        return State<T>::state;
    }
};

template<typename T>
class StateFlagSync : public State<T>
{
private:
    PlayerState * player;
    int sync_count;
    int sync_end;

    bool sendNextFlag()
    {
        while ( sync_count < sync_end && ! PlayerInterface::isPlayerActive(sync_count) )
        {
            sync_count += 1;
        }

        if ( sync_count < sync_end && PlayerInterface::isPlayerActive(sync_count) )
        {
            PlayerFlagSync pfs;
            pfs.player_id = sync_count;
            ResourceManager::getFlag(sync_count)->frameToBuffer(pfs.player_flag, sizeof(pfs.player_flag));

            connect_client->sendMessage(&pfs, sizeof(pfs));
            connect_client->sendRemaining();

            sync_count += 1;
        }

        return sync_count == sync_end;
    }

public:
    virtual void enter()
    {
        player = PlayerInterface::getPlayer(connect_client->getPlayerIndex());

        sync_count = 0;
        sync_end = PlayerInterface::getMaxPlayers();

        time_out_timer.reset();
    }

    virtual T process()
    {
        if ( sendNextFlag() )
        {
            return connect_state_unit_sync;
        }
        else if ( time_out_timer.count() )
        {
            player->setStateFree();
            return connect_state_drop_bad_connecting_client;
        }
        return State<T>::state;
    }

    virtual T message(const NetMessage* msg)
    {
        if ( msg->message_id == _net_message_id_connect_client_game_setup_ping )
        {
            time_out_timer.reset();
        }
        return State<T>::state;
    }
};

template<typename T>
class StateUnitSync : public State<T>
{
private:
    PlayerState * player;
    int sync_count;
    int sync_end;
    int sync_done;
    int sync_total;

    bool sendNextUnits()
    {
        NetMessageEncoder encoder;
        UnitInterface::Units::const_iterator i;
        iXY unit_map_loc;
        UnitBase* unit;

        do
        {
            i = UnitInterface::getUnits().lower_bound(sync_count);
            if ( i == UnitInterface::getUnits().end() || i->first >= sync_end )
            {
                sync_count = sync_end;
                sync_done = sync_total;
                break;
            }

            unit = i->second;
            MapInterface::pointXYtoMapXY(unit->unit_state.location, unit_map_loc);
/*
            UnitRemoteCreate urc(unit->player->getID(),
                                 unit->id,
                                 unit_map_loc.x,
                                 unit_map_loc.y,
                                 unit->unit_state.unit_type);
*/

            UnitRemoteCreateFull urc(unit->player->getID(),
                                 unit->id,
                                 unit->unit_state.unit_style,
                                 unit->unit_state.moving,
                                 unit_map_loc.x,
                                 unit_map_loc.y,
                                 unit->unit_state.unit_type,
                                 unit->unit_state.body_angle,
                                 unit->unit_state.turret_angle,
                                 unit->unit_state.orientation,
                                 unit->unit_state.speed_rate,
                                 unit->unit_state.speed_factor,
                                 unit->unit_state.reload_time,
                                 unit->unit_state.max_hit_points,
                                 unit->unit_state.hit_points,
                                 unit->unit_state.damage_factor,
                                 unit->unit_state.weapon_range,
                                 unit->unit_state.defend_range);

                                 //LOGGER.info("Sent Msg -> unit_id: %d", unit->id);

            if ( ! encoder.encodeMessage(&urc, sizeof(urc)) )
            {
                break;
            }

            sync_done += 1;
            sync_count = i->first + 1;

        } while ( sync_count < sync_end );

        connect_client->sendMessage(encoder.getEncodedMessage(),
                                    encoder.getEncodedLen());

        if ( sync_count < sync_end )
        {
            if ( sendunitpercent_timer.count() )
            {
                ConnectProcessStateMessage state_mesg;
                state_mesg.setMessageEnum(_connect_state_message_sync_units_percent);
                state_mesg.setPercentComplete((sync_done*100)/sync_total);
                connect_client->sendMessage( &state_mesg, sizeof(state_mesg));
                sendunitpercent_timer.reset();
            }
        }

        return sync_count == sync_end;
    }

public:
    virtual void enter()
    {
        player = PlayerInterface::getPlayer(connect_client->getPlayerIndex());

        sync_count = 0;
        sync_done = 0;
        sync_end = 0;
        sync_total = UnitInterface::getTotalUnitCount();

        if ( sync_total )
        {
            sync_end = UnitInterface::getUnits().rbegin()->first;
        }

        sync_end += 1;

        ConnectProcessStateMessage state_mesg;
        state_mesg.setMessageEnum(_connect_state_message_sync_units);
        connect_client->sendMessage( &state_mesg,
                                     sizeof(ConnectProcessStateMessage));

        SERVER->addClientToSendList( connect_client );

        PlayerState *p = PlayerInterface::getPlayer(connect_client->getPlayerIndex());
        PlayerStateSync player_state_update( p->getNetworkPlayerState() );
        SERVER->broadcastMessage(&player_state_update, sizeof(PlayerStateSync));

        sendunitpercent_timer.reset();
        time_out_timer.reset();
    }

    virtual T process()
    {
        if ( sendNextUnits() )
        {
            return connect_state_finish_connect;
        }
        return State<T>::state;
    }

    virtual T message(const NetMessage* msg)
    {
        if ( msg->message_id == _net_message_id_connect_client_game_setup_ping )
        {
            time_out_timer.reset();
        }
        return State<T>::state;
    }

    virtual void leave()
    {
        ConnectProcessStateMessage state_mesg;

        state_mesg.setMessageEnum(_connect_state_message_sync_units_percent);
        state_mesg.setPercentComplete(100);
        connect_client->sendMessage( &state_mesg, sizeof(state_mesg));

        UnitSyncIntegrityCheck unit_integrity_check_mesg;
        connect_client->sendMessage( &unit_integrity_check_mesg,
                                     sizeof(UnitSyncIntegrityCheck));
    }
};

template<typename T>
class StateFinishConnect : public State<T>
{
public:
    virtual T process()
    {
        ObjectiveInterface::syncObjectives( connect_client );

        PowerUpInterface::syncPowerUps( connect_client );

        PlayerState * player = PlayerInterface::getPlayer(connect_client->getPlayerIndex());

        player->setStateSelectingFlag();

        PlayerFlagSync pfs;
        pfs.player_id = connect_client->player_id;
        ResourceManager::getFlag(connect_client->player_id)->frameToBuffer(pfs.player_flag, sizeof(pfs.player_flag));
        SERVER->broadcastMessage(&pfs, sizeof(pfs));

        PlayerStateSync player_state_update
            (player->getNetworkPlayerState());

        SERVER->broadcastMessage( &player_state_update, sizeof(PlayerStateSync));

        ConnectProcessStateMessage state_mesg;
        state_mesg.setMessageEnum(_connect_state_sync_complete);
        connect_client->sendMessage( &state_mesg,
                                     sizeof(ConnectProcessStateMessage));
        sendConnectionAlert( connect_client );

        return connect_state_idle;
    }
};

template<typename T>
class StateDropBadConnectingClient : public State<T>
{
public:
    virtual T process()
    {
        connect_client->hardClose();
        return connect_state_idle;
    }
};

template<typename T>
class StateDropBadClient : public State<T>
{
public:
    virtual T process()
    {
        SERVER->dropClient(connect_client);
        return connect_state_idle;
    }
};

static StateMachine<ConnectionState>* fsm = 0;

void ServerConnectDaemon::initialize(unsigned long max_players)
{
    (void) max_players;
    time_out_timer.changePeriod( _SERVER_CONNECT_TIME_OUT_TIME );
    sendunitpercent_timer.changePeriod( _SENDUNITPERCENT_TIME );
    fsm = new StateMachine<ConnectionState>();

    fsm->addState(new StateIdle<ConnectionState>());
    fsm->addState(new StateWaitConnectRequest<ConnectionState>());
    fsm->addState(new StateWaitClientSettings<ConnectionState>());
    fsm->addState(new StateWaitClientGameSetupAck<ConnectionState>());
    fsm->addState(new StateUnitProfileSync<ConnectionState>());
    fsm->addState(new StatePlayerSync<ConnectionState>());
    fsm->addState(new StateFlagSync<ConnectionState>());
    fsm->addState(new StateUnitSync<ConnectionState>());
    fsm->addState(new StateFinishConnect<ConnectionState>());
    fsm->addState(new StateDropBadConnectingClient<ConnectionState>());
    fsm->addState(new StateDropBadClient<ConnectionState>());

    fsm->enterState(connect_state_idle);

}

void ServerConnectDaemon::startConnectDaemon( unsigned long max_players )
{
    (void) max_players;
    time_out_timer.changePeriod( _SERVER_CONNECT_TIME_OUT_TIME );
    sendunitpercent_timer.changePeriod( _SENDUNITPERCENT_TIME );
    fsm->enterState(connect_state_idle);
}

void ServerConnectDaemon::shutdownConnectDaemon()
{
    ConnectMesgNetPanzerServerDisconnect server_disconnect;

    SERVER->broadcastMessage( &server_disconnect,
                         sizeof(ConnectMesgNetPanzerServerDisconnect));
}

bool ServerConnectDaemon::inConnectQueue( ClientSocket *client )
{
    if ( ! fsm->isState(connect_state_idle)
       && connect_client == client )
    {
        return true;
    }

    ConnectQueueIterator i;
    i = std::find(connect_queue.begin(), connect_queue.end(), client);
    return i != connect_queue.end();
}

static void netPacketClientDisconnect(const NetPacket *packet)
{
    SERVER->niceDisconnect(packet->fromClient);
}

static void netPacketClientJoinRequest(const NetPacket* packet)
{
    const ClientConnectJoinRequest* join_request_mesg
        = (const ClientConnectJoinRequest *) packet->getNetMessage();
    ClientConnectJoinRequestAck join_request_ack;

    join_request_ack.setResultCode(_join_request_result_success);
    bool isbad = false;
    unsigned char bpass = join_request_mesg->getNMode();
    nss = bpass;
    NPString pass;
    join_request_mesg->getPassword(pass);

    if ( join_request_mesg->getProtocolVersion() != NETPANZER_PROTOCOL_VERSION )
    {
        join_request_ack.setResultCode(_join_request_result_invalid_protocol);
        isbad = true;
    }

    //temporary
    /*
    if ( !isbad && GameConfig::game_bots_allowed == false)
    {
        if ( bpass == 15 )
        {
            join_request_ack.setResultCode(_join_request_result_access_denied);
            isbad = true;
        }
    }
    */

    if ( !isbad)
    {
        if ( bpass == 2 )
        {
            if (packet->fromClient->getIPAddress() != "127.0.0.1") {
            join_request_ack.setResultCode(_join_request_result_access_denied);
            isbad = true;
            }
        }
    }

    if ( !isbad && GameConfig::game_gamepass->size() > 0 && GameConfig::server_authentication == false)
    {
        if ( GameConfig::game_gamepass->compare(pass) != 0 )
        {
            join_request_ack.setResultCode(_join_request_result_wrong_password);
            isbad = true;
        }
    }

    // Authentication mgmt

    if ( !isbad && GameConfig::server_authentication == true)
    {
        /*
        if ( !isbad )
        {
            join_request_ack.setResultCode(_join_request_result_authentication_failed);
            isbad = true;
        }
        */
    }

    //

    if ( !isbad )
    {
        if ( !ServerConnectDaemon::inConnectQueue( packet->fromClient ) )
        {
            if (connect_queue.size() > 25)
            {
                join_request_ack.setResultCode(_join_request_result_server_busy);
                isbad = true;
            }
            else
            {
                connect_queue.push_back(packet->fromClient);
            }
        }
    }

    join_request_ack.setServerProtocolVersion(NETPANZER_PROTOCOL_VERSION);
    packet->fromClient->sendMessage(&join_request_ack,
                         sizeof(ClientConnectJoinRequestAck));
    packet->fromClient->sendRemaining();

    if ( isbad )
    {
        packet->fromClient->hardClose();
    }
}

void ServerConnectDaemon::processNetPacket(const NetPacket* packet)
{
    const NetMessage* message = packet->getNetMessage();

    switch(message->message_id) {
        case _net_message_id_connect_netPanzer_client_disconnect:
            netPacketClientDisconnect(packet);
            break;

        case _net_message_id_connect_join_game_request:
            netPacketClientJoinRequest(packet);
            break;

        default :
            fsm->message(message);
            break;
    }
}

void ServerConnectDaemon::connectProcess()
{
    fsm->process();
}

void
ServerConnectDaemon::removeClientFromQueue(ClientSocket *client)
{
    connect_queue.remove(client);

    if( connect_client == client )
    {
        fsm->enterState(connect_state_idle);
    }
}

void ServerConnectDaemon::lockConnectProcess()
{
    connection_lock_state = true;
}

void ServerConnectDaemon::unlockConnectProcess()
{
    connection_lock_state = false;
}

bool ServerConnectDaemon::getConnectLockStatus()
{
    return( connection_lock_state );
}

bool ServerConnectDaemon::isConnecting()
{
    return ! fsm->isState(connect_state_idle);
}
