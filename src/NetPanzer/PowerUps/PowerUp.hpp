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
#ifndef _POWERUP_HPP
#define _POWERUP_HPP

#include "Core/CoreTypes.hpp"
#include "Classes/SpriteSorter.hpp"
#include "Classes/Sprite.hpp"
#include "Classes/Network/PowerUpNetMessage.hpp"

enum { _power_up_lifecycle_state_active,
       _power_up_lifecycle_state_inactive
     };

class PowerUp
{
protected:
    bool isPowerUpHit( UnitID *unit_id );
    virtual void onHit( UnitID unit_id ) = 0;

    SpritePacked sprite;
    SpritePacked sprite_shadow;

public:
    static SpritePacked POWERUP_ANIM;
    static SpritePacked POWERUP_ANIM_SHADOW;
    static SpritePacked POWERUP_ANIM_R;
    static SpritePacked POWERUP_ANIM_SHADOW_R;

    iXY       map_loc;
    iXY       world_loc;
    PowerUpID ID;
    int       type;
    int       life_cycle_state;

public:
    PowerUp();
    PowerUp(iXY map_loc, PowerUpID ID, int type);
    PowerUp(iXY map_loc, int type);
    virtual ~PowerUp()
    { }

    void updateState();

    void offloadGraphics(SpriteSorter& );

    virtual void onHitMessage(PowerUpHitMesg* )
    { }
    PowerUp *next;
};
#endif // ** _POWERUP_HPP
