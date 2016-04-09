/*
Copyright (C) 2008 by Aaron Perez <aaronps@gmail.com>

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

 * Created on September 17, 2008, 8:50 PM
 */

#include "Views/Components/InfoBar.hpp"
#include "2D/Palette.hpp"
#include "Interfaces/GameConfig.hpp"
#include "Units/UnitInterface.hpp"
#include "Interfaces/PlayerInterface.hpp"
#include "Objectives/ObjectiveInterface.hpp"
#include "Interfaces/GameManager.hpp"


static const PIX titles_color = 206; // #6d78c1 more or less "slate blue"
static const char* titles =
        "game              units           frags             objs.           time               FPS";

static const PIX format_color = 249; // #d9d9ff
static const char* format =
        "     %10s         %3d/%-3d         %4d/%-4d         %3d/%-3d        %02d:%02d/%02d:%02d       %.2f";

static const PIX bars_color = 185; // #6f906d
static const char* bars =
        "                |               |                 |               |                  |";



void
InfoBar::draw(Surface &dest)
{
    iRect r(position.x, position.y, dest.getWidth(), position.y+12);
    dest.bltLookup(r, Palette::darkGray256.getColorArray());

    char buf[512];
    
    snprintf(buf, sizeof(buf),
             format,
//             "game %s | units %3d/%-3d | frags %4d/%-4d | objs. %3d/%-3d | time %02d:%02d/%02d:%02d | FPS %.2f",
             gameconfig->getGameTypeString(),
             
             int(UnitInterface::getUnitCount(PlayerInterface::getLocalPlayerIndex())),
             GameConfig::game_maxunits / GameConfig::game_maxplayers,
             
             PlayerInterface::getLocalPlayer()->getKills(),
             (GameConfig::game_gametype == _gametype_fraglimit)?(int)GameConfig::game_fraglimit:0,
             
             PlayerInterface::getLocalPlayer()->getObjectivesHeld(),
             (GameConfig::game_gametype == _gametype_objective)?ObjectiveInterface::getObjectiveLimit():0,
             
             (int)GameManager::getGameTime() / 3600,
             (int)(GameManager::getGameTime() / 60) % 60,
             (GameConfig::game_gametype == _gametype_timelimit)?GameConfig::game_timelimit / 60:0,
             (GameConfig::game_gametype == _gametype_timelimit)?GameConfig::game_timelimit % 60:0,
             
             TimerInterface::getFPSAvg()
             );
    int posx = position.x + 2;
    int posy = position.y + 2;
    dest.bltStringShadowed(posx, posy, titles, titles_color, Color::black);
    dest.bltStringShadowed(posx, posy, bars, bars_color, Color::black);
    dest.bltStringShadowed(posx, posy, buf, format_color, Color::black);
    
//    dest.bltStringShadowed(position.x+2,position.y+2,buf,Color::white, Color::black);

}
