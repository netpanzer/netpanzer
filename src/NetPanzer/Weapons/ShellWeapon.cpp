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


#include "ShellWeapon.hpp"
#include "Classes/UnitMessageTypes.hpp"
#include "Units/UnitInterface.hpp"
#include "Classes/Network/NetworkState.hpp"
#include "Interfaces/WorldViewInterface.hpp"
#include "System/Sound.hpp"
#include "Particles/Particle2D.hpp"
#include "WeaponGlobals.hpp"
#include "Particles/ParticleInterface.hpp"
#include "Util/Math.hpp"
#include "Interfaces/GameConfig.hpp"
#include "Particles/FlashParticle2D.hpp"


ShellWeapon::ShellWeapon(UnitID owner, unsigned short owner_type_id, unsigned short damage, iXY &start, iXY &end)
        : Weapon(owner, owner_type_id, damage, start, end)
{
    velocity = gShellVelocity;

    int frame360 = getGoalAngle(start, end);
    int frame36  = frame360 / 10;

    // I have to change this from my angle to Vlad's unit angle to get the frame number in Vlad's format.
    frame36 = (35 - frame36);
    frame36 += 9;
    frame36 %= 36;

    // Get the correct tip position of the barrel.
    ParticleInterface::addMuzzlePuff(fXYZ(start.x, 0, start.y), fXYZ(direction.x, 0, direction.y), frame36, Weapon::owner_type_id);

    shell.setData(gShellPackedSurface);
    shell.setFrame(frame360);
    shell.setSpriteHeight(weaponLayer);

    shell.setDrawModeSolid();

    shellShadow.setDrawModeBlend(&Palette::colorTableDarkenALittle);
    shellShadow.setData(gShellPackedSurface);
    shellShadow.setFrame(getGoalAngle(start, end));
    shellShadow.setSpriteHeight(weaponShadowLayer);
}

void ShellWeapon::fsmFlight()
{
    bool end_cycle = false;

    do {
        switch( fsmFlight_state ) {
        case _fsmFlight_idle : {
                end_cycle = true;
            }
            break;

        case _fsmFlight_in_flight : {
                if (path.increment(&location, (short) velocity) == true ) {
                    fsmFlight_state = _fsmFlight_on_target;
                    end_cycle = true;
                } else {
                    end_cycle = true;
                }

            }
            break;

        case _fsmFlight_on_target : {
                UMesgWeaponHit weapon_hit;

                if ( NetworkState::status == _network_state_server ) {
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
                                        WorldViewInterface::getCameraDistance( location ) );

                // **  Particle Shit
                iXY loc = iXY( location.x, location.y );
                ParticleInterface::addMiss(loc, Weapon::owner_type_id);
                end_cycle = true;
            }
            break;

        }

    } while ( end_cycle == false );
}

void ShellWeapon::updateStatus( void )
{
    if ( fsm_timer.count() ) {
        fsmFlight();
    }

    shell.setWorldPos(location);

    if (GameConfig::video_shadows) {
        shellShadow.setWorldPos(location);
    }
}

void ShellWeapon::offloadGraphics(SpriteSorter &sorter)
{
    shell.setWorldPos(location);

    if (GameConfig::video_shadows) {
        shellShadow.setWorldPos(location.x - 10, location.y);
    }

    sorter.addSprite(&shell);

    if (GameConfig::video_shadows) {
        sorter.addSprite(&shellShadow);
    }
}
