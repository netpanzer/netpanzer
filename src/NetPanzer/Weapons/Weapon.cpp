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


#include <math.h>

#include "Weapons/Weapon.hpp"

#include "Classes/UnitMessageTypes.hpp"
#include "Units/UnitInterface.hpp"

#include "Classes/Network/NetworkState.hpp"

// NOTE: Temp until new sprites put in
#include "Interfaces/WorldViewInterface.hpp"
#include "Classes/ScreenSurface.hpp"

#include "Util/Exception.hpp"
#include "System/Sound.hpp"
#include "Particles/ParticleInterface.hpp"
#include "WeaponGlobals.hpp"
#include "Util/Math.hpp"

void Weapon::init()
{
    //packFiles();
    gMissleMediumPackedSurface.load("pics/particles/missles/pak/misslesMedium.pak");
    gMissleSmallPackedSurface.load("pics/particles/missles/pak/misslesSmall.pak");
    gShellPackedSurface.load("pics/particles/shells/pak/shells.pak");

    Surface       temp;
    PackedSurface pack;

    gMissleThrustPackedSurface.load("pics/particles/lights/pak/missleThrust.pak");
    gMissleGroundLightPackedSurface.load("pics/particles/lights/pak/missleGroundLight.pak");
}

void Weapon::uninit()
{
    //packFiles();
    gMissleMediumPackedSurface.unload("pics/particles/missles/pak/misslesMedium.pak");
    gMissleSmallPackedSurface.unload("pics/particles/missles/pak/misslesSmall.pak");
    gShellPackedSurface.unload("pics/particles/shells/pak/shells.pak");
    gMissleThrustPackedSurface.unload("pics/particles/lights/pak/missleThrust.pak");
    gMissleGroundLightPackedSurface.unload("pics/particles/lights/pak/missleGroundLight.pak");
}

Weapon::Weapon(UnitID owner, unsigned short owner_type_id, unsigned short damage, iXY &start, iXY &end)
{
    // I use this to line things up, so leave this in here when this
    // get redone.  thank you.
    direction = Math::unitDirection(start, end);

    owner_id               = owner;
    Weapon::owner_type_id  = owner_type_id;
    damage_factor          = damage;
    fsmFlight_state        = _fsmFlight_in_flight;

    path.set_path( start, end );
    path.increment( &location, 15 );
    fsm_timer.changeRate( 20 );

    lifecycle_status = _lifecycle_weapon_in_fight;

    // This is getting called as well in all derived class constructors.
    shell.setAttrib( location, 0 );
}

void Weapon::fsmFlight( void )
{
    bool end_cycle = false;

    do {
        switch( fsmFlight_state ) {
            case _fsmFlight_idle:
                end_cycle = true;
                break;

            case _fsmFlight_in_flight:
                if ( path.increment( &location, 4 ) == true ) {
                    fsmFlight_state = _fsmFlight_on_target;
                }
                end_cycle = true;
                break;

            case _fsmFlight_on_target: {
                UMesgWeaponHit weapon_hit;

                if (NetworkState::status == _network_state_server) {
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
                sound->playSound("hit_target");

                // **  Particle Shit
                iXY loc = iXY( location.x, location.y );
                ParticleInterface::addMiss(loc, Weapon::owner_type_id);
                end_cycle = true;
                break;
            }
        }
    } while ( end_cycle == false );
}

void Weapon::updateStatus( void )
{
    if ( fsm_timer.count() )
        fsmFlight();

    // ** NOTE: Temp unit new sprites put in
    iRect world_win;

    shell.setWorldPos( location );

    WorldViewInterface::getViewWindow(&world_win );

    if ( shell.isVisible( world_win ) == true ) {
        shell.blit(screen, world_win );
    }
}

int Weapon::getGoalAngle(const iXY &start, const iXY &end)
{
    // Get the slope of the line from the object to the mouse
    float num = start.y - end.y; // invert Y since screen is inverted
    float den = end.x - start.x;

    // Figure out what angle we want to be pointing, in radians.  This
    // value is from -PI, which points left, moving counterclockwise to 0,
    // which points right, continuing clockwise to +PI, which points
    // back left.
    float goalAngleRad = atan2(num, den);

    // Fix the goal angle around to match up with the way our angle
    // measurements work.  We measure with 0 pointing north, moving around
    // clockwise.  We also want to measure the angle from 0.0 to 1.0.
    float ourGoalAngle = ((M_PI / 2) - goalAngleRad) / (M_PI * 2);
    int   angle        = (int) (ourGoalAngle * 360);

    while (angle < 0) {
        angle += 360;
    }

    while (angle >= 360) {
        angle -= 360;
    }

    return angle;
}
