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
#ifndef _UNIT_GLOBAL_POWERUP_HPP
#define _UNIT_GLOBAL_POWERUP_HPP

#include "PowerUps/PowerUp.hpp"
#include "Units/UnitState.hpp"

class UnitGlobalPowerUp : public PowerUp
{
protected:
    int unit_powerup_type;

    virtual void onHit( UnitID unit_id );


    void powerUpGlobalFirePower( UnitState *unit_state, UnitID unit_id );
    void powerUpGlobalSpeed( UnitState *unit_state, UnitID unit_id );
    void powerUpGlobalRepair( UnitState *unit_state, UnitID unit_id );
    void powerUpGlobalReload( UnitState *unit_state, UnitID unit_id );
    void powerUpGlobalDestruct( UnitID unit_id );


public:

    UnitGlobalPowerUp(iXY map_loc, int type);
    virtual ~UnitGlobalPowerUp()
    { }

    virtual void onHitMessage( PowerUpHitMesg *message  );

};

#endif // ** _UNIT_GLOBAL_POWERUP_HPP
