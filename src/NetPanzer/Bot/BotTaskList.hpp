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
#ifndef BOTTASKSLIST_H
#define BOTTASKSLIST_H

class BotTask;

#include "Units/UnitBase.hpp"

#include <map>
typedef std::map<UnitID, BotTask*> bottasks_t;

class BotTaskList {
    public:
        enum {
            TASK_IDLE,
            TASK_MOVE,
            TASK_ATTACK
        };
    private:
        bottasks_t m_tasks;

        void adjustLastUnitLoc(UnitBase *unit);
    public:
        ~BotTaskList();

        void setUnitTask(UnitBase *unit, int task);
        int queryUnitTask(UnitBase *unit);
};

#endif
