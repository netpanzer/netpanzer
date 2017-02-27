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
#ifndef _UNIT_POWERUP_HPP
#define _UNIT_POWERUP_HPP

#include "PowerUps/PowerUp.hpp"
#include "Units/UnitState.hpp"

class UnitPowerUp : public PowerUp
{
protected:
    int unit_powerup_type;

    virtual void onHit( UnitID unit_id );

    void powerUpHitPoints( UnitState *unit_state, UnitID unit_id );
    void powerUpFirePower( UnitState *unit_state, UnitID unit_id );
    void powerUpSpeed( UnitState *unit_state, UnitID unit_id );
    void powerUpRepair( UnitState *unit_state, UnitID unit_id );
    void powerUpReload( UnitState *unit_state, UnitID unit_id );
    void powerUpWRange( UnitState *unit_state, UnitID unit_id );
    void powerUpDestruct( UnitID unit_id );
    void powerUpGRepair( UnitState *unit_state, UnitID unit_id );
    void powerUpSuperunit( UnitState *unit_state, UnitID unit_id );

public:

    UnitPowerUp(iXY map_loc, int type);
    virtual ~UnitPowerUp()
    { }

    virtual void onHitMessage( PowerUpHitMesg *message  );

};

#endif // ** _UNIT_POWERUP_HPP
