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

#include <string.h>
#include "Util/Log.hpp"
#include "Units/UnitBlackBoard.hpp"
#include "Units/Vehicle.hpp"
#include "Units/UnitGlobals.hpp"
#include "Classes/Network/NetworkState.hpp"
#include "Interfaces/PathScheduler.hpp"
#include "Interfaces/MapInterface.hpp"
#include "Weapons/ProjectileInterface.hpp"
#include "Units/UnitInterface.hpp"
#include "Units/UnitProfileInterface.hpp"
#include "System/Sound.hpp"


// NOTE: Temp unit new sprites put in
#include "Interfaces/GameConfig.hpp"

#include "Interfaces/ConsoleInterface.hpp"
#include "Particles/ParticleInterface.hpp"
#include "Interfaces/WorldViewInterface.hpp"

/*
Unit speeds as the profiles says:
  Archer   16 x 2 = 32 px/s (1.0   tile/second)
  Bear     14 x 2 = 28 px/s (0.875 t/s)
  Bobcat   20 x 4 = 80 px/s (2.5   t/s)
  Drake    16 x 2 = 32 px/s (1.0   t/s)
  Manta    20 x 4 = 80 px/s (2.5   t/s)
  Panther1 18 x 2 = 36 px/s (1.125 t/s)
  SPanzer  22 x 4 = 88 px/s (2.75  t/s)
  Stinger  18 x 2 = 36 px/s (1.125 t/s)
  Titan    16 x 2 = 32 px/s (1.0   t/s)
  Wolf     20 x 4 = 80 px/s (2.5   t/s)

UnitState timer ticks 10 times per second, so reload times are:
  Archer   110 = 11.0 s
  Bear      40 =  4.0 s
  Bobcat    20 =  2.0 s
  Drake     50 =  5.0 s
  Manta     35 =  3.5 s
  Panther1  50 =  5.0 s
  SPanzer   20 =  2.0 s
  Stinger  100 = 10.0 s
  Titan     60 =  6.0 s
  Wolf      40 =  4.0 s


*/

#define MOVEWAIT_TIME 0.9f

enum{ _rotate_and_move, _rotate_stop_move };

Vehicle::Vehicle(bool liveornot,
                 PlayerState* player, unsigned char utype,
                 UnitID id, unsigned char unit_style,  bool moving, iXY initial_loc,
                 AngleInt body_angle, AngleInt turret_angle,
                 unsigned short orientation, unsigned short speed_rate,
                 unsigned short speed_factor, unsigned short reload_time,
                 short max_hit_points, short hit_points, unsigned short damage_factor,
                 unsigned long weapon_range, unsigned long defend_range) : UnitBase(player, id)
{
    smolderWait    = 0.0f;
    smolderWaitMin = 0.0f;

    if(!MapInterface::inside(initial_loc))
        throw std::runtime_error("Invalid position");

    iXY loc;
    MapInterface::mapXYtoPointXY(initial_loc, loc);
    unit_state.location = loc;
    UnitBlackBoard::markUnitLoc( initial_loc );
    fsm_timer.changeRate( 10 );
    unit_state_timer.changeRate( 10 );
    setAiFsmDefendHold();
    pending_AI_comm = false;
    unit_state.lifecycle_state = _UNIT_LIFECYCLE_ACTIVE;
    memset( fsm_active_list, 0, sizeof( bool ) * 7 );

    in_sync_flag = true;


    if ( MapInterface::units_shadow_blending == 0 ) {

    body_anim_shadow.setDrawModeBlend(&Palette::colorTableDarkenALot);
    turret_anim_shadow.setDrawModeBlend(&Palette::colorTableDarkenALot);

    } else {

    body_anim_shadow.setDrawModeBlend(&Palette::colorTableDarkenALittle);
    turret_anim_shadow.setDrawModeBlend(&Palette::colorTableDarkenALittle);

    }
    //body_anim_shadow.setDrawModeBlend(&Palette::colorTableDarkenALot);
    //turret_anim_shadow.setDrawModeBlend(&Palette::colorTableDarkenALot);

    path_generated = false;
    critical_ai_section = false;
    ai_fsm_transition_complete = false;

    reload_counter = 0;
    death_counter = 0;

    ai_command_state = 0;
    external_ai_event = _external_event_null;

    fsmBodyRotate_rotation = 0;
    fsmBodyRotate_goal_angle = 0;
    fsmTurretRotate_rotation = 0;
    fsmTurretRotate_goal_angle = 0;

    interpolation_speed = 0;
    fsmMove_first_stamp = false;
    fsmMove_offset_x = fsmMove_offset_y = 0;
    fsmMove_moves_counter = 0;
    fsmMove_moves_per_square = 0;

    move_opcode_sent = false;
    fsmMoveMapSquare_movement_type = 0;

    aiFsmDefendHold_state = 0;

    // from unit profiles
    if (liveornot) {


    UnitProfile *profile;

    profile = UnitProfileInterface::getUnitProfile( utype );
/*
    unit_state.hit_points = profile->hit_points;
    unit_state.max_hit_points = profile->hit_points;
    unit_state.damage_factor = profile->attack_factor;
    unit_state.defend_range = profile->defend_range;
    unit_state.speed_factor = profile->speed_factor;
    unit_state.speed_rate = profile->speed_rate;
    unit_state.reload_time = profile->reload_time;
    unit_state.weapon_range = profile->attack_range;
    unit_state.unit_type = utype;
*/
    // perfect sync of existing units for a newly connected player
    unit_state.hit_points = hit_points;
    unit_state.max_hit_points = hit_points;
    unit_state.damage_factor = damage_factor;
    unit_state.defend_range = defend_range;
    unit_state.speed_factor = speed_factor;
    unit_state.speed_rate = speed_rate;
    unit_state.reload_time = reload_time;
    unit_state.weapon_range = weapon_range;
    unit_state.unit_type = utype;

    unit_state.body_angle = body_angle;
    unit_state.turret_angle = turret_angle;

    unit_state.unit_style = unit_style;

    unit_state.moving = moving;







    NPString unitName = profile->unitname;
    NPString file_path = "units/profiles/";
    file_path += unitName;
    file_path += ".upf";


    NPString spath;
    unsigned char stylesnum;
    if ( NetworkState::status == _network_state_server )
    {
    spath = GameConfig::getUnitStyle(unit_state.unit_style);
    stylesnum = GameConfig::getUnitStylesNum();
    } else {
    spath = GameManager::stlist[unit_state.unit_style];
    stylesnum = GameManager::ststylesnum;
    }

    NPString ustylepath = "units/pics/pak/" + spath + "/";


    //LOGGER.info("live path: %s", (ustylepath+profile->bodySprite_name).c_str());

/*
    profile->bodySprite.load(ustylepath+profile->bodySprite_name);
    profile->bodyShadow.load(ustylepath+profile->bodyShadow_name);
    profile->turretSprite.load(ustylepath+profile->turretSprite_name);
    profile->turretShadow.load(ustylepath+profile->turretShadow_name);
*/




    select_info_box.setHitBarAttributes( profile->hit_points, Color::yellow );



    body_anim.setData( UnitProfileSprites::profiles_sprites[utype*stylesnum+unit_style]->bodySprite );
    body_anim_shadow.setData( UnitProfileSprites::profiles_sprites[utype*stylesnum+unit_style]->bodyShadow );
    turret_anim.setData( UnitProfileSprites::profiles_sprites[utype*stylesnum+unit_style]->turretSprite );
    turret_anim_shadow.setData( UnitProfileSprites::profiles_sprites[utype*stylesnum+unit_style]->turretShadow );




    soundSelect = profile->soundSelected;
    fireSound = profile->fireSound;
    if ( profile->weaponType == "QUADMISSILE" )
    {
        weaponType = Weapon::_quad_missile;
    }
    else if ( profile->weaponType == "BULLET" )
    {
        weaponType = Weapon::_bullet;
    }
    else if ( profile->weaponType == "SHELL" )
    {
        weaponType = Weapon::_shell;
    }
    else if ( profile->weaponType == "DOUBLEMISSILE" )
    {
        weaponType = Weapon::_double_missile;
    }
    else
    {
        weaponType = Weapon::_bullet;
    }
    int bsize = profile->boundBox / 2;
    select_info_box.setBoxAttributes( BoundBox( -bsize, -bsize, bsize, bsize), Color::blue);


    if ( NetworkState::status == _network_state_server )
    {
    if( unit_state_timer.count() ) {

    //UnitBase* unitlive = UnitInterface::getUnit( id );
    //UnitState& unit_state = unitlive->unit_state;

    updateUnitStateProperties();

    }

    updateAIState();
    checkPendingAICommStatus();
    } else {
    processOpcodeQueue();
    }

    if( unit_state.hit_points < unit_state.max_hit_points )
    {
        smolderWait += TimerInterface::getTimeSlice();

        int intPercent = unit_state.percentDamageInt();

        if (intPercent > 50 && (rand() % 100) < intPercent)
        {
            if (smolderWait > smolderWaitMin)
            {
                float percent = unit_state.percentDamageFloat();

                smolderWaitMin  = (float(100 - percent) / 100.0) + (float(100 - percent) / 100.0) * 0.3;

                ParticleInterface::addUnitDamagePuffParticle(unit_state);

                smolderWait = 0.0f;
            }
        }
    }


    } else {
    setUnitProperties(utype);
    }


    iXY zero;
    zero.zero();
    body_anim.setAttrib( zero, zero, unitLayer );
    turret_anim.setAttrib( zero, zero, unitLayer );
    body_anim_shadow.setAttrib( zero, zero, unitLayer);
    turret_anim_shadow.setAttrib( zero, zero, unitLayer);

    select_info_box.setBoxState( false );
    select_info_box.setFlag( player->getFlag() );
    select_info_box.setName( player->getName() );

    body_anim_shadow.attachSprite( &body_anim, zero );
    body_anim_shadow.attachSprite( &turret_anim_shadow, zero );
    body_anim_shadow.attachSprite( &turret_anim, zero );
    body_anim_shadow.attachSprite( &select_info_box, zero );

    aiFsmMoveToLoc_wait_timer.changePeriod( MOVEWAIT_TIME );
}

