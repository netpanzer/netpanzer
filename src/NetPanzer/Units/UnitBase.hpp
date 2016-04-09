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
#ifndef _UNITBASE_HPP
#define _UNITBASE_HPP

#include "Core/CoreTypes.hpp"
#include "Units/UnitState.hpp"

class UnitMessage;
class UnitOpcode;
class SpriteSorter;
class PlayerState;

class UnitBase
{
public:
    PlayerState* player;
    UnitID       id;
    UnitState    unit_state;
    bool         in_sync_flag;

    UnitBase(PlayerState* newPlayer, UnitID newId)
        : player(newPlayer), id(newId)
    { }
    virtual ~UnitBase()
    { }

    virtual void processMessage(const UnitMessage* ) = 0;
    virtual void evalCommandOpcode(const UnitOpcode* ) = 0;
    virtual void updateState() = 0;
    virtual void syncUnit() = 0;
    virtual void offloadGraphics(SpriteSorter& ) = 0;
    virtual void soundSelected() = 0;

    bool isWeaponInRange(const iXY& loc) const
    {
        int x = loc.x - unit_state.location.x;
        int y = loc.y - unit_state.location.y;
        return (unsigned)(x*x + y*y) < unit_state.weapon_range;
    }

private:
    friend class UnitInterface;
    void setID(UnitID id)
    {
        this->id = id;
    }

    UnitID getID() const
    {
        return id;
    }

protected:
    UnitBase *groupLinkNext;
};

#endif // ** _UNITBASE_HPP
