/*
Copyright (C) 2004 by Matthias Braun <matze@braunis.de>

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


#include <string>
#include <sstream>
#include <stdio.h>
#include <unistd.h>

#include "Classes/OpcodeDebugger.hpp"
#include "Units/UnitOpcodes.hpp"

void OpcodeDebugger::logOpcode(std::ostream& log, UnitOpcode* opcode)
{
    log << " UNIT:" << opcode->getUnitID()
        << " FL:" << (int) opcode->flags << " ";
    switch(opcode->opcode) {
        case _UNIT_OPCODE_MOVE:
        {
            MoveOpcode* movecode = (MoveOpcode*) opcode;
            log << "move: S:" << movecode->getSquare()
                << " dX:" << (int) movecode->loc_x_offset
                << " dY:" << (int) movecode->loc_y_offset;
            break;
        }
        case _UNIT_OPCODE_TURRET_TRACK_POINT:
        {
            TurretTrackPointOpcode* trackcode =(TurretTrackPointOpcode*) opcode;
            log << "ttrackp: x:" << trackcode->getTarget().x
                << " y:" << trackcode->getTarget().y
                << " A:" << (int) trackcode->activate;
            break;
        }         
        case _UNIT_OPCODE_TURRET_TRACK_TARGET:
        {
            TurretTrackTargetOpcode* tracktcode
                = (TurretTrackTargetOpcode*) opcode;
            log << "ttrack: U:" << tracktcode->getTargetUnitID()
                << " A:" << (int) tracktcode->activate;
            break;
        }
        case _UNIT_OPCODE_FIRE_WEAPON:
        {
            FireWeaponOpcode* weaponcode = (FireWeaponOpcode*) opcode;
            log << "fire: X:" << weaponcode->getTarget().x
                << " Y:" << weaponcode->getTarget().y;
            break;
        }
        case _UNIT_OPCODE_SYNC_UNIT:
        {
            //SyncUnitOpcode* synccode = (SyncUnitOpcode*) opcode;
            log << "sync";
            break;
        }
        case _UNIT_OPCODE_UPDATE_STATE:
        {
            UpdateStateUnitOpcode* updatecode = 
                (UpdateStateUnitOpcode*) opcode;
            log << "upd: HP:" << (int) updatecode->getHitPoints();
            break;
        }
        case _UNIT_OPCODE_DESTRUCT:
        {
            /*DestructUnitOpcode* destructcode =
                (DestructUnitOpcode*) opcode;*/
            log << "destruct";
            break;
        }
        default:
        {
            log << "unknown";
            break;
        }
    }
}

