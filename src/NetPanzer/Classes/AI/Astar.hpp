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
#ifndef _ASTAR_HPP
#define _ASTAR_HPP

#include <queue>

#include "ArrayUtil/BitArray.hpp"
#include "Interfaces/MapInterface.hpp"
#include "Units/UnitBlackBoard.hpp"
#include "Classes/AI/PathList.hpp"
#include "Units/UnitBase.hpp"

enum { _path_request_full, _path_request_update };
enum { _slot_status_free, _slot_status_busy, _slot_status_wait, _slot_status_flush };
enum { _path_result_success, _path_result_goal_unreachable };

class PathRequest
{
public:
    unsigned short status;
    unsigned short result_code;

    unsigned short request_type;
    unsigned short path_type;
    UnitID         unit_id;
    iXY       start;
    iXY       goal;
    PathList      *path;

    void set( UnitID &unit_id,
                  iXY &start,
                  iXY &goal,
                  unsigned short path_type,
                  PathList *path,
                  unsigned short request_type )
    {
        PathRequest::unit_id = unit_id;
        PathRequest::start = start;
        PathRequest::goal = goal;
        PathRequest::path_type = path_type;
        PathRequest::path = path;
        PathRequest::request_type = request_type;
        PathRequest::status = _slot_status_free;
    }
};

class AstarNode
{
public:
    iXY map_loc;
    unsigned long abs_loc;
    long f;
    long g;
    long h;
    AstarNode *parent;

    bool operator <(const AstarNode& other) const
    {
        return other.f < f;
    }
};

enum { _path_merge_front, _path_merge_rear };

class AstarNodePtrCompare
            : public std::binary_function<const AstarNode*, const AstarNode*, bool>
{
public:
    bool operator()(const AstarNode* node1, const AstarNode* node2) const
    {
        return *node1 < *node2;
    }
};

class Astar : private MapInterface
{
private:
    AstarNode *node_list;

    unsigned long node_index;
    unsigned long node_list_size;

    AstarNode *getNewNode();
    void releaseNode(AstarNode *node);
    void resetNodeList();
    void initializeNodeList(unsigned long initial_size);

    AstarNode *free_list_ptr;
    bool dynamic_node_management_flag;

protected:
    unsigned long mapXYtoAbsloc( iXY map_loc );

    AstarNode current_node;
    AstarNode goal_node;
    AstarNode *best_node;

    std::priority_queue<AstarNode*, std::vector<AstarNode*>,
                        AstarNodePtrCompare> open;

    BitArray open_set;
    BitArray closed_set;

    // ** For pathing debugging **
    BitArray astar_set_array;
    bool sample_set_array_flag;
    bool start_sampling_flag;
    bool debug_mode_flag;

    unsigned long steps;
    unsigned long step_limit;
    unsigned long total_steps;

    float total_pathing_time;

    long heuristic_weight;                // factor for heuristic   f = g() + w * h()

    bool        succ_swap_flag;
    unsigned short path_type_flag;
    bool        ini_flag;

    PathRequest *path_request_ptr;
    unsigned short path_merge_type;

    void initializePath( iXY &start, iXY &goal, unsigned short path_type );

    long heuristic( iXY &pointA, iXY &pointB );

    unsigned char generateSucc( unsigned short direction, AstarNode *node,
                                AstarNode *succ );

    bool process_succ( PathList *path, int *result_code );
    
public:
    Astar();

    void initializeAstar(unsigned long node_list_size,
                         unsigned long step_limit = 50);

    bool generatePath( PathRequest *path_request,
                       unsigned short path_merge_type,
                       bool dynamic_node_managment,
                       int *result_code );

    void cleanUp();

    void setDebugMode( bool on_off );

    void sampleSetArrays();

    BitArray * getSampledSetArrays();
};

#endif // ** _ASTAR_HPP
