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
#ifndef _PATHSCHEDULER_HPP
#define _PATHSCHEDULER_HPP

#include "Classes/AI/Astar.hpp"
#include "Classes/AI/PathList.hpp"
#include "Units/UnitBase.hpp"
#include "ArrayUtil/QueueTemplate.hpp"
#include "ArrayUtil/ArrayTemplate.hpp"


class PathCacheEntry
{
public:
    iXY  start;
    iXY  goal;
    PathList  path;
    bool   valid;

    inline void set( iXY start, iXY goal, PathList &path )
    {
        PathCacheEntry::start = start;
        PathCacheEntry::goal = goal;
        PathCacheEntry::path = path;
    }
};

class PathCache
{
protected:
    long start_proximity_threshold;
    long goal_proximity_threshold;

    unsigned long add_path_length_threshold;

    ArrayTemplate<PathCacheEntry> cache_list;

    unsigned long cache_size;
    unsigned long entry_replace_index;

public:
    void initialize();
    void initialize(unsigned long cache_size);

    bool search(PathRequest &path_request);
    void add(PathRequest &path_request);
};

enum { _path_generator_status_free,
       _path_generator_status_busy,
       _path_generator_status_waiting };

class PathGenerator
{
public:
    Astar astar;

protected:
    PathCache path_cache;

    enum { _pathing_fsm_none,
           _pathing_fsm_full_path,
           _pathing_fsm_update_path,
           _pathing_fsm_cache_path
         };

    enum { _pathing_fsm_state_initialize,
           _pathing_fsm_state_initialize_part_a,
           _pathing_fsm_state_generate_path,
           _pathing_fsm_state_generate_path_part_a,
           _pathing_fsm_state_initialize_part_b,
           _pathing_fsm_state_generate_path_part_b,
           _pathing_fsm_state_complete
         };

    unsigned short pathing_fsm;
    unsigned short pathing_fsm_state;

    int path_generation_status;

    PathRequest path_request;

    void pathingFsmFullPath();

    unsigned long path_update_threshold;
    unsigned long path_update_length;
    void pathingFsmUpdatePath();

    iXY working_start;
    iXY working_goal;
    unsigned long path_splice_length;
    void pathingFsmCachePath();

public:
    void initialize();
    void initialize(unsigned long resources, unsigned long cache_size);

    void initializePathGeneration( PathRequest &path_request );
    void terminatePathGeneration( UnitID &unit_id );

    inline unsigned short generationStatus()
    {
        return path_generation_status;
    }

    inline bool generationComplete( UnitID &unit_id )
    {
        if ( path_generation_status == _path_generator_status_waiting )
            if ( path_request.unit_id == unit_id ) {
                path_generation_status = _path_generator_status_free;
                return true;
            }

        return false;
    }

    void run();
};

class PathRequestQueue : public QueueTemplate< PathRequest >
{
public:
    void killRequest( UnitID &unit_id );
};

class PathScheduler
{
protected:
    static PathRequestQueue long_request_queue;
    static PathRequestQueue short_request_queue;

protected:
    static unsigned long short_pather_count;
    static unsigned long long_pather_count;

    static PathGenerator long_pather[1];
    static PathGenerator short_pather[1];

    static long short_queue_distance_threshold;
public:
    static void initialize();

    static void initialize( unsigned long short_queue_size, unsigned long long_queue_size );

    static void initialize( unsigned long short_queue_size,
                            unsigned long long_queue_size,
                            unsigned long short_pather_resources,
                            unsigned long long_pather_resources
                          );

    static void cleanUp();

    static void requestPath( PathRequest &path_request );

    static bool queryPath( UnitID &unit_id );

    static void killRequest( UnitID &unit_id );

    static void run( void );

    // ** Debug Stuff **
    static void setShortPatherDebug( bool on_off );
    static void setLongPatherDebug( bool on_off );

    static void sampleShortPather();
    static void sampleLongPather();

    static BitArray * getShortPatherSetArray( void );
    static BitArray * getLongPatherSetArray( void );
};

#endif // ** _PATHSCHEDULER_HPP
