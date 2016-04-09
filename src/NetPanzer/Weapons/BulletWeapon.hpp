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
#ifndef __BulletWeapon_hpp__
#define __BulletWeapon_hpp__

#include "Weapons/Weapon.hpp"

class BulletWeapon : public Weapon
{
private:
    static float velocity;

    virtual void fsmFlight();
    virtual void setSurface();

public:
    BulletWeapon(UnitID owner, unsigned short owner_type_id, unsigned short damage, iXY &start, iXY &end);
    virtual void updateStatus();
    virtual void offloadGraphics( SpriteSorter &sorter );

};

#endif // __BulletWeapon_hpp__