void Vehicle::setUnitProperties( unsigned char utype )
{
    UnitProfile *profile;

    profile = UnitProfileInterface::getUnitProfile( utype );

    unit_state.hit_points = profile->hit_points;
    unit_state.max_hit_points = profile->hit_points;
    unit_state.damage_factor = profile->attack_factor;
    unit_state.defend_range = profile->defend_range;
    unit_state.speed_factor = profile->speed_factor;
    unit_state.speed_rate = profile->speed_rate;
    unit_state.reload_time = profile->reload_time;
    unit_state.weapon_range = profile->attack_range;
    unit_state.unit_type = utype;

    unit_state.unit_style = player->getPlayerStyle();
    //unit_state.moving = moving;

    // full path of sprites here

    NPString unitName = profile->unitname;
    NPString file_path = "units/profiles/";
    file_path += unitName;
    file_path += ".upf";

    NPString spath;
    unsigned char stylesnum;
    if ( NetworkState::status == _network_state_server )
    {
    spath = GameConfig::getUnitStyle(unit_state.unit_style);
    stylesnum = GameConfig::getUnitStylesNum();
    } else {
    spath = GameManager::stlist[unit_state.unit_style];
    stylesnum = GameManager::ststylesnum;
    }


    NPString ustylepath = "units/pics/pak/" + spath + "/";

    //LOGGER.info("create path: %s", (ustylepath+profile->bodySprite_name).c_str());

/*
    profile->bodySprite.load(ustylepath+profile->bodySprite_name);
    profile->bodyShadow.load(ustylepath+profile->bodyShadow_name);
    profile->turretSprite.load(ustylepath+profile->turretSprite_name);
    profile->turretShadow.load(ustylepath+profile->turretShadow_name);
*/



    select_info_box.setHitBarAttributes( profile->hit_points, Color::yellow );



    body_anim.setData( UnitProfileSprites::profiles_sprites[utype*stylesnum+unit_state.unit_style]->bodySprite );
    body_anim_shadow.setData( UnitProfileSprites::profiles_sprites[utype*stylesnum+unit_state.unit_style]->bodyShadow );
    turret_anim.setData( UnitProfileSprites::profiles_sprites[utype*stylesnum+unit_state.unit_style]->turretSprite );
    turret_anim_shadow.setData( UnitProfileSprites::profiles_sprites[utype*stylesnum+unit_state.unit_style]->turretShadow );

    /*
    body_anim.setData( profile->bodySprite );
    body_anim_shadow.setData( profile->bodyShadow );
    turret_anim.setData( profile->turretSprite );
    turret_anim_shadow.setData( profile->turretShadow );
    */

    soundSelect = profile->soundSelected;
    fireSound = profile->fireSound;
    if ( profile->weaponType == "QUADMISSILE" )
    {
        weaponType = Weapon::_quad_missile;
    }
    else if ( profile->weaponType == "BULLET" )
    {
        weaponType = Weapon::_bullet;
    }
    else if ( profile->weaponType == "SHELL" )
    {
        weaponType = Weapon::_shell;
    }
    else if ( profile->weaponType == "DOUBLEMISSILE" )
    {
        weaponType = Weapon::_double_missile;
    }
    else
    {
        weaponType = Weapon::_bullet;
    }
    int bsize = profile->boundBox / 2;
    select_info_box.setBoxAttributes( BoundBox( -bsize, -bsize, bsize, bsize), Color::blue);

 }


void Vehicle::updateUnitStateProperties()
{
    if ( reload_counter < unit_state.reload_time )
        reload_counter++;

    if ( (unit_state.lifecycle_state == _UNIT_LIFECYCLE_PENDING_DESTRUCT) &&
            (ai_command_state == _ai_command_idle)
       )
    {
        unit_state.lifecycle_state = _UNIT_LIFECYCLE_DESTROYED;
        death_counter = 0;
    }

    if ( unit_state.lifecycle_state == _UNIT_LIFECYCLE_DESTROYED )
    {
        if ( death_counter == 5 )
        {
            unit_state.lifecycle_state = _UNIT_LIFECYCLE_INACTIVE;

            DestructUnitOpcode destruct_opcode;
            destruct_opcode.setUnitID(id);
            UnitInterface::sendOpcode(&destruct_opcode);
        }
        else
        {
            death_counter++;
        }
    }
}
/*
void Vehicle::updateLiveUnitState(UnitID id, unsigned short orientation, unsigned short speed_rate, unsigned short speed_factor,
                                      unsigned short reload_time, short max_hit_points,
                                      short hit_points, unsigned short damage_factor,
                                      unsigned long weapon_range, unsigned long defend_range)
{
    //if( unit_state_timer.count() ) {

        UnitBase* unitx = UnitInterface::getUnit(id);
        UnitState& unit_state = unitx->unit_state;

        unit_state.orientation = orientation;
        unit_state.speed_rate = speed_rate;
        unit_state.speed_factor = speed_factor;
        unit_state.reload_time = reload_time;
        unit_state.max_hit_points = max_hit_points;
        unit_state.hit_points = hit_points;
        unit_state.damage_factor = damage_factor;
        unit_state.weapon_range = weapon_range;
        unit_state.defend_range = defend_range;

    //}
}
*/
void Vehicle::orientationToOffset( unsigned short orientation, signed char *offset_x, signed char *offset_y )
{
    switch ( orientation )
    {
        case 0:
            *offset_x =  1; *offset_y =  0;
            break;
        case 1:
            *offset_x =  1; *offset_y = -1;
            break;
        case 2:
            *offset_x =  0; *offset_y = -1;
            break;
        case 3:
            *offset_x = -1; *offset_y = -1;
            break;
        case 4:
            *offset_x = -1; *offset_y =  0;
            break;
        case 5:
            *offset_x = -1; *offset_y =  1;
            break;
        case 6:
            *offset_x =  0; *offset_y =  1;
            break;
        case 7:
            *offset_x =  1; *offset_y =  1;
            break;
    }

}

unsigned short Vehicle::mapXYtoOrientation( unsigned long square, long *goal_angle )
{
    iXY current_loc, next_loc;

    MapInterface::pointXYtoMapXY( unit_state.location, current_loc );
    MapInterface::offsetToMapXY( square, next_loc );

    // so many magic numbers
    if ( (next_loc.x > current_loc.x)  &&  (next_loc.y == current_loc.y) )
    {
        *goal_angle = 0;
        return( 0 );
    }

    if ( (next_loc.x > current_loc.x)  &&  (next_loc.y < current_loc.y) )
    {
        *goal_angle = 5;
        return( 1 );
    }

    if ( (next_loc.x == current_loc.x)  &&  (next_loc.y < current_loc.y) )
    {
        *goal_angle = 9;
        return( 2 );
    }

    if ( (next_loc.x < current_loc.x)  &&  (next_loc.y < current_loc.y) )
    {
        *goal_angle = 14;
        return( 3 );
    }

    if ( (next_loc.x < current_loc.x)  &&  (next_loc.y  == current_loc.y) )
    {
        *goal_angle = 18;
        return( 4 );
    }

    if ( (next_loc.x < current_loc.x)  &&  (next_loc.y > current_loc.y) )
    {
        *goal_angle = 23;
        return( 5 );
    }

    if ( (next_loc.x == current_loc.x)  &&  (next_loc.y > current_loc.y) )
    {
        *goal_angle = 27;
        return( 6 );
    }

    if ( (next_loc.x > current_loc.x)  &&  (next_loc.y > current_loc.y) )
    {
        *goal_angle = 32;
        return( 7 );
    }

    return( 0xFFFF );


}

unsigned short Vehicle::shortestRotation( AngleInt &angle, long goal_angle, long *delta )
{
    long delta_plus, delta_minus;

    if( goal_angle > angle.angle_int )
        delta_plus  = ( goal_angle ) - (angle.angle_int) ;
    else
        delta_plus  = (angle.angle_limit + goal_angle) - angle.angle_int;

    if ( goal_angle > angle.angle_int )
        delta_minus = angle.angle_limit - (goal_angle  -  angle.angle_int) ;
    else
        delta_minus = (angle.angle_int ) - ( goal_angle );

    if (delta_minus > delta_plus)
    {
        *delta = delta_plus;
        return( _rotate_pos );
    }
    else
    {
        *delta = delta_minus;
        return( _rotate_neg );
    }
}

void Vehicle::locationOffset( unsigned long square, iXY &offset )
{
    iXY square_map_loc;
    iXY unit_map_loc;

    MapInterface::offsetToMapXY( square, square_map_loc );
    MapInterface::pointXYtoMapXY( unit_state.location, unit_map_loc );

    offset = unit_map_loc - square_map_loc ;
}

void Vehicle::setFsmBodyRotate( long goal_angle, unsigned short rotation )
{
    fsmBodyRotate_rotation = rotation;
    fsmBodyRotate_goal_angle = goal_angle;
}

bool Vehicle::fsmBodyRotate()
{
    if( unit_state.body_angle.angle_int != fsmBodyRotate_goal_angle )
    {
        if ( fsmBodyRotate_rotation == _rotate_pos )
        {
            ++unit_state.body_angle;
        }
        else
        {
            --unit_state.body_angle;
        }  // ** else turn_rotation **
    }
    else
    {
        return( true );   // goal direction achieved
    }

    return( false );
}

void Vehicle::setFsmTurretRotate( long goal_angle, unsigned short rotation )
{
    fsmTurretRotate_rotation = rotation;
    fsmTurretRotate_goal_angle = goal_angle;
}

bool Vehicle::fsmTurretRotate()
{
    if( unit_state.turret_angle.angle_int != fsmTurretRotate_goal_angle )
    {
        if ( fsmTurretRotate_rotation == _rotate_pos )
        {
            ++unit_state.turret_angle;
        }
        else
        {
            --unit_state.turret_angle;
        }  // ** else turn_rotation **
    }
    else
    {
        return( true );   // goal direction achieved
    }

    return( false );
}

