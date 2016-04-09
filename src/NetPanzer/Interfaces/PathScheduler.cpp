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


#include <math.h>

#include "Util/Log.hpp"
#include "PathScheduler.hpp"
#include "Classes/AI/PathingState.hpp"
#include "Util/NTimer.hpp"

void PathCache::initialize( void )
{
    initialize( 10 );
}

void PathCache::initialize( unsigned long cache_size )
{
    PathCache::cache_size = cache_size;

    entry_replace_index = PathCache::cache_size;

    cache_list.initialize( PathCache::cache_size );

    for( unsigned long i = 0; i < PathCache::cache_size; i++ ) {
        //cache_list[ i ].path.initialize();
        cache_list[ i ].valid = false;
    }

    start_proximity_threshold = 625;	// 25 * 25
    goal_proximity_threshold  = 625;  // 25 * 25
    add_path_length_threshold = 100;
}

bool PathCache::search( PathRequest &path_request )
{

    for ( unsigned long list_index = 0; list_index < cache_size; list_index++ ) {
        long start_mag;
        long goal_mag;
        PathCacheEntry *cache_entry;

        cache_entry = &(cache_list[ list_index ]);

        if ( cache_entry->valid == true ) {
            start_mag = (long) ((cache_entry->start - path_request.start).mag2());
            goal_mag = (long) (cache_entry->goal - path_request.goal).mag2();

            if ( (start_mag <= start_proximity_threshold) && (goal_mag <= goal_proximity_threshold) ) {
                *(path_request.path) = cache_entry->path;
                return true;
            }
        } // ** if ( cache_entry->valid == true )

    } // ** for

    return false;
}

void PathCache::add(PathRequest &path_request)
{
    if (cache_size <= 0 )
        return;

    if (path_request.path->listCount() >= add_path_length_threshold) {
        entry_replace_index = (entry_replace_index % cache_size);

        cache_list[ entry_replace_index ].set( path_request.start,
                                               path_request.goal,
                                               *(path_request.path) );
        cache_list[ entry_replace_index ].valid = true;

        entry_replace_index++;
    }
}

void PathGenerator::initialize()
{
    initialize( 4000, 10 );
}

void PathGenerator::initialize(unsigned long resources, unsigned long cache_size)
{
    astar.initializeAstar( resources );
    path_cache.initialize( cache_size );

    path_generation_status = _path_generator_status_free;
    pathing_fsm = _pathing_fsm_none;

    path_update_threshold = 15;
    path_update_length = 10;
    path_splice_length = 15;
}


void PathGenerator::initializePathGeneration( PathRequest &path_request )
{
    PathGenerator::path_request = path_request;

    if( path_request.request_type == _path_request_full ) {
        if ( path_cache.search( path_request ) == true ) {
            path_generation_status = _path_generator_status_busy;
            pathing_fsm = _pathing_fsm_cache_path;
            pathing_fsm_state = _pathing_fsm_state_initialize_part_a;

            PathingState::path_cache_hits++;
        } else {
            path_generation_status = _path_generator_status_busy;
            pathing_fsm = _pathing_fsm_full_path;
            pathing_fsm_state = _pathing_fsm_state_initialize;

            PathingState::path_cache_misses++;
        }
    } else
        if ( path_request.request_type == _path_request_update ) {
            path_generation_status = _path_generator_status_busy;
            pathing_fsm = _pathing_fsm_update_path;
            pathing_fsm_state = _pathing_fsm_state_initialize;
        }
}

void PathGenerator::terminatePathGeneration( UnitID &unit_id )
{
    if ( (path_generation_status == _path_generator_status_busy) ||
            (path_generation_status == _path_generator_status_waiting)
       ) {
        if ( path_request.unit_id == unit_id )
        {
            astar.cleanUp();
            path_generation_status = _path_generator_status_free;
            pathing_fsm = _pathing_fsm_none;
        }
    }

}

void PathGenerator::pathingFsmFullPath()
{
    bool end_cycle = false;

    do {
        switch ( pathing_fsm_state ) {
        case _pathing_fsm_state_initialize : {
                path_request.path->reset();
                pathing_fsm_state = _pathing_fsm_state_generate_path;
            }
            break;

        case _pathing_fsm_state_generate_path : {
                int path_result_code;

                if ( astar.generatePath( &path_request, _path_merge_front, false, &path_result_code ) ) {
                    if ( path_result_code == _path_result_success ) {
                        path_cache.add( path_request );
                    }

                    path_generation_status = _path_generator_status_waiting;
                    pathing_fsm_state = _pathing_fsm_state_complete;

                    PathingState::path_gen_count++;
                } else {
                    end_cycle = true;
                }

            }
            break;

        case _pathing_fsm_state_complete : {
                end_cycle = true;
            }
            break;

        }
    } while( end_cycle == false );

}

