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

#include "PowerUps/PowerUp.hpp"
#include "Util/Log.hpp"
#include "Units/UnitBlackBoard.hpp"
#include "Units/UnitInterface.hpp"
#include "Interfaces/MapInterface.hpp"
#include "Classes/Network/NetworkState.hpp"

SpritePacked PowerUp::POWERUP_ANIM;
SpritePacked PowerUp::POWERUP_ANIM_SHADOW;
SpritePacked PowerUp::POWERUP_ANIM_R;
SpritePacked PowerUp::POWERUP_ANIM_SHADOW_R;

PowerUp::PowerUp()
{
    life_cycle_state = _power_up_lifecycle_state_active;
}

PowerUp::PowerUp(iXY map_loc, PowerUpID ID, int type)
{
    this->map_loc = map_loc;
    this->ID = ID;
    this->type = type;

    MapInterface::mapXYtoPointXY( map_loc, this->world_loc );

    this->life_cycle_state = _power_up_lifecycle_state_active;

    if (type != 2) {
    sprite.setData( POWERUP_ANIM );
    } else {
    sprite.setData( POWERUP_ANIM_R );
    }

    sprite.setAttrib( world_loc, iXY(0,0), 5 );

    if (type != 2) {
    sprite_shadow.setData( POWERUP_ANIM_SHADOW );
    } else {
    sprite_shadow.setData( POWERUP_ANIM_SHADOW_R );
    }

    sprite_shadow.setAttrib( world_loc, iXY(0,0), 4 );
    sprite_shadow.setDrawModeBlend(&Palette::colorTableDarkenALot);
}

PowerUp::PowerUp(iXY map_loc, int type)
{
    this->map_loc = map_loc;
    this->ID = -1;
    this->type = type;

    MapInterface::mapXYtoPointXY( map_loc, this->world_loc );

    this->life_cycle_state = _power_up_lifecycle_state_active;

    if (type != 2) {
    sprite.setData( POWERUP_ANIM );
    } else {
    sprite.setData( POWERUP_ANIM_R );
    }

    sprite.setAttrib( world_loc, iXY(0,0), 5 );

    if (type != 2) {
    sprite_shadow.setData( POWERUP_ANIM_SHADOW );
    } else {
    sprite_shadow.setData( POWERUP_ANIM_SHADOW_R );
    }

    sprite_shadow.setAttrib( world_loc, iXY(0,0), 4 );
    sprite_shadow.setDrawModeBlend(&Palette::colorTableDarkenALot);
}

bool PowerUp::isPowerUpHit(UnitID *unit_id)
{
    if( UnitBlackBoard::unitOccupiesLoc(map_loc)
        && UnitInterface::queryUnitAtMapLoc(map_loc, unit_id) )
    {
        return true;
    }

    return false;
}

void PowerUp::offloadGraphics( SpriteSorter &sorter )
{
    sprite.nextFrame();
    sprite_shadow.setFrame( sprite.getCurFrame() );

    sorter.addSprite( &sprite );
    sorter.addSprite( &sprite_shadow );
}

void
PowerUp::updateState( void )
{
    if ( NetworkState::status == _network_state_server
         && life_cycle_state == _power_up_lifecycle_state_active )
    {
        UnitID unit_id;
        if( isPowerUpHit( &unit_id ) == true )
        {
            onHit( unit_id );
        }
    }
}