void Vehicle::setFsmMove( unsigned short orientation )
{
    fsm_timer.changeRate( unit_state.speed_rate );
    orientationToOffset( orientation, &fsmMove_offset_x, &fsmMove_offset_y );
    fsmMove_moves_counter = 0;
    fsmMove_moves_per_square = 32 / unit_state.speed_factor;
    /*
    fsmMove_moves_per_square = 32;
    interpolation_speed = unit_state.speed_rate * unit_state.speed_factor;
    if ( NetworkState::status == _network_state_client )
     { interpolation_speed += 2; }
    fsmMove_first_stamp = true;
    */
    //Server here!
    //LOGGER.info("Unit speed rate=%d | speed_factor=%d", (unsigned short)unit_state.speed_rate, (unsigned short)unit_state.speed_factor);
}

bool Vehicle::fsmMove()
{
    /*
    long move_offset;

    if( fsmMove_first_stamp == true )
       {
        start_move_stamp = now();
        fsmMove_first_stamp = false;
       }

    end_move_stamp = now();

    move_offset = (end_move_stamp - start_move_stamp) * interpolation_speed;

    if( (move_offset + fsmMove_moves_counter) > fsmMove_moves_per_square )
     {
      move_offset = fsmMove_moves_per_square - fsmMove_moves_counter;
     }
    */

    if ( fsmMove_moves_counter < fsmMove_moves_per_square )
    {
        /*
        fsmMove_moves_counter += move_offset;

        unit_state.location.x = unit_state.location.x + ( move_offset * fsmMove_offset_x );

        unit_state.location.y = unit_state.location.y + ( move_offset * fsmMove_offset_y );
        */

        fsmMove_moves_counter++;

        unit_state.location.x = unit_state.location.x + ( unit_state.speed_factor * fsmMove_offset_x );

        unit_state.location.y = unit_state.location.y + ( unit_state.speed_factor * fsmMove_offset_y );

        if ( NetworkState::status == _network_state_client ) {
        ParticleInterface::addMoveDirtPuff(unit_state);
        }
        //start_move_stamp = now();
        unit_state.moving = true;
    }
    //LOGGER.info("moves_counter=%d | moves_per_square=%d", (unsigned short)fsmMove_moves_counter, (unsigned short)fsmMove_moves_per_square);
    if( fsmMove_moves_counter >= fsmMove_moves_per_square)
    {
        //fsmMove_first_stamp = true;
        unit_state.moving = false;
        fsm_timer.changeRate( 10 );
        return( true );
    }

    return( false );
}

void Vehicle::setFsmMoveMapSquare( unsigned long square )
{
    long goal_angle = 0;
    long delta;
    unsigned short rotation;

    unit_state.orientation = mapXYtoOrientation( square, &goal_angle );
    rotation = shortestRotation( unit_state.body_angle, goal_angle, &delta );

    setFsmBodyRotate( goal_angle, rotation );
    setFsmMove( unit_state.orientation );

    if ( delta <= 9 )
        fsmMoveMapSquare_movement_type = _rotate_and_move;
    else
        fsmMoveMapSquare_movement_type = _rotate_stop_move;

    fsm_active_list[ _control_move_map_square ] = true;
    critical_ai_section = true;

    if ( NetworkState::status == _network_state_server )
    {
        iXY loc_offset;

        move_opcode.opcode = _UNIT_OPCODE_MOVE;
        move_opcode.setUnitID(id);
        move_opcode.setSquare(square);
        locationOffset( square, loc_offset );
        move_opcode.loc_x_offset = (signed char) loc_offset.x;
        move_opcode.loc_y_offset = (signed char) loc_offset.y;
        if ( move_opcode_sent == true )
        {
            UnitInterface::sendOpcode(&move_opcode);
        }
    }

}




bool Vehicle::fsmMoveMapSquare()
{
    if ( move_opcode_sent == false && NetworkState::status == _network_state_server )
    {
        if ( opcode_move_timer.count() )
        {
            UnitInterface::sendOpcode(&move_opcode);
            move_opcode_sent = true;
        }
    }

    switch( fsmMoveMapSquare_movement_type )
    {
        case  _rotate_stop_move :
        {
            if ( fsmBodyRotate() )
            {
                if ( fsmMove() )
                {
                    fsm_active_list[ _control_move_map_square ] = false;
                    critical_ai_section = false;

                    if ( move_opcode_sent == false && NetworkState::status == _network_state_server )
                    {
                        UnitInterface::sendOpcode(&move_opcode);
                        move_opcode_sent = true;
                    }

                    return( true );
                }
            }
        }
        break;

        case _rotate_and_move :
        {
            fsmBodyRotate();
            if ( fsmMove() )
            {
                fsm_active_list[ _control_move_map_square ] = false;
                critical_ai_section = false;

                if ( move_opcode_sent == false && NetworkState::status == _network_state_server )
                {
                    UnitInterface::sendOpcode(&move_opcode);
                    move_opcode_sent = true;
                }

                return( true );
            }
        }
        break;
    } // ** switch

    return( false );
}


void Vehicle::setFsmTurretTrackPoint(const iXY& target)
{
    iXY direction_vector;

    fsmTurretTrackPoint_target = target;
    direction_vector = fsmTurretTrackPoint_target - unit_state.location;
    fsmTurretTrackPoint_target_angle.set( direction_vector );

    fsmTurretTrackPoint_on_target = false;

    fsm_active_list[ _control_turret_track_point ] = true;

    if ( NetworkState::status == _network_state_server )
    {
        TurretTrackPointOpcode track_point_opcode;
        track_point_opcode.opcode = _UNIT_OPCODE_TURRET_TRACK_POINT;
        track_point_opcode.setUnitID(id);
        track_point_opcode.setTarget(target);
        track_point_opcode.activate = true;
        UnitInterface::sendOpcode( &track_point_opcode );
    }

}

void Vehicle::clearFsmTurretTrackPoint()
{
    fsm_active_list[ _control_turret_track_point ] = false;

    fsmTurretTrackPoint_on_target = false;

    if ( NetworkState::status == _network_state_server )
    {
        TurretTrackPointOpcode track_point_opcode;
        track_point_opcode.opcode = _UNIT_OPCODE_TURRET_TRACK_POINT;
        track_point_opcode.setUnitID(id);
        track_point_opcode.activate = false;
        UnitInterface::sendOpcode( &track_point_opcode );
    }
}

void Vehicle::syncFsmTurretTrackPoint()
{
    if ( fsm_active_list[ _control_turret_track_point ] == true )
    {
        TurretTrackPointOpcode track_point_opcode;
        track_point_opcode.flags = _unit_opcode_flag_sync;
        track_point_opcode.opcode = _UNIT_OPCODE_TURRET_TRACK_POINT;
        track_point_opcode.setUnitID(id);
        track_point_opcode.setTarget(fsmTurretTrackPoint_target);
        track_point_opcode.activate = true;

        UnitInterface::sendOpcode( &track_point_opcode );
    }
}

void Vehicle::fsmTurretTrackPoint()
{
    long goal_angle;
    long delta;
    unsigned short rotation;
    iXY direction_vector;

    direction_vector = fsmTurretTrackPoint_target - unit_state.location;
    fsmTurretTrackPoint_target_angle.set( direction_vector );

    goal_angle = fsmTurretTrackPoint_target_angle.DegreesInt( 10 );
    rotation = shortestRotation( unit_state.turret_angle, goal_angle, &delta );
    setFsmTurretRotate( goal_angle, rotation );

    if (  fsmTurretRotate() == true )
        fsmTurretTrackPoint_on_target = true;
    else
        fsmTurretTrackPoint_on_target = false;

}

void Vehicle::setFsmTurretTrackTarget(UnitID target_id)
{
    fsmTurretTrackTarget_target_id = target_id;
    fsm_active_list[ _control_turret_track_target ] = true;

    if ( NetworkState::status == _network_state_server )
    {
        TurretTrackTargetOpcode track_target_opcode;
        track_target_opcode.opcode  = _UNIT_OPCODE_TURRET_TRACK_TARGET;
        track_target_opcode.setUnitID(id);
        track_target_opcode.setTargetUnitID(target_id);
        track_target_opcode.activate = true;
        UnitInterface::sendOpcode( &track_target_opcode );
    }

}

void Vehicle::clearFsmTurretTrackTarget()
{
    fsm_active_list[ _control_turret_track_target ] = false;

    if ( NetworkState::status == _network_state_server )
    {
        TurretTrackTargetOpcode track_target_opcode;
        track_target_opcode.opcode  = _UNIT_OPCODE_TURRET_TRACK_TARGET;
        track_target_opcode.setUnitID(id);
        track_target_opcode.activate = false;
        UnitInterface::sendOpcode( &track_target_opcode );
    }

}

void Vehicle::syncFsmTurretTrackTarget()
{
    if ( fsm_active_list[ _control_turret_track_target ] == true )
    {
        TurretTrackTargetOpcode track_target_opcode;
        track_target_opcode.opcode  = _UNIT_OPCODE_TURRET_TRACK_TARGET;
        track_target_opcode.flags = _unit_opcode_flag_sync;
        track_target_opcode.setUnitID(id);
        track_target_opcode.setTargetUnitID(fsmTurretTrackTarget_target_id);
        track_target_opcode.activate = true;
        UnitInterface::sendOpcode( &track_target_opcode );
    }

}


void Vehicle::fsmTurretTrackTarget()
{
    long goal_angle;
    long delta;
    unsigned short rotation;
    UnitBase *target_unit_ptr;
    iXY direction_vector;

    target_unit_ptr = UnitInterface::getUnit( fsmTurretTrackTarget_target_id );

    if ( target_unit_ptr != 0 )
    {
        direction_vector = target_unit_ptr->unit_state.location - unit_state.location;
        Angle float_angle( direction_vector );

        goal_angle = float_angle.DegreesInt( 10 );
        rotation = shortestRotation( unit_state.turret_angle, goal_angle, &delta );
        setFsmTurretRotate( goal_angle, rotation );
    }

    if ( fsmTurretRotate() == true )
    {
        fsmTurretTrackTarget_on_target = true;
    }
    else
    {
        fsmTurretTrackTarget_on_target = false;
    }
}



