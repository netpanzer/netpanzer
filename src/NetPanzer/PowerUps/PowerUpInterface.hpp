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
#ifndef _POWERUP_INTERFACE_HPP
#define _POWERUP_INTERFACE_HPP

#include <vector>

#include "PowerUps/PowerUp.hpp"
#include "Util/Timer.hpp"
#include "Classes/Network/NetPacket.hpp"
#include "Network/ClientSocket.hpp"

class PowerUpInterface
{
protected:
    typedef std::vector<PowerUp *> PowerUpList;
    static PowerUpList powerup_list;

    static PowerUpID getNextPowerUpID();

    static int power_up_limit;
    static int power_up_regen_time_upper_bound;
    static int power_up_regen_time_lower_bound;

    static Timer regen_timer;

    static void setPowerUpLimits( unsigned long map_size_x, unsigned long map_size_y );

    static void generatePowerUp();

    static void netMessagePowerUpCreate(const NetMessage* message);
    static void netMessagePowerUpHit(const NetMessage* message );

public:

    static void initialize();
    static void uninitialize();
    static void resetLogic();

    static void updateState();
    static void offloadGraphics( SpriteSorter &sorter );

    static void processNetMessages(const NetMessage* message );

    static void syncPowerUps( ClientSocket * client );
    static void syncPowerUpsBC();
};

#endif // ** _POWERUP_INTERFACE_HPP
