/*
Copyright (C) 2003 Ivo Danihelka <ivo@danihelka.net>

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

#ifndef BOTPLAYER_H
#define BOTPLAYER_H

#include "Core/CoreTypes.hpp"
#include "Bot.hpp"
#include "Util/Timer.hpp"
#include "Util/NTimer.hpp"

class Objective;

#include <vector>
typedef std::vector<PlayerID> playerList_t;
typedef std::vector<ObjectiveID> outpostList_t;


class BotPlayer : public Bot {
    private:
        Timer m_timer;
    public:
        BotPlayer();
        virtual void processEvents();
        virtual void processEvents2();
        virtual void processEvents3();
        virtual void processEvents4();
        virtual void processEvents5();
        virtual void processEvents6();
        virtual void processEvents7();
        virtual void processEvents8();
        virtual void processEvents9();

        PlayerID isReady();
        UnitBase *getRandomUnit(PlayerID playerIndex);
        UnitBase *getSoRandomUnit(PlayerID playerIndex);

        playerList_t getEnemyPlayers();
        PlayerID getRandomEnemyPlayer();
        UnitBase *getRandomEnemy();


        void unitOccupyOupost(UnitBase *unit);
        void unitOccupyOupostNA(UnitBase *unit);
        void unitOccupyEnemyOupostNA(UnitBase *unit);
        void outpostProduce();
        void outpostProduceZero();
        void goThere(UnitBase *unit, iXY dest_loc);
        void doGoThere(iXY dest_loc);
        void orientationToOffset( unsigned short orientation, signed char *offset_x, signed char *offset_y );
        void disownRandomOwned();

        NTimer actionTimer;
        //NTimer actionTimer2;
        NTimer inactivityTimer;
        //NTimer pathTimer;
        //NTimer leavingTime;
        //NTimer movingTimer;
        //NTimer shootingTimer;
        NTimer touchTimer;

        enum { _bot_idle,
               _bot_heading_outpost,
               _bot_capturing_outpost,
               _bot_outpost_occupied,
               _bot_defending_outpost
             };

        static bool attack_flag;
        static Objective *target_outpost;
        static unsigned short bot_objs;
        static unsigned char bot_status;
        static bool idle_units;
        static unsigned char state_change;
        static unsigned char threat_level;
        //static unsigned long increment;
        static bool inrange;
        static bool touch_flag;

};

#endif