void Vehicle::setFsmGunneryLocation(const iXY& target )
{
    if ( fsm_active_list[ _control_gunnery_target ] == true )
    {
        clearFsmGunneryTarget();
    }

    fsmGunneryLocation_target = target;
    setFsmTurretTrackPoint( target );
    fsm_active_list[ _control_gunnery_location ] = true;
}

void Vehicle::clearFsmGunneryLocation()
{
    fsm_active_list[ _control_gunnery_location ] = false;
    clearFsmTurretTrackPoint();
}

void Vehicle::fsmGunneryLocation()
{
    iXY range_vector;

    range_vector = fsmGunneryLocation_target - unit_state.location;
    //LOGGER.info("Unit reload=%d | Unit reload_time=%d", (unsigned short)reload_counter, (unsigned short)unit_state.reload_time);
    if ( (range_vector.mag2() < unit_state.weapon_range) &&
            (fsmTurretTrackPoint_on_target == true) &&
            (reload_counter >= unit_state.reload_time)
       )
    {
        fireWeapon( fsmGunneryLocation_target );
        clearFsmGunneryLocation();
    }

}

void Vehicle::setFsmGunneryTarget(UnitID target_id)
{
    if ( fsm_active_list[ _control_gunnery_location ] == true )
    {
        clearFsmGunneryLocation();
    }

    if ( fsm_active_list[ _control_turret_track_point ] == true )
    {
        clearFsmTurretTrackPoint();
    }

    fsmGunneryTarget_target_id = target_id;
    setFsmTurretTrackTarget( target_id );
    fsm_active_list[ _control_gunnery_target ] = true;
}

void Vehicle::clearFsmGunneryTarget()
{
    fsm_active_list[ _control_gunnery_target ] = false;
    clearFsmTurretTrackTarget();
}

void Vehicle::fsmGunneryTarget()
{
    UnitBase *target_unit_ptr;
    UnitState *target_unit_state;
    iXY range_vector;

    target_unit_ptr = UnitInterface::getUnit( fsmGunneryTarget_target_id );
    if ( target_unit_ptr == 0 )
    {
        clearFsmGunneryTarget();
    }
    else
    {
        target_unit_state = &(target_unit_ptr->unit_state);

        if ( target_unit_state->lifecycle_state == _UNIT_LIFECYCLE_DESTROYED )
        {
            clearFsmGunneryTarget();
        }
        else
        {
            range_vector = target_unit_state->location - unit_state.location;

            if ( (range_vector.mag2() < unit_state.weapon_range) &&
                    (fsmTurretTrackTarget_on_target == true) &&
                    (reload_counter == unit_state.reload_time )
               )
            {
                fireWeapon( target_unit_state->location );
            }
        }
    }


}

void Vehicle::aiFsmIdle()
{
    if ( pending_AI_comm == true )
    {
        ai_fsm_transition_complete = true;
    }

}

bool Vehicle::ruleMoveToLoc_GoalReached()
{
    iXY map_loc;
    MapInterface::pointXYtoMapXY( unit_state.location, map_loc );
    if ( map_loc == aiFsmMoveToLoc_goal )
        return true;

    return false;
}


void Vehicle::aiFsmMoveToLoc()
{
    bool end_cycle = false;

    do
    {
        switch ( aiFsmMoveToLoc_state )
        {

            // *************************************************************
            case _aiFsmMoveToLoc_path_generate :
            {
                // QueryPath: Has a path been generated for unit ?
                path_generated = PathScheduler::queryPath(id);


                if ( external_ai_event == _external_event_pending_unit_destruct  )
                {
                    // External Event: This unit is about to be deleted
                    // Action : Exit fsm gracefully
                    aiFsmMoveToLoc_OnExitCleanUp();

                    iXY current_map_loc;
                    MapInterface::pointXYtoMapXY( unit_state.location, current_map_loc );
                    UnitBlackBoard::unmarkUnitLoc( current_map_loc );

                    external_ai_event = _external_event_null;
                    ai_command_state = _ai_command_idle;
                    end_cycle = true;
                    return;
                }
                else if ( pending_AI_comm == true )
                {
                    // External Event: A new AI command is pending
                    // Action: Allow command transition to occur
                    ai_fsm_transition_complete = true;
                    aiFsmMoveToLoc_OnExitCleanUp();
                    end_cycle = true;
                    return;
                }
                else if ( path_generated == true )
                {
                    // Rule QueryPath: is true move to next state
                    //LOG( ("Path Successfully Generated") );
                    aiFsmMoveToLoc_state = _aiFsmMoveToLoc_check_goal;
                }
                else
                {
                    //LOGGER.info("Vehicle. Stop querying for path!");

                    end_cycle = true;
                    return;
                }
            }
            break;

            // *************************************************************

            case _aiFsmMoveToLoc_check_goal :
            {
                //  GoalReached: Has the goal been reached ?
                if ( ruleMoveToLoc_GoalReached() == true )
                {
                    // Rule GoalReached : is true
                    // Action : Exit fsm
                    aiFsmMoveToLoc_prev_loc = unit_state.location;
                    MapInterface::pointXYtoMapXY( aiFsmMoveToLoc_prev_loc, aiFsmMoveToLoc_prev_loc );
                    UnitBlackBoard::markUnitLoc( aiFsmMoveToLoc_prev_loc );

                    aiFsmMoveToLoc_OnExitCleanUp();
                    setAiFsmDefendHold();

                    end_cycle = true;
                    return;
                }
                else
                {
                    if ( aiFsmMoveToLoc_path_not_finished == false )
                    {
                        //  Rule: GoalReached is false AND Unit is at the end of path
                        //  Action : Request path generation to goal
                        iXY start;
                        PathRequest path_request;

                        //LOG( ("Incomplete Path -- Regenerating Path") );
                        MapInterface::pointXYtoMapXY( unit_state.location, start );
                        path_request.set(id, start, aiFsmMoveToLoc_goal, 0,  &path, _path_request_full );
                        PathScheduler::requestPath( path_request );
                        aiFsmMoveToLoc_path_not_finished = true;
                        aiFsmMoveToLoc_state = _aiFsmMoveToLoc_path_generate;
                    }
                    else
                    {
                        // Rule GoalReached: is false
                        // Action: Get next move;
                        aiFsmMoveToLoc_state = _aiFsmMoveToLoc_next_move;
                    }
                }
            }
            break;

            // *************************************************************

            case _aiFsmMoveToLoc_next_move :
            {
                // CurrentPathComplete: is Unit at the end of the current path
                aiFsmMoveToLoc_path_not_finished = path.popFirst( &aiFsmMoveToLoc_next_square );
                MapInterface::offsetToMapXY( aiFsmMoveToLoc_next_square, aiFsmMoveToLoc_next_loc );

                if ( !aiFsmMoveToLoc_path_not_finished )
                {
                    // Rule: CurrentPathComplete is true
                    // Action : check if unit is at the goal
                    aiFsmMoveToLoc_state = _aiFsmMoveToLoc_check_goal;
                }
                else
                {
                    // Rule: CurrentPathComplete is false
                    // Action: Check if next location is empty
                    aiFsmMoveToLoc_prev_loc = unit_state.location;
                    MapInterface::pointXYtoMapXY( aiFsmMoveToLoc_prev_loc, aiFsmMoveToLoc_prev_loc );
                    UnitBlackBoard::markUnitLoc( aiFsmMoveToLoc_prev_loc );

                    //aiFsmMoveToLoc_wait_timer.changePeriod( MOVEWAIT_TIME );
                    aiFsmMoveToLoc_wait_timer.reset();
                    aiFsmMoveToLoc_state = _aiFsmMoveToLoc_wait_clear_loc;
                }
            }
            break;

            // *************************************************************

            case _aiFsmMoveToLoc_wait_clear_loc :
            {

                if ( external_ai_event == _external_event_pending_unit_destruct  )
                {
                    // External Event: This unit is about to be deleted
                    // Action : Exit fsm gracefully
                    aiFsmMoveToLoc_OnExitCleanUp();
                    UnitBlackBoard::unmarkUnitLoc( aiFsmMoveToLoc_prev_loc );
                    external_ai_event = _external_event_null;
                    ai_command_state = _ai_command_idle;
                    end_cycle = true;
                    return;
                }
                else if ( pending_AI_comm == true )
                {
                    // External Event: A new AI command is pending
                    // Action: Allow command transition to occur
                    ai_fsm_transition_complete = true;
                    aiFsmMoveToLoc_OnExitCleanUp();
                    end_cycle = true;
                    return;
                } // NextSquareOccupied: does the next square contain a abstruction
                else if ( UnitBlackBoard::unitOccupiesLoc( aiFsmMoveToLoc_next_loc ) == true )
                {
                    // Rule: NextSquareOccupied is true
                    // Action: Check Wait Timer

                    if ( aiFsmMoveToLoc_wait_timer.count() )
                    {
                        // Rule: NextSquareOccupied is true AND WaitTimer is finished
                        // Action: Preform path update
                        if ( aiFsmMoveToLoc_next_loc == aiFsmMoveToLoc_goal )
                        {

                            UnitInterface::unit_placement_matrix.reset( aiFsmMoveToLoc_goal );
                            UnitInterface::unit_placement_matrix.getNextEmptyLoc( &aiFsmMoveToLoc_goal );

                            PathRequest path_request;
                            path_request.set(id, aiFsmMoveToLoc_prev_loc, aiFsmMoveToLoc_goal, 0, &path, _path_request_full );
                            PathScheduler::requestPath( path_request );
                            aiFsmMoveToLoc_wait_timer.changePeriod( MOVEWAIT_TIME );
                        }
                        else
                        {
//                            LOGGER.warning("Requesting updated path for unit %d from %d,%d to %d,%d", id,
//                                           aiFsmMoveToLoc_prev_loc.x, aiFsmMoveToLoc_prev_loc.y,
//                                           aiFsmMoveToLoc_goal.x,aiFsmMoveToLoc_goal.y);
                            PathRequest path_request;
                            path_request.set(id, aiFsmMoveToLoc_prev_loc, aiFsmMoveToLoc_goal, 0, &path, _path_request_update );
                            PathScheduler::requestPath( path_request );
                            // XXX the more times timeout the longer will take next time --> bad idea
                            aiFsmMoveToLoc_wait_timer.changePeriod( MOVEWAIT_TIME );
                        }

                        aiFsmMoveToLoc_state = _aiFsmMoveToLoc_path_generate;
                    }
                    // can't move and has to wait, finish the loop.
                    end_cycle = true;
                    return;
                }
                else
                {
                    // Rule: NextSquareOccupied is false
                    // Action: Begin move to next square
                    UnitBlackBoard::markUnitLoc( aiFsmMoveToLoc_next_loc );
                    setFsmMoveMapSquare( aiFsmMoveToLoc_next_square );

                    aiFsmMoveToLoc_state = _aiFsmMoveToLoc_move_wait;
                    aiFsmMoveToLoc_wait_timer.changePeriod( MOVEWAIT_TIME );
                }

            }
            break;

            // *************************************************************

            case _aiFsmMoveToLoc_move_wait :
            {
                // MoveFinished : has the low level fsm finshed
                if ( fsm_active_list[ _control_move_map_square ] == false )
                {
                    // Rule: MoveFinished is true
                    // Action: Check for a pending transition
                    UnitBlackBoard::unmarkUnitLoc( aiFsmMoveToLoc_prev_loc );
                    aiFsmMoveToLoc_state = _aiFsmMoveToLoc_check_fsm_transition;
                    end_cycle = true;
                }
                else
                {
                    end_cycle = true;
                    return;
                } // ** else

            }
            break;

            // *************************************************************

            case _aiFsmMoveToLoc_check_fsm_transition :
            {
                if ( external_ai_event == _external_event_pending_unit_destruct )
                {
                    // External Event: This unit is about to be deleted
                    // Action : Exit fsm gracefully
                    aiFsmMoveToLoc_OnExitCleanUp();
                    UnitBlackBoard::unmarkUnitLoc( aiFsmMoveToLoc_next_loc );
                    external_ai_event = _external_event_null;
                    ai_command_state = _ai_command_idle;
                    end_cycle = true;
                    return;
                }
                else if ( pending_AI_comm == true )
                {
                    // External Event: A new AI command is pending
                    // Action: Allow command transition to occur
                    ai_fsm_transition_complete = true;
                    aiFsmMoveToLoc_OnExitCleanUp();
                    end_cycle = true;
                    return;
                }
                else
                {
                    aiFsmMoveToLoc_state = _aiFsmMoveToLoc_check_goal;
                }

            }
            break;

        } // ** switch

    } while (end_cycle == false);
}