void PathGenerator::pathingFsmUpdatePath()
{
    bool end_cycle = false;

    do {
        switch ( pathing_fsm_state ) {
        case _pathing_fsm_state_initialize : {
                unsigned long abs_new_goal = 0;
                iXY new_goal;
                unsigned long path_length;

                path_length = path_request.path->listCount();

                if ( path_length <= path_update_threshold ) {
                    path_request.path->reset();
                } else {
                    for ( unsigned long i = 0; i < path_update_length; i++ ) {
                        path_request.path->popFirst( &abs_new_goal );
                    }

                    MapInterface::offsetToMapXY( abs_new_goal, new_goal);
                    path_request.goal = new_goal;
                }

                pathing_fsm_state = _pathing_fsm_state_generate_path;
            }
            break;

        case _pathing_fsm_state_generate_path : {
                int path_result_code;

                if ( astar.generatePath( &path_request, _path_merge_front, false, &path_result_code ) ) {
                    path_generation_status = _path_generator_status_waiting;
                    pathing_fsm_state = _pathing_fsm_state_complete;

                    PathingState::update_gen_count++;
                } else {
                    end_cycle = true;
                }

            }
            break;

        case _pathing_fsm_state_complete : {
                end_cycle = true;
            }
            break;

        }
    } while( end_cycle == false );

}

void PathGenerator::pathingFsmCachePath()
{
    bool end_cycle = false;

    do {
        switch ( pathing_fsm_state ) {
        case _pathing_fsm_state_initialize_part_a : {
                unsigned long abs_new_goal = 0;
                iXY new_goal;

                working_start = path_request.start;
                working_goal  = path_request.goal;

                for ( unsigned long i = 0; i < path_splice_length; i++ ) {
                    path_request.path->popFirst( &abs_new_goal );
                }

                MapInterface::offsetToMapXY( abs_new_goal, new_goal);
                path_request.goal = new_goal;

                pathing_fsm_state = _pathing_fsm_state_generate_path_part_a;
            }
            break;


        case _pathing_fsm_state_generate_path_part_a : {
                int path_result_code;

                if ( astar.generatePath( &path_request, _path_merge_front, false, &path_result_code ) ) {
                    pathing_fsm_state = _pathing_fsm_state_initialize_part_b;

                    PathingState::update_gen_count++;
                } else {
                    end_cycle = true;
                }
            }
            break;


        case _pathing_fsm_state_initialize_part_b : {
                unsigned long abs_new_goal = 0;
                iXY new_goal;

                for ( unsigned long i = 0; i < path_splice_length; i++ ) {
                    path_request.path->popLast( &abs_new_goal );
                }

                MapInterface::offsetToMapXY( abs_new_goal, new_goal);
                path_request.goal = new_goal;
                path_request.start = working_goal;

                pathing_fsm_state = _pathing_fsm_state_generate_path_part_b;
            }
            break;

        case _pathing_fsm_state_generate_path_part_b : {
                int path_result_code;

                if ( astar.generatePath( &path_request, _path_merge_rear, false, &path_result_code ) ) {
                    path_request.goal = working_goal;
                    path_request.start = working_start;
                    path_generation_status = _path_generator_status_waiting;
                    pathing_fsm_state = _pathing_fsm_state_complete;

                    PathingState::update_gen_count++;
                } else {
                    end_cycle = true;
                }
            }
            break;

        case _pathing_fsm_state_complete : {
                end_cycle = true;
            }
            break;

        }
    } while( end_cycle == false );

}


void PathGenerator::run( )
{
    switch( pathing_fsm ) {
    case _pathing_fsm_full_path :
        pathingFsmFullPath();
        break;

    case _pathing_fsm_update_path :
        pathingFsmUpdatePath();
        break;

    case _pathing_fsm_cache_path :
        pathingFsmCachePath();
        break;
    } // ** switch

}


void PathRequestQueue::killRequest( UnitID &unit_id )
{
    unsigned long index;

    index = front;

    while( index != rear ) {
        index = (index + 1) % size;

        if ( (array[ index ].unit_id == unit_id ) ) {
            array[ index ].status = _slot_status_flush;
        }
    }
}


PathRequestQueue PathScheduler::long_request_queue;
PathRequestQueue PathScheduler::short_request_queue;


unsigned long PathScheduler::short_pather_count;
unsigned long PathScheduler::long_pather_count;

PathGenerator PathScheduler::long_pather[1];
PathGenerator PathScheduler::short_pather[1];

long PathScheduler::short_queue_distance_threshold;

