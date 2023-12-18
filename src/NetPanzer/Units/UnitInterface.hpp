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
#ifndef _UNITINTERFACE_HPP
#define _UNITINTERFACE_HPP

#include <vector>
#include <map>
#include "Units/UnitBucketArray.hpp"
#include "Classes/UnitMessage.hpp"
#include "Classes/PlayerState.hpp"
#include "Util/Timer.hpp"
#include "Classes/PlacementMatrix.hpp"

#include "Classes/Network/UnitNetMessage.hpp"
#include "Units/UnitOpcodeEncoder.hpp"

enum { _search_all_players,
       _search_player,
       _search_exclude_player,
       _search_enemy_only };

enum { _no_unit_found, _unit_player, _unit_allied, _unit_enemy };

class NetPacket;

class UnitInterface
{
public:
    typedef std::map<UnitID, UnitBase*> Units;
    typedef std::vector<UnitBase*> PlayerUnitList;

private:
    static Units units;
    static PlayerUnitList* playerUnitLists;

    static UnitBucketArray unit_bucket_array;
    static PlayerID max_players;
    static size_t units_per_player;
    static PlacementMatrix unit_placement_matrix;

    static Uint16 lastUnitID;
    static UnitID newUnitID();

    static UnitBase* newUnit(unsigned short unit_type,
                             const iXY &location,
                             PlayerID player_index,
                             UnitID id);
    static void addNewUnit(UnitBase *unit);
    static void removeUnit(Units::iterator i);

    static void sortBucketArray();

public:
    static void initialize( unsigned long max_units );
    static void cleanUp();
    static void reset();

    static const Units& getUnits()
    {
        return units;
    }

    static const PlayerUnitList& getPlayerUnits(PlayerID player_id)
    {
        assert(player_id < max_players);
        return playerUnitLists[player_id];
    }

#if 0
    static UnitList * getUnitList(size_t player)
    {
        assert( (player < max_players) );
        return ( &unit_lists[ player ] );
    }
#endif

    static size_t getUnitCount(PlayerID player_index)
    {
        assert( (player_index < max_players) );
        return playerUnitLists[player_index].size();
    }

    static size_t getTotalUnitCount()
    {
        return units.size();
    }

    static UnitBase* getUnit(UnitID unit_id);

    static void processNetPacket(const NetPacket* packet);
    static void sendMessage(const UnitMessage* message,
            const PlayerState* player = 0);

    static void updateUnitStatus();

    static void offloadGraphics( SpriteSorter &sorter );

    static UnitBase* createUnit( unsigned short unit_type,
                                  const iXY &location,
                                  PlayerID player_id);

    static void spawnPlayerUnits( const iXY &location,
                                  PlayerID player_id,
                                  const PlayerUnitConfig &unit_config );

    static void queryUnitsAt(std::vector<UnitID>& working_list,
                             const iXY& point, PlayerID player_id,
                             unsigned char search_flags);

    static void queryUnitsAt(std::vector<UnitID>& working_list,
                            const iRect& rect, PlayerID player_id,
                            unsigned char search_flags);

    static PlayerState* querySinglePlayerInArea(const iRect& rect);

    static bool queryClosestUnit( UnitBase **closest_unit_ptr,
                                   iXY &loc,
                                   PlayerID player_id,
                                   unsigned char search_flags );

    static bool queryClosestUnit( UnitBase **closest_unit_ptr,
                                   iRect &bounding_rect,
                                   iXY &loc );

    static bool queryClosestEnemyUnit(UnitBase **closest_unit_ptr,
                                      iXY &loc,
                                      PlayerID player_index);

    static bool queryClosestEnemyUnitInRange(UnitBase **closest_unit_ptr,
                                      iXY &loc, unsigned long wrange,
                                      PlayerID player_index);

    static bool queryUnitAtMapLoc( iXY map_loc, UnitID *query_unit_id );

    static unsigned char queryUnitLocationStatus( iXY loc );





protected:
    // Unit Message Handler Methods
    static void processManagerMessage(const UnitMessage *message);
    static void unitManagerMesgEndLifecycle(const UnitMessage *message);

protected:
    friend class Vehicle;

    // Network Message Handler Variables
    static Timer message_timer;
    static Timer no_guarantee_message_timer;
    static UnitOpcodeEncoder opcode_encoder;

    // Network Message Handler Methods
    static void sendOpcode(const UnitOpcode* opcode)
    {
        opcode_encoder.encode(opcode);
    }

    static void unitSyncMessage(const NetMessage *net_message );
    static void unitOpcodeMessage(const NetMessage *net_message, size_t size);
    static void unitDestroyMessage(const NetMessage *net_message );
    static void unitCreateMessage(const NetMessage *net_message );
    static void unitSyncIntegrityCheckMessage(const NetMessage *net_message );
    static void unitModSpeedMessage(const NetMessage *net_message );
    static void unitModReloadMessage(const NetMessage* net_message);
    static void unitModFireMessage(const NetMessage* net_message);
    static void unitModWRangeMessage(const NetMessage* net_message);
    static void unitModHPMessage(const NetMessage* net_message);
    static void unitModMHPMessage(const NetMessage* net_message);
    static void unitModGRMessage(const NetMessage* net_message);
    static void unitModSuperunitMessage(const NetMessage* net_message);
    static void unitCreateMessageFull(const NetMessage *net_message );

    static void unitModGSpeedMessage(const NetMessage* net_message);
    static void unitModGReloadMessage(const NetMessage* net_message);
    static void unitModGFireMessage(const NetMessage* net_message);

protected:
    static unsigned long  sync_units_iterator;
    static bool	      sync_units_complete_flag;
    static unsigned short sync_units_list_index;
    static Timer	  sync_units_packet_timer;
    static unsigned long  sync_units_in_sync_count;
    static unsigned long  sync_units_in_sync_partial_count;
    static unsigned long  sync_units_total_units;

public:
    static void sendOpcodeP(const UnitOpcode* opcode)
    {
        opcode_encoder.encode(opcode);
    }
    static void processNetMessage(const NetMessage *net_message, size_t size);
    static void destroyPlayerUnits(PlayerID player_id);
};

#endif // ** _UNITINTERFACE_HPP