void Vehicle::aiFsmMoveToLoc_OnExitCleanUp()
{
    PathScheduler::killRequest(id);
    clearFsmTurretTrackPoint();
}

void Vehicle::aiFsmAttackUnit()
{
    bool end_cycle = false;

    UnitState *target_unit_state = 0;
    iXY range_vector;

    UnitBase* target_unit_ptr
        = UnitInterface::getUnit( aiFsmAttackUnit_target_ID );
    if ( target_unit_ptr == 0 )
    {
        aiFsmAttackUnit_target_destroyed = true;
        if(aiFsmAttackUnit_state != _aiFsmAttackUnit_move_wait)
        {
            setAiFsmDefendHold();
            aiFsmAttackUnit_OnExitCleanUp();
            return;
        }
    }
    else
    {
        target_unit_state = &(target_unit_ptr->unit_state);

        if ( target_unit_state->lifecycle_state == _UNIT_LIFECYCLE_DESTROYED )
        {
            aiFsmAttackUnit_target_destroyed = true;

            if ( aiFsmAttackUnit_state !=  _aiFsmAttackUnit_move_wait )
            {
                setAiFsmDefendHold();
                aiFsmAttackUnit_OnExitCleanUp();
                return;
            }
        }
    }

    do
    {
        switch ( aiFsmAttackUnit_state )
        {
            // *************************************************************

            case _aiFsmAttackUnit_path_generate :
            {
                // QueryPath: Has a path been generated for unit ?
                path_generated = PathScheduler::queryPath(id);

                if ( external_ai_event == _external_event_pending_unit_destruct )
                {
                    // External Event: This unit is about to be deleted
                    // Action : Exit fsm gracefully
                    aiFsmAttackUnit_OnExitCleanUp();

                    iXY current_map_loc;
                    MapInterface::pointXYtoMapXY( unit_state.location, current_map_loc );
                    UnitBlackBoard::unmarkUnitLoc( current_map_loc );

                    external_ai_event = _external_event_null;
                    ai_command_state = _ai_command_idle;
                    end_cycle = true;
                    return;
                }
                else if ( pending_AI_comm == true )
                {
                    // External Event: A new AI command is pending
                    // Action: Allow command transition to occur
                    ai_fsm_transition_complete = true;
                    aiFsmAttackUnit_OnExitCleanUp();
                    end_cycle = true;
                    return;
                }
                else if ( path_generated == true )
                {
                    // Rule QueryPath: is true move to next state
                    aiFsmAttackUnit_state = _aiFsmAttackUnit_range_check;
                }
                else
                {
                    end_cycle = true;
                    return;
                }

            }
            break;
            // *************************************************************

            case _aiFsmAttackUnit_range_check :
            {
                // RangeVector: the absolute distance between unit and target
                range_vector = target_unit_state->location - unit_state.location;

                if ( range_vector.mag2() < unit_state.weapon_range )
                {
                    // Rule: RangeVector < WeaponRange, unit is in range
                    // Action: Remain in position
                    aiFsmAttackUnit_prev_loc = unit_state.location;
                    MapInterface::pointXYtoMapXY( aiFsmAttackUnit_prev_loc, aiFsmAttackUnit_prev_loc );
                    UnitBlackBoard::markUnitLoc( aiFsmAttackUnit_prev_loc );
                    aiFsmAttackUnit_state = _aiFsmAttackUnit_idle;
                    end_cycle = true;
                    return;
                }
                else
                {
                    if ( aiFsmAttackUnit_path_not_finished == false )
                    {
                        iXY start;
                        MapInterface::pointXYtoMapXY( unit_state.location, start );

                        PathRequest path_request;
                        path_request.set(id, start, aiFsmAttackUnit_target_goal_loc, 0, &path, _path_request_full );
                        PathScheduler::requestPath( path_request );

                        aiFsmAttackUnit_path_not_finished = true;
                        aiFsmAttackUnit_state = _aiFsmAttackUnit_path_generate;
                    }
                    else
                    {
                        // Rule: RangeVector > WeaponRange
                        // Action: Get next move
                        aiFsmAttackUnit_state = _aiFsmAttackUnit_next_move;
                    }
                }
            }
            break;
            // *************************************************************

            case _aiFsmAttackUnit_idle :
            {
                range_vector = target_unit_state->location - unit_state.location;

                if ( external_ai_event == _external_event_pending_unit_destruct )
                {
                    // External Event: This unit is about to be deleted
                    // Action : Exit fsm gracefully
                    aiFsmAttackUnit_OnExitCleanUp();
                    UnitBlackBoard::unmarkUnitLoc( aiFsmAttackUnit_prev_loc );
                    external_ai_event = _external_event_null;
                    ai_command_state = _ai_command_idle;
                    end_cycle = true;
                    return;
                }
                else if ( pending_AI_comm == true )
                {
                    // External Event: A new AI command is pending
                    // Action: Allow command transition to occur
                    ai_fsm_transition_complete = true;
                    aiFsmAttackUnit_OnExitCleanUp();
                    end_cycle = true;
                    return;
                }
                else if ( range_vector.mag2() < unit_state.weapon_range )
                {
                    // Rule: RangeVector < WeaponRange, unit is in range
                    // Action: Remain in position
                    end_cycle = true;
                    return;
                }
                else
                {
                    // Rule: RangeVector > WeaponRange
                    // Action: Get next move
                    aiFsmAttackUnit_state = _aiFsmAttackUnit_next_move;
                }
            }
            break;

            // *************************************************************

            case _aiFsmAttackUnit_next_move :
            {
                // CurrentPathComplete: is Unit at the end of the current path
                aiFsmAttackUnit_path_not_finished = path.popFirst( &aiFsmAttackUnit_next_square );
                MapInterface::offsetToMapXY( aiFsmAttackUnit_next_square, aiFsmAttackUnit_next_loc );

                if ( !aiFsmAttackUnit_path_not_finished )
                {
                    // Rule: CurrentPathComplete is true
                    // Action : check if unit is at the goal
                    aiFsmAttackUnit_state = _aiFsmAttackUnit_range_check;
                }
                else
                {
                    // Rule: CurrentPathComplete is false
                    // Action: Check if next location is empty
                    aiFsmAttackUnit_prev_loc = unit_state.location;
                    MapInterface::pointXYtoMapXY( aiFsmAttackUnit_prev_loc, aiFsmAttackUnit_prev_loc );
                    UnitBlackBoard::markUnitLoc( aiFsmAttackUnit_prev_loc );
                    aiFsmAttackUnit_wait_timer.changePeriod( 0.8f );
                    aiFsmAttackUnit_state = _aiFsmAttackUnit_wait_clear_loc;
                }
            }
            break;

            // *************************************************************

            case _aiFsmAttackUnit_wait_clear_loc :
            {
                if ( external_ai_event == _external_event_pending_unit_destruct )
                {
                    // External Event: This unit is about to be deleted
                    // Action : Exit fsm gracefully
                    aiFsmAttackUnit_OnExitCleanUp();
                    UnitBlackBoard::unmarkUnitLoc( aiFsmAttackUnit_prev_loc );
                    external_ai_event = _external_event_null;
                    ai_command_state = _ai_command_idle;
                    end_cycle = true;
                    return;
                }
                else if ( pending_AI_comm == true )
                {
                    // External Event: A new AI command is pending
                    // Action: Allow command transition to occur
                    ai_fsm_transition_complete = true;
                    aiFsmAttackUnit_OnExitCleanUp();
                    end_cycle = true;
                    return;
                }
                else if ( UnitBlackBoard::unitOccupiesLoc( aiFsmAttackUnit_next_loc ) == true )
                {
                    if ( aiFsmAttackUnit_wait_timer.count() )
                    {
                        if ( aiFsmAttackUnit_next_loc == aiFsmAttackUnit_target_goal_loc )
                        {
                            UnitInterface::unit_placement_matrix.reset( aiFsmAttackUnit_target_goal_loc );
                            UnitInterface::unit_placement_matrix.getNextEmptyLoc( &aiFsmAttackUnit_target_goal_loc );

                            PathRequest path_request;
                            path_request.set( id, aiFsmAttackUnit_prev_loc, aiFsmAttackUnit_target_goal_loc, 0, &path, _path_request_full );
                            PathScheduler::requestPath( path_request );

                        }
                        else
                        {
                            PathRequest path_request;
                            path_request.set( id, aiFsmAttackUnit_prev_loc, aiFsmAttackUnit_target_goal_loc, 0, &path, _path_request_update );
                            PathScheduler::requestPath( path_request );
                        }

                        aiFsmAttackUnit_state = _aiFsmAttackUnit_path_generate;
                    }

                    end_cycle = true;
                    return;
                }
                else
                {
                    UnitBlackBoard::markUnitLoc( aiFsmAttackUnit_next_loc );
                    setFsmMoveMapSquare( aiFsmAttackUnit_next_square );
                    aiFsmAttackUnit_state = _aiFsmAttackUnit_move_wait;
                }
            }
            break;

            // *************************************************************

            case _aiFsmAttackUnit_move_wait :
            {
                if ( fsm_active_list[ _control_move_map_square ] == false )
                {
                    UnitBlackBoard::unmarkUnitLoc( aiFsmAttackUnit_prev_loc );
                    aiFsmAttackUnit_state = _aiFsmAttackUnit_check_fsm_transition;
                }
                end_cycle = true;
                return;
            }
            break;

            // *************************************************************

            case _aiFsmAttackUnit_check_fsm_transition :
            {
                if ( external_ai_event == _external_event_pending_unit_destruct )
                {
                    // External Event: This unit is about to be deleted
                    // Action : Exit fsm gracefully
                    aiFsmAttackUnit_OnExitCleanUp();
                    UnitBlackBoard::unmarkUnitLoc( aiFsmAttackUnit_next_loc );
                    external_ai_event = _external_event_null;
                    ai_command_state = _ai_command_idle;
                    end_cycle = true;
                    return;
                }
                else if ( pending_AI_comm == true )
                {
                    ai_fsm_transition_complete = true;
                    aiFsmAttackUnit_OnExitCleanUp();
                    end_cycle = true;
                    return;
                }
                else if (  aiFsmAttackUnit_target_destroyed == true )
                {
                    setAiFsmDefendHold();
                    aiFsmAttackUnit_OnExitCleanUp();
                    end_cycle = true;
                    return;
                }
                else
                {
                    aiFsmAttackUnit_state = _aiFsmAttackUnit_check_path_deviation;
                }
            }
            break;
            // *************************************************************

            case _aiFsmAttackUnit_check_path_deviation :
            {
                iXY deviation_vector;
                iXY goal_point_loc;

                MapInterface::mapXYtoPointXY( aiFsmAttackUnit_target_goal_loc, goal_point_loc );
                deviation_vector = target_unit_state->location - goal_point_loc;

                if ( deviation_vector.mag2() > unit_state.weapon_range )
                {
                    MapInterface::pointXYtoMapXY( target_unit_state->location, aiFsmAttackUnit_target_goal_loc );

                    PathRequest path_request;
                    path_request.set( id, aiFsmAttackUnit_next_loc, aiFsmAttackUnit_target_goal_loc, 0, &path, _path_request_full );
                    PathScheduler::requestPath( path_request );

                    aiFsmAttackUnit_state = _aiFsmAttackUnit_path_generate;
                    end_cycle = true;
                    return;
                }
                else
                {
                    aiFsmAttackUnit_state = _aiFsmAttackUnit_range_check;
                }

            }
            break;

        } // ** switch

    } while ( end_cycle == false );

}

