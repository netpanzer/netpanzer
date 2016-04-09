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


#include "BotTaskList.hpp"
#include "BotTask.hpp"
#include "Util/Log.hpp"

//-----------------------------------------------------------------
BotTaskList::~BotTaskList()
{
    for (bottasks_t::iterator i = m_tasks.begin(); i != m_tasks.end(); i++) {
        delete i->second;
    }
}
//-----------------------------------------------------------------
/**
 * If unit didn't move then task=IDLE
 */
void BotTaskList::adjustLastUnitLoc(UnitBase *unit)
{
    iXY curLoc = unit->unit_state.location;
    if (curLoc == m_tasks[unit->id]->getLastLoc()) {
        m_tasks[unit->id]->setTask(TASK_IDLE);
    }
    m_tasks[unit->id]->setLastLoc(curLoc);
}
//-----------------------------------------------------------------
void BotTaskList::setUnitTask(UnitBase *unit, int task)
{
    bottasks_t::iterator it = m_tasks.find(unit->id);
    if (it == m_tasks.end()) {
        iXY curLoc = unit->unit_state.location;
        m_tasks[unit->id] = new BotTask(curLoc, task);
    }
    else {
        it->second->setTask(task);
    }
}
//-----------------------------------------------------------------
/**
 * Query for unit task and adjust unit last location.
 */
int BotTaskList::queryUnitTask(UnitBase *unit)
{
    bottasks_t::iterator it = m_tasks.find(unit->id);
    if (it == m_tasks.end()) {
        return TASK_IDLE;
    }
    else {
        adjustLastUnitLoc(unit);
        return m_tasks[unit->id]->getTask();
    }
}

