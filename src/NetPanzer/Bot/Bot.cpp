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
#ifdef WIN32
#include <windows.h>
#endif

#include <sstream>


#include "Interfaces/WorldViewInterface.hpp"
#include "Interfaces/MapInterface.hpp"
#include "Interfaces/PlayerInterface.hpp"
#include "Interfaces/ChatInterface.hpp"

#include "Units/UnitInterface.hpp"

#include "Classes/UnitMessageTypes.hpp"
#include "Classes/Network/TerminalNetMesg.hpp"
#include "Classes/Network/PlayerNetMessage.hpp"

#include "Classes/Network/NetMessageEncoder.hpp"
#include "Classes/PlacementMatrix.hpp"
#include "Classes/ScreenSurface.hpp"

#include "Util/NTimer.hpp"

#include "Interfaces/GameConfig.hpp"

#include "Units/Vehicle.hpp"

#include "Scripts/ScriptManager.hpp"


//-------------------

#include <iostream>

#include "Bot.hpp"

#include "Classes/PlacementMatrix.hpp"
#include "Units/UnitBase.hpp"
#include "Objectives/Objective.hpp"
#include "Classes/Network/TerminalNetMesg.hpp"
#include "Classes/Network/NetworkClient.hpp"
#include "Classes/Network/NetworkState.hpp"
#include "Objectives/ObjectiveInterface.hpp"
#include "Util/Log.hpp"

Bot *Bot::s_bot = 0;
//-----------------------------------------------------------------
void
Bot::initialize(Bot *bot)
{
    s_bot = bot;
}
//-----------------------------------------------------------------
void
Bot::shutdown()
{
    if (s_bot) {
        delete s_bot;
        s_bot = 0;
    }
}
//-----------------------------------------------------------------
void
Bot::moveUnit(UnitBase *unit, iXY map_pos)
{
    assert(unit != 0);

    PlacementMatrix matrix;
    matrix.reset(map_pos);
    matrix.getNextEmptyLoc(&map_pos);

    TerminalUnitCmdRequest comm_mesg;
    comm_mesg.comm_request.setHeader(unit->id, _umesg_flag_unique);
    comm_mesg.comm_request.setMoveToLoc(map_pos);

    CLIENT->sendMessage(&comm_mesg, sizeof(TerminalUnitCmdRequest));
    m_tasks.setUnitTask(unit, BotTaskList::TASK_MOVE);

    //LOGGER.debug("bot: moveUnit %d to %dx%d", unit->id, map_pos.x, map_pos.y);
}
//-----------------------------------------------------------------
void
Bot::attackUnit(UnitBase *unit, UnitBase *enemyUnit)
{
    assert(unit != 0);
    assert(enemyUnit != 0);

    TerminalUnitCmdRequest comm_mesg;
    comm_mesg.comm_request.setHeader(unit->id, _umesg_flag_unique);
    comm_mesg.comm_request.setTargetUnit(enemyUnit->id);

    CLIENT->sendMessage(&comm_mesg, sizeof(TerminalUnitCmdRequest));
    m_tasks.setUnitTask(unit, BotTaskList::TASK_ATTACK);

    //LOGGER.debug("bot: attackUnit %d to %d", unit->id, enemyUnit->id);
}
//-----------------------------------------------------------------
void
Bot::manualFire(UnitBase *unit, iXY world_pos)
{
    assert(unit != 0);

    TerminalUnitCmdRequest comm_mesg;
    comm_mesg.comm_request.setHeader(unit->id, _umesg_flag_unique);
    comm_mesg.comm_request.setManualFire(world_pos);

    CLIENT->sendMessage(&comm_mesg, sizeof(TerminalUnitCmdRequest));
    //NOTE: manual fire is not special unit task,
    // unit can move and fire simultanous
}
//-----------------------------------------------------------------
void
Bot::produceUnit(ObjectiveID outpostID, int selectedProduce)
{
    LOGGER.debug("bot: produceUnit outpost=%d selectedProduce=%d",
                 outpostID, selectedProduce);

    ObjectiveInterface::sendChangeGeneratingUnit(outpostID, selectedProduce, true);
 }



 void
Bot::sendMoveCommand(const iXY& world_pos)
{

    iXY map_pos;
    PlacementMatrix matrix;


    UnitBase *unit_ptr;

    TerminalUnitCmdRequest msg;

    MapInterface::pointXYtoMapXY( world_pos, map_pos );
    matrix.reset( map_pos );

    NetMessageEncoder encoder;



        const std::vector<UnitBase*>& uc = UnitInterface::getPlayerUnits(PlayerInterface::getLocalPlayerIndex());

        for ( Uint8 id_list_index = 0; id_list_index < uc.size(); ++id_list_index)
        {


        unit_ptr = uc[ id_list_index ];

        if ( unit_ptr != 0 )
        {

                matrix.getNextEmptyLoc( &map_pos );
                msg.comm_request.setHeader( unit_ptr->id,
                                            _umesg_flag_unique );

                msg.comm_request.setMoveToLoc( map_pos );

                if ( !encoder.encodeMessage(&msg, sizeof(msg)) )
                {
                    CLIENT->sendMessage(encoder.getEncodedMessage(),
                                        encoder.getEncodedLen());
                    encoder.resetEncoder();
                    encoder.encodeMessage(&msg, sizeof(msg));
                }

        }
    }

    if ( ! encoder.isEmpty() )
    {
        CLIENT->sendMessage(encoder.getEncodedMessage(),
                            encoder.getEncodedLen());
    }


}