void Vehicle::aiFsmAttackUnit_OnExitCleanUp()
{
    PathScheduler::killRequest( id );
    clearFsmGunneryTarget();
}

void Vehicle::setAiFsmDefendHold()
{
    ai_command_state = _ai_command_defend_hold;
    aiFsmDefendHold_state = _aiFsmDefendHold_search_for_enemy;
    aiFsmDefendHold_search_timer.changePeriod( .5 );
}

void Vehicle::aiFsmDefendHold()
{
    bool end_cycle = false;

    UnitBase  *target_unit_ptr;
    UnitState *target_unit_state;

    iXY range_vector;

    do
    {
        switch ( aiFsmDefendHold_state )
        {
            case _aiFsmDefendHold_search_for_enemy :
            {
                if ( external_ai_event == _external_event_pending_unit_destruct )
                {
                    // External Event: This unit is about to be deleted
                    // Action : Exit fsm gracefully
                    external_ai_event = _external_event_null;
                    ai_command_state = _ai_command_idle;
                    end_cycle = true;
                    return;
                }
                else if ( pending_AI_comm == true )
                {
                    // External Event: A new AI command is pending
                    // Action: Allow command transition to occur
                    ai_fsm_transition_complete = true;
                    end_cycle = true;
                    return;
                }
                else if ( aiFsmDefendHold_search_timer.count() )
                {
                    if (UnitInterface::queryClosestEnemyUnit(
                                &target_unit_ptr, unit_state.location,
                                player->getID() ) )
                    {
                        target_unit_state = &(target_unit_ptr->unit_state);
                        range_vector = target_unit_state->location - unit_state.location;
                        if ( range_vector.mag2() <= unit_state.defend_range )
                        {
                            aiFsmDefendHold_target_ID = target_unit_ptr->id;
                            setFsmGunneryTarget( aiFsmDefendHold_target_ID );
                            aiFsmDefendHold_state = _aiFsmDefendHold_attack_enemy;
                            end_cycle = true;
                            return;
                        }
                        else
                        {
                            end_cycle = true;
                            return;
                        }
                    } // **  quearyClosestEnemyUnit
                    else
                    {
                        end_cycle = true;
                        return;
                    }

                } // ** aiFsmDefendHold_search_timer.count()
                else
                {
                    end_cycle = true;
                    return;
                }
            }
            break;

            case _aiFsmDefendHold_attack_enemy :
            {
                if ( external_ai_event == _external_event_pending_unit_destruct )
                {
                    // External Event: This unit is about to be deleted
                    // Action : Exit fsm gracefully
                    clearFsmGunneryTarget();
                    external_ai_event = _external_event_null;
                    ai_command_state = _ai_command_idle;
                    end_cycle = true;
                    return;
                }
                else if ( pending_AI_comm == true )
                {
                    // External Event: A new AI command is pending
                    // Action: Allow command transition to occur
                    clearFsmGunneryTarget();
                    ai_fsm_transition_complete = true;
                    end_cycle = true;
                    return;
                }
                else
                {
                    target_unit_ptr = UnitInterface::getUnit( aiFsmDefendHold_target_ID );
                    if ( target_unit_ptr == 0 )
                    {
                        clearFsmGunneryTarget();
                        aiFsmDefendHold_state = _aiFsmDefendHold_search_for_enemy;
                        end_cycle = true;
                        return;
                    }
                    else
                    {
                        target_unit_state = &(target_unit_ptr->unit_state);
                        if ( target_unit_state->lifecycle_state == _UNIT_LIFECYCLE_DESTROYED )
                        {
                            clearFsmGunneryTarget();
                            aiFsmDefendHold_state = _aiFsmDefendHold_search_for_enemy;
                            end_cycle = true;
                            return;
                        } // ** if
                        else
                        {
                            range_vector = target_unit_state->location - unit_state.location;
                            if ( range_vector.mag2() > unit_state.defend_range )
                            {
                                clearFsmGunneryTarget();
                                aiFsmDefendHold_state = _aiFsmDefendHold_search_for_enemy;
                                end_cycle = true;
                                return;
                            }
                        }
                    }
                }
                end_cycle = true;
                return;
            }
            break;
        } // ** switch

    } while ( end_cycle == false );

}





void Vehicle::aiFsmManualMove()
{
    bool end_cycle = false;
    signed char offset_x = 0, offset_y = 0;
    size_t next_square;

    do
    {
        switch( aiFsmManualMove_state )
        {
            case _aiFsmManualMove_next_move :
            {
                aiFsmManualMove_prev_loc = aiFsmManualMove_next_loc;
                orientationToOffset( aiFsmManualMove_move_orientation, &offset_x, &offset_y );
                aiFsmManualMove_next_loc.x += offset_x;
                aiFsmManualMove_next_loc.y += offset_y;
                next_square = MapInterface::mapXYtoOffset(aiFsmManualMove_next_loc);

                if ( MapInterface::getMovementValue( aiFsmManualMove_next_loc ) == 0xFF )
                {
                    setAiFsmDefendHold();
                    end_cycle = true;
                    return;
                }
                else if( UnitBlackBoard::unitOccupiesLoc( aiFsmManualMove_next_loc ) == true )
                {
                    setAiFsmDefendHold();
                    end_cycle = true;
                    return;
                }
                else
                {
                    UnitBlackBoard::markUnitLoc( aiFsmManualMove_next_loc );
                    setFsmMoveMapSquare( next_square );
                    aiFsmManualMove_state = _aiFsmManualMove_move_wait;
                }
            }
            break;

            // *************************************************************

            case _aiFsmManualMove_move_wait :
            {
                if ( fsm_active_list[ _control_move_map_square ] == false )
                {
                    UnitBlackBoard::unmarkUnitLoc( aiFsmManualMove_prev_loc );
                    aiFsmManualMove_state = _aiFsmManualMove_check_fsm_transition;
                    end_cycle = true;
                    return;
                }
                else
                {
                    end_cycle = true;
                    return;
                } // ** else
            }
            break;

            // *************************************************************

            case _aiFsmManualMove_check_fsm_transition :
            {
                if ( external_ai_event == _external_event_pending_unit_destruct )
                {
                    // External Event: This unit is about to be deleted
                    // Action : Exit fsm gracefully
                    UnitBlackBoard::unmarkUnitLoc( aiFsmManualMove_next_loc );
                    external_ai_event = _external_event_null;
                    ai_command_state = _ai_command_idle;
                    end_cycle = true;
                    return;
                }
                else if ( pending_AI_comm == true )
                {
                    ai_fsm_transition_complete = true;
                    end_cycle = true;
                    return;
                }
                else
                {
                    aiFsmManualMove_state = _aiFsmManualMove_next_move;
                }
            }
            break;

        } // ** switch
    } while ( end_cycle == false );

}

