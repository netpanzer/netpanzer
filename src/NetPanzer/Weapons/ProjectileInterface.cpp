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

#include "ProjectileInterface.hpp"

#include "Particles/ParticleInterface.hpp"
#include "Weapons/MissleWeapon.hpp"
#include "Weapons/BulletWeapon.hpp"
#include "Weapons/ShellWeapon.hpp"
#include "Interfaces/WorldViewInterface.hpp"
#include "Util/Math.hpp"

std::list<Projectile*> ProjectileInterface::projectiles;

void ProjectileInterface::removeAll()
{
    std::list<Projectile*>::iterator i;
    for(i = projectiles.begin(); i != projectiles.end(); ++i)
        delete *i;

    projectiles.clear();
}

void ProjectileInterface::resetLogic()
{
    removeAll();
}

void ProjectileInterface::newProjectile( unsigned short projectile_type,
        unsigned short owner_type_id,
        UnitID &owner, unsigned short damage,
        iXY &start, iXY &end )
{
    Projectile *temp;

    assert(projectile_type != Weapon::_none);
    
    if (projectile_type == Weapon::_quad_missile) {
        // The following code launches 4 missles spaced out evenly like they would be in a
        // missle bay.
        iXY startPos;
        iXY endPos;

        const float startRadius = 5.0f;
        const int endRadius     = 50;

        fXY direction = Math::unitDirection(start, end);
        fXY offsetPos;

        //// West outer
        endPos    = iXY(end.x + (rand() % endRadius << 1) - endRadius, end.y + (rand() % endRadius << 1) - endRadius);
        offsetPos = Math::unitDirectionWest(direction) * startRadius * 2;
        startPos = start + iXY(int(offsetPos.x), int(offsetPos.y));
        temp = new MissleWeapon( owner, owner_type_id, 0, damage, startPos, endPos);
        projectiles.push_back(temp);

        ParticleInterface::addMissleLaunchPuff(startPos, direction, owner_type_id);

        // West inner
        endPos    = iXY(end.x + (rand() % endRadius << 1) - endRadius, end.y + (rand() % endRadius << 1) - endRadius);
        offsetPos = Math::unitDirectionWest(direction) * startRadius;
        startPos = start + iXY(int(offsetPos.x), int(offsetPos.y));
        temp = new MissleWeapon( owner, owner_type_id, 0, damage, startPos, endPos);
        projectiles.push_back(temp);
        ParticleInterface::addMissleLaunchPuff(startPos, direction, owner_type_id);

        // East inner
        endPos    = iXY(end.x + (rand() % endRadius << 1) - endRadius, end.y + (rand() % endRadius << 1) - endRadius);
        offsetPos = Math::unitDirectionEast(direction) * startRadius;
        startPos = start + iXY(int(offsetPos.x), int(offsetPos.y));
        temp = new MissleWeapon( owner, owner_type_id, 0, damage, startPos, endPos);
        projectiles.push_back(temp);
        ParticleInterface::addMissleLaunchPuff(startPos, direction, owner_type_id);

        //// East outer
        endPos    = iXY(end.x + (rand() % endRadius << 1) - endRadius, end.y + (rand() % endRadius << 1) - endRadius);
        offsetPos = Math::unitDirectionEast(direction) * startRadius * 2;
        startPos = start + iXY(int(offsetPos.x), int(offsetPos.y));
        temp = new MissleWeapon( owner, owner_type_id, 0, damage, startPos, endPos);
        projectiles.push_back(temp);
        ParticleInterface::addMissleLaunchPuff(startPos, direction, owner_type_id);
    } else if (projectile_type == Weapon::_bullet) {
        temp = new BulletWeapon( owner, owner_type_id, damage, start, end );
        projectiles.push_back(temp);
    } else if (projectile_type == Weapon::_shell) {
        temp = new ShellWeapon( owner, owner_type_id, damage, start, end );
        projectiles.push_back(temp);
    } else if (projectile_type == Weapon::_double_missile) {
        // The following code launches 2 missles spaced out evenly like they would be in a
        // missle bay.
        iXY startPos;
        iXY endPos;

        const float startRadius = 5.0f;
        const int endRadius     = 30;

        fXY direction = Math::unitDirection(start, end);
        fXY offsetPos;

        // West inner
        endPos    = iXY(end.x + (rand() % endRadius << 1) - endRadius, end.y + (rand() % endRadius << 1) - endRadius);
        offsetPos = Math::unitDirectionWest(direction) * startRadius;
        startPos = start + iXY(int(offsetPos.x), int(offsetPos.y));
        temp = new MissleWeapon( owner, owner_type_id, 1, damage, startPos, endPos);
        projectiles.push_back(temp);
        ParticleInterface::addMissleLaunchPuff(startPos, direction, owner_type_id);

        // East inner
        endPos    = iXY(end.x + (rand() % endRadius << 1) - endRadius, end.y + (rand() % endRadius << 1) - endRadius);
        offsetPos = Math::unitDirectionEast(direction) * startRadius;
        startPos = start + iXY(int(offsetPos.x), int(offsetPos.y));
        temp = new MissleWeapon( owner, owner_type_id, 1, damage, startPos, endPos);
        projectiles.push_back(temp);
        ParticleInterface::addMissleLaunchPuff(startPos, direction, owner_type_id);
    }
}

void ProjectileInterface::updateStatus()
{
    std::list<Projectile*>::iterator i;
    for(i = projectiles.begin(); i != projectiles.end(); ) {
        Projectile* projectile = *i;

        if (projectile->lifecycle_status == _lifecycle_weapon_in_active) {
            i = projectiles.erase(i);
            delete projectile;
            continue;
        }
        
        projectile->updateStatus();
        ++i;
    }
}

void ProjectileInterface::offloadGraphics( SpriteSorter &sorter )
{
    std::list<Projectile*>::iterator i;

    for(i = projectiles.begin(); i != projectiles.end(); ++i) {
        Projectile* projectile = *i;
        projectile->offloadGraphics(sorter);
    }
}

