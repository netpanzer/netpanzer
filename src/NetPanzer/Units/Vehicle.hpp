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
#ifndef _VEHICLE_HPP
#define _VEHICLE_HPP
#include <string>

#include "Units/UnitBase.hpp"
#include "Util/Timer.hpp"
#include "Classes/UnitMessageTypes.hpp"
#include "Units/UnitOpcodes.hpp"
#include "Classes/AI/PathList.hpp"

#include "Classes/Sprite.hpp"
#include "Classes/SelectionBoxSprite.hpp"

enum { _control_idle,
       _control_move,
       _control_move_map_square,
       _control_turret_track_point,
       _control_turret_track_target,
       _control_gunnery_location,
       _control_gunnery_target    };

enum { _ai_command_idle,
       _ai_command_move_to_loc,
       _ai_command_attack_unit,
       _ai_command_manual_move,
       _ai_command_defend_hold };

enum { _aiFsmMoveToLoc_path_generate,
       _aiFsmMoveToLoc_check_goal,
       _aiFsmMoveToLoc_next_move,
       _aiFsmMoveToLoc_move_wait,
       _aiFsmMoveToLoc_wait_clear_loc,
       _aiFsmMoveToLoc_check_fsm_transition };

enum { _aiFsmAttackUnit_path_generate,
       _aiFsmAttackUnit_range_check,
       _aiFsmAttackUnit_idle,
       _aiFsmAttackUnit_next_move,
       _aiFsmAttackUnit_move_wait,
       _aiFsmAttackUnit_wait_clear_loc,
       _aiFsmAttackUnit_check_fsm_transition,
       _aiFsmAttackUnit_check_path_deviation };

enum { _aiFsmDefendHold_search_for_enemy,
       _aiFsmDefendHold_attack_enemy };

enum { _aiFsmManualMove_next_move,
       _aiFsmManualMove_move_wait,
       _aiFsmManualMove_check_fsm_transition };

enum { _external_event_null,
       _external_event_pending_unit_destruct };

enum { _rotate_pos, _rotate_neg };

class Vehicle : public UnitBase
{
protected:
    SpritePacked body_anim;
    SpritePacked turret_anim;

    SpritePacked body_anim_shadow;
    SpritePacked turret_anim_shadow;

    UnitSelectionBox select_info_box;

    std::string soundSelect;
    std::string fireSound;
    unsigned short weaponType;

    Timer unit_state_timer;
    Timer fsm_timer;
    bool fsm_active_list[ 7 ];

    PathList path;
    bool path_generated;
    bool critical_ai_section;
    bool ai_fsm_transition_complete;

    unsigned short reload_counter;
    unsigned short death_counter;
    void updateUnitStateProperties();

    unsigned char ai_command_state;
    unsigned char external_ai_event;


    unsigned short shortestRotation( AngleInt &angle, long goal_angle, long *delta );
    unsigned short mapXYtoOrientation( unsigned long square, long *goal_angle  );
    void orientationToOffset( unsigned short orientation, signed char *offset_x, signed char *offset_y );
    void locationOffset( unsigned long square, iXY &offset );

    // ** FSMs and AI FSMs
    unsigned short fsmBodyRotate_rotation;
    long           fsmBodyRotate_goal_angle;
    void    setFsmBodyRotate( long goal_angle, unsigned short rotation );
    bool fsmBodyRotate();

    unsigned short fsmTurretRotate_rotation;
    long           fsmTurretRotate_goal_angle;
    void    setFsmTurretRotate( long goal_angle, unsigned short rotation );
    bool fsmTurretRotate();

    float interpolation_speed;
    TimeStamp start_move_stamp;
    TimeStamp end_move_stamp;
    bool     fsmMove_first_stamp;
    signed char fsmMove_offset_x;
    signed char fsmMove_offset_y;
    unsigned char fsmMove_moves_counter;
    unsigned char fsmMove_moves_per_square;
    void setFsmMove( unsigned short orientation );
    bool fsmMove();

    MoveOpcode move_opcode;
    Timer opcode_move_timer;
    bool move_opcode_sent;
    unsigned char fsmMoveMapSquare_movement_type;
    void setFsmMoveMapSquare( unsigned long square );
    bool fsmMoveMapSquare();

    iXY fsmTurretTrackPoint_target;
    Angle fsmTurretTrackPoint_target_angle;
    bool fsmTurretTrackPoint_on_target;
    void setFsmTurretTrackPoint(const iXY& target );
    void clearFsmTurretTrackPoint();
    void syncFsmTurretTrackPoint();
    void fsmTurretTrackPoint();

    UnitID fsmTurretTrackTarget_target_id;
    bool fsmTurretTrackTarget_on_target;
    void setFsmTurretTrackTarget(UnitID target_id);
    void clearFsmTurretTrackTarget();
    void syncFsmTurretTrackTarget();
    void fsmTurretTrackTarget();