void Vehicle::fireWeapon( iXY &target_loc )
{
    reload_counter = 0;

    unsigned short projectile_type = launchProjectile();
    ProjectileInterface::newProjectile(projectile_type, unit_state.unit_type, id,
                                       unit_state.damage_factor,
                                       unit_state.location,
                                       target_loc
                                      );

    if ( NetworkState::status == _network_state_server ) {
        FireWeaponOpcode fire_opcode;
        fire_opcode.opcode = _UNIT_OPCODE_FIRE_WEAPON;
        fire_opcode.setUnitID(id);
        fire_opcode.setTarget(target_loc);
        UnitInterface::sendOpcode(&fire_opcode);
    }
}

//-----------------------------------------------------------------
void Vehicle::soundSelected()
{
        sound->playSound( soundSelect.size()?soundSelect.c_str():"yessir");
}

void Vehicle::accessThreatLevels()
{
    if ( unit_state.threat_level == _threat_level_under_attack )
    {
        if( threat_level_under_attack_timer.count() )
        {
            unit_state.threat_level = _threat_level_all_clear;
        }
    }

}

//-----------------------------------------------------------------
unsigned short Vehicle::launchProjectile()
{
    long distance = WorldViewInterface::getCameraDistance(unit_state.location);
    sound->playAmbientSound(fireSound.c_str(), distance );
    sound->playBattle();

    return weaponType;
}

//-----------------------------------------------------------------
void Vehicle::updateFsmState()
{
    if ( fsm_timer.count() ) {
        if ( fsm_active_list[ _control_move_map_square ] == true )
            fsmMoveMapSquare();

        if ( fsm_active_list[ _control_turret_track_point ] == true )
            fsmTurretTrackPoint();

        if ( fsm_active_list[ _control_turret_track_target ] == true )
            fsmTurretTrackTarget();

        if ( fsm_active_list[ _control_gunnery_location ] == true )
            fsmGunneryLocation();

        if ( fsm_active_list[ _control_gunnery_target ] == true )
            fsmGunneryTarget();
    }
}


void Vehicle::updateAIState()
{
    switch(ai_command_state)
    {
        case _ai_command_idle:
            aiFsmIdle();
            break;

        case _ai_command_defend_hold:
            aiFsmDefendHold();
            break;

        case _ai_command_move_to_loc:
            aiFsmMoveToLoc();
            break;

        case _ai_command_attack_unit:
            aiFsmAttackUnit();
            break;

        case _ai_command_manual_move:
            aiFsmManualMove();
            break;
    }
}


void Vehicle::checkPendingAICommStatus()
{
    if ( (pending_AI_comm == true) &&
            (ai_fsm_transition_complete == true) )
    {
        pending_AI_comm = false;
        ai_fsm_transition_complete = false;

        if ( unit_state.lifecycle_state == _UNIT_LIFECYCLE_ACTIVE )
        {
            switch( pending_AI_comm_mesg.command )
            {
                case _command_move_to_loc :
                    setCommandMoveToLoc( &pending_AI_comm_mesg );
                    break;

                case _command_attack_unit :
                    setCommandAttackUnit( &pending_AI_comm_mesg );
                    break;

                case _command_start_manual_move :
                    setCommandManualMove( &pending_AI_comm_mesg  );
                    break;

                case _command_stop_manual_move :
                    setCommandManualMove( &pending_AI_comm_mesg  );
                    break;
            } // ** switch

        } // ** unit_state.lifecycle_state == _UNIT_LIFECYCLE_ACTIVE

    } // ** if

}

void Vehicle::setCommandMoveToLoc(const UMesgAICommand* message)
{
    iXY start;

    if ( fsm_active_list[ _control_gunnery_location ] == true )
    {
        clearFsmGunneryLocation();
    }

    aiFsmMoveToLoc_goal = message->getGoalLoc();
    ai_command_state = _ai_command_move_to_loc;
    aiFsmMoveToLoc_state = _aiFsmMoveToLoc_path_generate;
    aiFsmMoveToLoc_path_not_finished = true;
    aiFsmMoveToLoc_wait_timer.changePeriod( MOVEWAIT_TIME );

    opcode_move_timer.changePeriod( 0.10f );
    move_opcode_sent = false;
    //move_opcode_sent = true;

    MapInterface::pointXYtoMapXY( unit_state.location, start );

    //LOG( ("UnitID %d, %d : Start %d, %d : Goal %d, %d", id.getPlayer(), id.getIndex(),
    //                                                    start.x, start.y,
    //                                                    aiFsmMoveToLoc_goal.x, aiFsmMoveToLoc_goal.y ) );
    //LOGGER.info("Unit speed rate=%d | speed_factor=%d", (unsigned short)unit_state.speed_rate, (unsigned short)unit_state.speed_factor);


    PathRequest path_request;
    path_request.set( id, start, aiFsmMoveToLoc_goal, 0, &path, _path_request_full );
    PathScheduler::requestPath( path_request );

    iXY target;
    MapInterface::mapXYtoPointXY( aiFsmMoveToLoc_goal, target);
    setFsmTurretTrackPoint( target );
}

void Vehicle::setCommandAttackUnit(const UMesgAICommand* message)
{
    iXY start;
    UnitBase *target_unit_ptr;
    UnitState *target_unit_state;

    aiFsmAttackUnit_target_ID = message->getTargetUnitID();

    target_unit_ptr = UnitInterface::getUnit( aiFsmAttackUnit_target_ID );
    if ( target_unit_ptr == 0 )
        return;

    target_unit_state = &(target_unit_ptr->unit_state);

    ai_command_state = _ai_command_attack_unit;

    aiFsmAttackUnit_state = _aiFsmAttackUnit_path_generate;
    aiFsmAttackUnit_path_not_finished = true;
    aiFsmAttackUnit_target_destroyed = false;

    MapInterface::pointXYtoMapXY( unit_state.location, start );
    MapInterface::pointXYtoMapXY( target_unit_state->location, aiFsmAttackUnit_target_goal_loc );

    PathRequest path_request;
    path_request.set( id, start, aiFsmAttackUnit_target_goal_loc, 0, &path, _path_request_full );
    PathScheduler::requestPath( path_request );

    opcode_move_timer.changePeriod( 0.10f );
    move_opcode_sent = false;
    //move_opcode_sent = true;

    setFsmGunneryTarget( aiFsmAttackUnit_target_ID );
}

void Vehicle::setCommandManualMove(const UMesgAICommand* message)
{
    if ( message->command == _command_start_manual_move )
    {
        aiFsmManualMove_move_orientation = message->manual_move_orientation;
        MapInterface::pointXYtoMapXY( unit_state.location, aiFsmManualMove_next_loc );
        aiFsmManualMove_state = _aiFsmManualMove_next_move;
        ai_command_state = _ai_command_manual_move;
    }
    else if ( message->command == _command_stop_manual_move )
    {
        ai_command_state = _ai_command_idle;
    }
}

void Vehicle::setCommandManualFire(const UMesgAICommand* message)
{
    setFsmGunneryLocation(message->getTargetLoc());
}

void Vehicle::messageAICommand(const UnitMessage* message)
{
    const UMesgAICommand *command_mesg = (const UMesgAICommand *) message;

    if ( unit_state.lifecycle_state == _UNIT_LIFECYCLE_ACTIVE )
    {
        if (command_mesg->command == _command_manual_fire)
        {
            setCommandManualFire( command_mesg );
        }
        else
        {
            memcpy(&pending_AI_comm_mesg, command_mesg, sizeof(UMesgAICommand));
            pending_AI_comm = true;
        }
    }
}


void Vehicle::messageWeaponHit(const UnitMessage *message)
{
    const UMesgWeaponHit *weapon_hit = (const UMesgWeaponHit *) message;

    if (!unit_state.bounds(weapon_hit->getHitLocation()))
        return;

    unit_state.hit_points -= weapon_hit->getDamageFactor();
    unit_state.threat_level = _threat_level_under_attack;
    threat_level_under_attack_timer.changePeriod(
            GameConfig::interface_attacknotificationtime);

    UpdateStateUnitOpcode update_state_opcode;

    update_state_opcode.setUnitID(id);
    update_state_opcode.setHitPoints(unit_state.hit_points);
    UnitInterface::sendOpcode( &update_state_opcode );

    if ( unit_state.hit_points <= 0 )
    {
        unit_state.lifecycle_state = _UNIT_LIFECYCLE_PENDING_DESTRUCT;
        external_ai_event = _external_event_pending_unit_destruct;

        UMesgEndLifeCycleUpdate lifecycle_update;
        lifecycle_update.set(id, weapon_hit->getOwnerUnitID(),
                unit_state.unit_type);
        UnitInterface::sendMessage(&lifecycle_update);

        // ** Note: Temp
        iXY current_map_loc;
        MapInterface::pointXYtoMapXY(unit_state.location, current_map_loc);
        UnitBlackBoard::unmarkUnitLoc(current_map_loc);
    }
}

