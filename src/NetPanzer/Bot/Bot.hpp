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
#ifndef BOT_H
#define BOT_H

class UnitBase;
class iXY;

#include "BotTaskList.hpp"
#include "Classes/SelectionList.hpp"

class Bot {
    static class Bot *s_bot;
    public:
        static void initialize(Bot *bot);
        static void shutdown();
        static Bot *bot() { return s_bot; }

        virtual ~Bot() {}
        virtual void processEvents() = 0;
        virtual void processEvents2() = 0;
        virtual void processEvents3() = 0;
        virtual void processEvents4() = 0;
        virtual void processEvents5() = 0;
        virtual void processEvents6() = 0;
        virtual void processEvents7() = 0;
        virtual void processEvents8() = 0;
        virtual void processEvents9() = 0;

        void sendMoveCommand(const iXY& world_pos);

        void moveUnit(UnitBase *unit, iXY map_pos);
        void attackUnit(UnitBase *unit, UnitBase *enemyUnit);
        void manualFire(UnitBase *unit, iXY world_pos);
        void produceUnit(ObjectiveID outpostID, int selectedProduce);
    protected:
        BotTaskList m_tasks;
};

#endif