    iXY fsmGunneryLocation_target;
    void setFsmGunneryLocation(const iXY& target );
    void clearFsmGunneryLocation();
    void fsmGunneryLocation();

    UnitID fsmGunneryTarget_target_id;
    void setFsmGunneryTarget(UnitID target_id);
    void clearFsmGunneryTarget();
    void fsmGunneryTarget();

    void aiFsmIdle();

    unsigned char aiFsmDefendHold_state;
    Timer	  aiFsmDefendHold_search_timer;
    UnitID  aiFsmDefendHold_target_ID;
    void setAiFsmDefendHold();
    void aiFsmDefendHold();

    iXY aiFsmMoveToLoc_goal;
    unsigned char aiFsmMoveToLoc_state;
    unsigned long aiFsmMoveToLoc_next_square;
    iXY aiFsmMoveToLoc_next_loc;
    iXY aiFsmMoveToLoc_prev_loc;
    Timer	   aiFsmMoveToLoc_wait_timer;
    bool  aiFsmMoveToLoc_path_not_finished;
    bool ruleMoveToLoc_GoalReached();
    void aiFsmMoveToLoc_OnExitCleanUp();
    void aiFsmMoveToLoc();

    UnitID   aiFsmAttackUnit_target_ID;
    iXY aiFsmAttackUnit_target_goal_loc;
    unsigned char aiFsmAttackUnit_state;
    unsigned long aiFsmAttackUnit_next_square;
    iXY aiFsmAttackUnit_next_loc;
    iXY aiFsmAttackUnit_prev_loc;
    Timer	   aiFsmAttackUnit_wait_timer;
    bool  aiFsmAttackUnit_path_not_finished;
    bool  aiFsmAttackUnit_target_destroyed;
    void aiFsmAttackUnit_OnExitCleanUp();
    void aiFsmAttackUnit();


    unsigned char aiFsmManualMove_move_orientation;
    unsigned char aiFsmManualMove_state;
    iXY aiFsmManualMove_next_loc;
    iXY aiFsmManualMove_prev_loc;
    void aiFsmManualMove();

    void fireWeapon( iXY &target_loc );
    virtual unsigned short launchProjectile();
    virtual void soundSelected();

    TimerFrameBase threat_level_under_attack_timer;
    void accessThreatLevels();

    void updateFsmState();
    void updateAIState();

    // ** Opcode Functions
    void unitOpcodeMove(const UnitOpcode* opcode );
    void unitOpcodeTrackPoint(const UnitOpcode* opcode );
    void unitOpcodeTrackTarget(const UnitOpcode* opcode );
    void unitOpcodeFireWeapon(const UnitOpcode* opcode );
    void unitOpcodeSync(const UnitOpcode* opcode );
    void unitOpcodeUpdateState(const UnitOpcode* opcode );
    void unitOpcodeDestruct(const UnitOpcode* opcode );

    UnitOpcodeQueue opcode_queue;
    UnitOpcodeQueue move_opcode_queue;
    void processMoveOpcodeQueue();
    void processOpcodeQueue();

    // ** Message Handlers
    UMesgAICommand pending_AI_comm_mesg;
    bool	     pending_AI_comm;
    void checkPendingAICommStatus();

    void setCommandMoveToLoc(const UMesgAICommand* message);
    void setCommandAttackUnit(const UMesgAICommand* message);
    void setCommandManualMove(const UMesgAICommand* message);
    void setCommandManualFire(const UMesgAICommand* message);

    void messageAICommand(const UnitMessage* message);
    void messageWeaponHit(const UnitMessage* message);
    void messageSelectBoxUpdate(const UnitMessage* message);
    void messageSelfDestruct(const UnitMessage* message);

    void setUnitProperties( unsigned char utype );

public:
    Vehicle(bool liveornot, PlayerState* player,
            unsigned char utype, UnitID id,
            unsigned char unit_style, bool moving, iXY initial_loc,
            AngleInt body_angle, AngleInt turret_angle,
            unsigned short orientation, unsigned short speed_rate,
            unsigned short speed_factor, unsigned short reload_time,
            short max_hit_points, short hit_points, unsigned short damage_factor,
            unsigned long weapon_range, unsigned long defend_range);

    virtual void updateState();

    virtual void processMessage(const UnitMessage* message);

    virtual void evalCommandOpcode(const UnitOpcode* opcode);

    virtual void syncUnit();

    virtual void offloadGraphics( SpriteSorter &sorter );

    float smolderWait;
    float smolderWaitMin;
/*
    static void updateLiveUnitState(UnitID id, unsigned short orientation, unsigned short speed_rate, unsigned short speed_factor,
                                      unsigned short reload_time, short max_hit_points,
                                      short hit_points, unsigned short damage_factor,
                                      unsigned long weapon_range, unsigned long defend_range);
*/

};


#endif // ** _VEHICLE_HPP
