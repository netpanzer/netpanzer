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
#ifndef _WEAPON_HPP
#define _WEAPON_HPP

#include "Core/CoreTypes.hpp"
#include "Weapons/Projectile.hpp"
#include "Util/BresenhamLine.hpp"
#include "Util/Timer.hpp"
#include "Classes/Sprite.hpp"
#include "Types/fXY.hpp"
#include "Particles/Particle2D.hpp"

enum
{
    _lifecycle_weapon_in_fight,
    _lifecycle_weapon_in_active
};

enum
{
    _fsmFlight_idle,
    _fsmFlight_in_flight,
    _fsmFlight_on_target
};

class Weapon : public Projectile
{
public:
    /**
     * Projectile types.
     */
    enum {
        _none,
        _quad_missile,
        _bullet,
        _shell,
        _double_missile
    };

protected:
    SpritePacked shell;
    SpritePacked shellShadow;

protected:
    UnitID         owner_id;
    unsigned short owner_type_id;

    unsigned short damage_factor;
    iXY       location;
    BresenhamLine  path;
    Timer          fsm_timer;
    unsigned char  fsmFlight_state;

    fXY            direction;

    virtual void fsmFlight();

public:

    Weapon(UnitID owner, unsigned short owner_type_id, unsigned short damage, iXY &start, iXY &end);

    virtual ~Weapon()
    { }

    virtual void updateStatus();

    virtual void offloadGraphics(SpriteSorter& )
    { }

    static void init();
    static void uninit();
    static int  getGoalAngle(const iXY &start, const iXY &end);
};

#endif // ** _WEAPON_HPP