void Vehicle::messageSelectBoxUpdate(const UnitMessage* message)
{
    const UMesgUpdateSelectBoxInfo *select_box_update
        = (const UMesgUpdateSelectBoxInfo *) message;

    switch (select_box_update->request_type)
    {
        case _select_box_allie_visibility:
        {
            select_info_box.setAllieIcon(select_box_update->allie_flag_visiblity );
            break;
        }
        break;

        case _select_box_flag_visiblity:
        {
            select_info_box.setFlagIcon( select_box_update->flag_visiblity );
            break;
        }

        case _select_box_is_allied:
        {
            select_info_box.setAllieState( select_box_update->allied_state );
            break;
        }
    }
}

void Vehicle::messageSelfDestruct(const UnitMessage* )
{
    unit_state.lifecycle_state = _UNIT_LIFECYCLE_PENDING_DESTRUCT;
    external_ai_event = _external_event_pending_unit_destruct;

    // ** Note: Temp
    iXY current_map_loc;
    MapInterface::pointXYtoMapXY( unit_state.location, current_map_loc );
    UnitBlackBoard::unmarkUnitLoc( current_map_loc );
}


void Vehicle::processMessage(const UnitMessage* message)
{
    if (unit_state.lifecycle_state != _UNIT_LIFECYCLE_ACTIVE)
        return;

    switch(message->message_id)
    {
        case _umesg_ai_command:
            messageAICommand(message);
            break;

        case _umesg_weapon_hit:
            messageWeaponHit(message);
            break;

        case _umesg_update_select_box_info:
            messageSelectBoxUpdate(message);
            break;

        case _umesg_self_destruct:
            messageSelfDestruct(message);
            break;

        default:
            LOGGER.warning("Unknown unit message (id %d)",
                    message->message_id);
            break;
    }
}

void Vehicle::unitOpcodeMove(const UnitOpcode* opcode)
{
    if ( ( (in_sync_flag == false) && !(opcode->flags & _unit_opcode_flag_sync) ) ||
            ( (in_sync_flag == true) && (opcode->flags & _unit_opcode_flag_sync) )
       )
        return;

    const MoveOpcode* move_opcode = (const MoveOpcode *) opcode;

    iXY sync_loc;
    iXY current_loc;

    MapInterface::offsetToMapXY(move_opcode->getSquare(), sync_loc );
    sync_loc.x = sync_loc.x + move_opcode->loc_x_offset;
    sync_loc.y = sync_loc.y + move_opcode->loc_y_offset;

    MapInterface::pointXYtoMapXY( unit_state.location, current_loc );

    if ( current_loc != sync_loc )
    {
        MapInterface::mapXYtoPointXY( sync_loc, unit_state.location );
    }

    setFsmMoveMapSquare(move_opcode->getSquare());
}

void Vehicle::unitOpcodeTrackPoint(const UnitOpcode* opcode )
{
    if ( ( (in_sync_flag == false) && !(opcode->flags & _unit_opcode_flag_sync) ) ||
            ( (in_sync_flag == true) && (opcode->flags & _unit_opcode_flag_sync) )
       )
        return;

    const TurretTrackPointOpcode* track_point_opcode
        = (const TurretTrackPointOpcode *) opcode;

    if ( track_point_opcode->activate == true )
    {
        iXY target = track_point_opcode->getTarget();
        setFsmTurretTrackPoint( target );
    }
    else
    {
        fsm_active_list[ _control_turret_track_point ] = false;
    }

}

void Vehicle::unitOpcodeTrackTarget(const UnitOpcode* opcode )
{
    if ( ( (in_sync_flag == false) && !(opcode->flags & _unit_opcode_flag_sync) ) ||
            ( (in_sync_flag == true) && (opcode->flags & _unit_opcode_flag_sync) )
       )
        return;

    const TurretTrackTargetOpcode* track_target_opcode
        = (const TurretTrackTargetOpcode *) opcode;

    if ( track_target_opcode->activate == true )
    {
        setFsmTurretTrackTarget( track_target_opcode->getTargetUnitID() );
    }
    else
    {
        fsm_active_list[ _control_turret_track_target ] = false;
    }

}

void Vehicle::unitOpcodeFireWeapon(const UnitOpcode* opcode )
{
    iXY target_loc;

    if ( ( (in_sync_flag == false) && !(opcode->flags & _unit_opcode_flag_sync) ) ||
            ( (in_sync_flag == true) && (opcode->flags & _unit_opcode_flag_sync) )
       )
        return;

    const FireWeaponOpcode *fire_weapon = (const FireWeaponOpcode *) opcode;
    target_loc = fire_weapon->getTarget();
    fireWeapon( target_loc );
}

void Vehicle::unitOpcodeSync(const UnitOpcode* )
{
    in_sync_flag = true;
}

void Vehicle::unitOpcodeUpdateState(const UnitOpcode* opcode )
{
    const UpdateStateUnitOpcode *update_state_opcode
        = (const UpdateStateUnitOpcode *) opcode;

    unit_state.hit_points = update_state_opcode->getHitPoints();

    unit_state.threat_level = _threat_level_under_attack;
    threat_level_under_attack_timer.changePeriod( 30 );
}

void Vehicle::unitOpcodeDestruct(const UnitOpcode* )
{
    unit_state.lifecycle_state = _UNIT_LIFECYCLE_INACTIVE;
}

void Vehicle::syncUnit()
{
    syncFsmTurretTrackPoint();
    syncFsmTurretTrackTarget();

    SyncUnitOpcode sync_opcode;

    sync_opcode.opcode = _UNIT_OPCODE_SYNC_UNIT;
    sync_opcode.setUnitID(id);
    UnitInterface::sendOpcode( &sync_opcode );
}


void Vehicle::processMoveOpcodeQueue()
{
    if (!move_opcode_queue.empty())
    {
        if( fsm_active_list[ _control_move_map_square ] == false )
        {
            if ( move_opcode_queue.size() >= 3 )
            {
                for( int i = 0; i < 2; i++ )
                {
                    move_opcode_queue.pop();
                }
                //ConsoleInterface::postMessage( "Move Opcode Queue Adjusted" );
            }
            UnitOpcodeStruct opcodedata = move_opcode_queue.front();
            move_opcode_queue.pop();
            const UnitOpcode* opcode = (UnitOpcode*) &opcodedata;
            unitOpcodeMove(opcode);
        }
    }

}

void Vehicle::processOpcodeQueue()
{
    processMoveOpcodeQueue();

    if (!opcode_queue.empty())
    {
        UnitOpcodeStruct opcodedata = opcode_queue.front();
        const UnitOpcode* opcode = (UnitOpcode*) &opcodedata;

        switch(opcode->opcode )
        {
            case _UNIT_OPCODE_TURRET_TRACK_POINT:
                unitOpcodeTrackPoint(opcode);
                break;

            case _UNIT_OPCODE_TURRET_TRACK_TARGET:
                unitOpcodeTrackTarget(opcode);
                break;

            case _UNIT_OPCODE_FIRE_WEAPON:
                unitOpcodeFireWeapon(opcode);
                break;

            case _UNIT_OPCODE_SYNC_UNIT:
                unitOpcodeSync(opcode);
                break;

            case _UNIT_OPCODE_UPDATE_STATE:
                unitOpcodeUpdateState(opcode);
                break;

            case _UNIT_OPCODE_DESTRUCT:
                unitOpcodeDestruct(opcode);
                break;

            default:
                LOGGER.warning("Unknown Opcode: %d.\n", opcode->opcode);
                assert(false);
                break;
        }
        opcode_queue.pop();
    }
}

void Vehicle::evalCommandOpcode(const UnitOpcode* opcode)
{
    if (opcode->opcode == _UNIT_OPCODE_MOVE)
    {
        move_opcode_queue.push(*((const UnitOpcodeStruct*) opcode));
    }
    else
    {
        opcode_queue.push(*((const UnitOpcodeStruct*) opcode));
    }
}

void Vehicle::updateState()
{
    updateFsmState();

    if ( NetworkState::status == _network_state_server )
    {
        if( unit_state_timer.count() )
            updateUnitStateProperties();

        updateAIState();
        checkPendingAICommStatus();
    }
    else
    {
        processOpcodeQueue();
    }

    accessThreatLevels();

    if( unit_state.hit_points < unit_state.max_hit_points )
    {
        smolderWait += TimerInterface::getTimeSlice();

        int intPercent = unit_state.percentDamageInt();

        if (intPercent > 50 && (rand() % 100) < intPercent)
        {
            if (smolderWait > smolderWaitMin)
            {
                float percent = unit_state.percentDamageFloat();

                smolderWaitMin  = (float(100 - percent) / 100.0) + (float(100 - percent) / 100.0) * 0.3;

                ParticleInterface::addUnitDamagePuffParticle(unit_state);

                smolderWait = 0.0f;
            }
        }
    }
}


void Vehicle::offloadGraphics( SpriteSorter &sorter )
{
    body_anim_shadow.setWorldPos( unit_state.location);

    if ( sorter.cullSprite( body_anim_shadow ) == false )
    {
        // Body
        body_anim.setWorldPos( unit_state.location );
        body_anim.setFrame( unit_state.body_angle.angle_int );

        // Turret
        turret_anim.setWorldPos( unit_state.location );
        turret_anim.setFrame( unit_state.turret_angle.angle_int );

        // Body Shadow
        body_anim_shadow.setFrame( unit_state.body_angle.angle_int );

        // Turret Shadow
        turret_anim_shadow.setWorldPos( unit_state.location );
        turret_anim_shadow.setFrame( unit_state.turret_angle.angle_int );

        select_info_box.setBoxState( unit_state.select );

        //Added layer selection to the selection box info.
        select_info_box.setAttrib( unit_state.location,
                GameConfig::interface_unitinfodrawlayer );
        select_info_box.setHitPoints( unit_state.hit_points );

        sorter.forceAddSprite( &body_anim_shadow );
    }

    //sorter.addSprite( &body_anim_shadow );
}