void PathScheduler::initialize()
{
    if( MapInterface::isMapLoaded() == true ) {
        unsigned long resources;
        size_t path_list_size;
        float map_x_size = MapInterface::getWidth();
        float map_y_size = MapInterface::getHeight();
        float map_size = (map_x_size * map_y_size);

        resources = (unsigned long) ( (map_size * 0.019018) + 4018.0 );

        path_list_size = (size_t) sqrt( (map_x_size * map_x_size) + (map_y_size * map_y_size) );
        path_list_size += size_t(0.3 * path_list_size);

        if ( path_list_size > 5000 ) {
            //DEFAULT_PATH_LIST_SIZE = (unsigned long) (path_list_size);
            LOG(("Warning: PathListSite>5000..."));
        }

        initialize( 200, 200, 2000, resources );

    } else {
        initialize( 200, 200, 2000, 8000 );
    }
}

void PathScheduler::initialize( unsigned long short_queue_size, unsigned long long_queue_size )
{
    initialize( short_queue_size, long_queue_size, 2000, 8000 );
}

void PathScheduler::initialize( unsigned long short_queue_size,
                                unsigned long long_queue_size,
                                unsigned long short_pather_resources,
                                unsigned long long_pather_resources
                              )
{
    short_pather_count = 1;
    long_pather_count  = 1;

    short_request_queue.initialize( short_queue_size );
    long_request_queue.initialize( long_queue_size );

    unsigned long i;

    for ( i = 0; i < short_pather_count; i++ ) {
        short_pather[ i ].initialize( short_pather_resources, 0 );
    }

    for ( i = 0; i < long_pather_count; i++ ) {
        long_pather[ i ].initialize( long_pather_resources, 10);
    }

    short_queue_distance_threshold = 900; // 30 * 30
}

void PathScheduler::cleanUp( void )
{
    short_request_queue.deallocate();
    long_request_queue.deallocate();
}

void PathScheduler::requestPath( PathRequest &path_request )
{
    long path_distance_estimate;

    path_distance_estimate
        = long ((path_request.start - path_request.goal).mag2());

    if ( path_distance_estimate <= short_queue_distance_threshold ) {
        short_request_queue.enqueue( path_request );
    } else {
        long_request_queue.enqueue( path_request );
    }
}

bool PathScheduler::queryPath( UnitID &unit_id )
{
    unsigned long i;

    for ( i = 0; i < short_pather_count; i++ ) {
        if ( short_pather[ i ].generationComplete( unit_id ) ) {
            return (true );
        }
    }

    for ( i = 0; i < long_pather_count; i++ ) {
        if ( long_pather[ i ].generationComplete( unit_id ) ) {
            return (true );
        }
    }

    return ( false );
}

void PathScheduler::killRequest( UnitID &unit_id )
{
    unsigned long i;

    short_request_queue.killRequest( unit_id );
    long_request_queue.killRequest( unit_id );

    for ( i = 0; i < short_pather_count; i++ ) {
        short_pather[i].terminatePathGeneration( unit_id );
    }

    for ( i = 0; i < long_pather_count; i++  ) {
        long_pather[i].terminatePathGeneration( unit_id );
    }

}

void PathScheduler::run()
{
    unsigned long i;
    PathRequest request;

    for ( i = 0; i < short_pather_count; i++	) {
        if( short_pather[ i ].generationStatus() == _path_generator_status_free) {
            if ( short_request_queue.isReady() ) {
                request = short_request_queue.dequeue();

                if ( request.status != _slot_status_flush ) {
                    short_pather[ i ].initializePathGeneration( request );
                }
            }
        }

        short_pather[ i ].run();
    }

    for ( i = 0; i < long_pather_count; i++  ) {
        if( long_pather[ i ].generationStatus() == _path_generator_status_free) {
            if ( long_request_queue.isReady() ) {
                request = long_request_queue.dequeue();

                if ( request.status != _slot_status_flush ) {
                    long_pather[ i ].initializePathGeneration( request );
                }
            }
        }

        long_pather[ i ].run();
    }

}

void PathScheduler::setShortPatherDebug( bool on_off )
{
    short_pather[0].astar.setDebugMode( on_off );
}

void PathScheduler::setLongPatherDebug( bool on_off )
{
    long_pather[0].astar.setDebugMode( on_off );
}

void PathScheduler::sampleShortPather()
{
    short_pather[0].astar.sampleSetArrays( );
}

void PathScheduler::sampleLongPather()
{
    long_pather[0].astar.sampleSetArrays( );
}

BitArray * PathScheduler::getShortPatherSetArray( void )
{
    return( short_pather[0].astar.getSampledSetArrays() );
}

BitArray * PathScheduler::getLongPatherSetArray( void )
{
    return( long_pather[0].astar.getSampledSetArrays() );
}
