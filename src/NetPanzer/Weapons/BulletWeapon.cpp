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


#include "BulletWeapon.hpp"
#include "Classes/UnitMessageTypes.hpp"
#include "Classes/Network/NetworkState.hpp"
#include "Units/UnitInterface.hpp"
#include "Interfaces/WorldViewInterface.hpp"
#include "System/Sound.hpp"
#include "Particles/ParticleInterface.hpp"
#include "WeaponGlobals.hpp"

float BulletWeapon::velocity = gBulletVelocity;

BulletWeapon::	BulletWeapon(UnitID owner, unsigned short owner_type_id, unsigned short damage, iXY &start, iXY &end)
        : Weapon(owner, owner_type_id, damage, start, end)
{
    setSurface();
}

void BulletWeapon::setSurface()
{}

void BulletWeapon::fsmFlight()
{
    bool end_cycle = false;

    do {
        switch(fsmFlight_state) {
            case _fsmFlight_idle:
                end_cycle = true;
                break;

            case _fsmFlight_in_flight:
                if (path.increment(&location, (short) velocity) == true ) {
                    fsmFlight_state = _fsmFlight_on_target;
                    end_cycle = true;
                } else {
                    end_cycle = true;
                }
                break;

            case _fsmFlight_on_target:
                if (NetworkState::status == _network_state_server) {
                    UMesgWeaponHit weapon_hit;
                    weapon_hit.setHeader(0, _umesg_flag_broadcast);
                    weapon_hit.message_id = _umesg_weapon_hit;
                    weapon_hit.setOwnerUnitID(owner_id);
                    weapon_hit.setHitLocation(location);
                    weapon_hit.setDamageFactor(damage_factor);
                    UnitInterface::sendMessage( &weapon_hit );
                }

                fsmFlight_state = _fsmFlight_idle;
                lifecycle_status = _lifecycle_weapon_in_active;

                //SFX
                sound->playAmbientSound("hit",
                        WorldViewInterface::getCameraDistance(location) );

                // **  Particle Shit
                {
                    iXY loc = iXY( location.x, location.y );
                    ParticleInterface::addDirtKick(loc);
                }
                end_cycle = true;
                break;
            default:
                assert(false);
        }
    } while(end_cycle == false);
}

void BulletWeapon::updateStatus()
{
    if (fsm_timer.count())
        fsmFlight();
}

void BulletWeapon::offloadGraphics(SpriteSorter& sorter)
{
    (void) sorter;
    //shell.setWorldPos( location );

    //sorter.addSprite( &shell );
}

