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


bool InfoBar::bar_on = false;


static const PIX titles_color = 206; // #6d78c1 more or less "slate blue"
static const char *titles =
        "game              units           frags             objs.           time               FPS";

static const PIX format_color = 249; // #d9d9ff
static const char *format =
        "     %10s         %3d/%-3d         %4d/%-4d         %3d/%-3d        %02d:%02d/%02d:%02d       %.2f";

static const PIX bars_color = 185; // #6f906d
static const char *bars =
        "                |               |                 |               |                  |";


static const PIX titles_color2 = 206; // #6d78c1 more or less "slate blue"
static const char *titles2 =
        "client ping             server ping             avg c. ping             avg s. ping               ";

static const PIX bars_color2 = 185; // #6f906d
static const char *bars2 =
        "                      |                       |                       |                           ";

static const PIX format_color2 = 217; // gray
static const char *format2 =
        "            %.1f                     %.1f                    %.1f                    %.1f         ";

static const char *format2alt =
        "            %s                     %s                     %s                     %s               ";


void
InfoBar::draw(Surface &dest) {
    iRect r(position.x, position.y, dest.getWidth(), position.y + Surface::getFontHeight() + 2);
    dest.bltLookup(r, Palette::darkGray256.getColorArray());

    char buf[512];
    char buf2[512];
    char buf2alt[512];


    snprintf(buf, sizeof(buf),
             format,
//             "game %s | units %3d/%-3d | frags %4d/%-4d | objs. %3d/%-3d | time %02d:%02d/%02d:%02d | FPS %.2f",
             gameconfig->getGameTypeString(),

             int(UnitInterface::getUnitCount(PlayerInterface::getLocalPlayerIndex())),
             GameConfig::game_maxunits / GameConfig::game_maxplayers,

             PlayerInterface::getLocalPlayer()->getKills(),
             (GameConfig::game_gametype == _gametype_fraglimit ||
              GameConfig::game_gametype == _gametype_objectiveANDfraglimit ||
              GameConfig::game_gametype == _gametype_fraglimitORtimelimit) ? (int) GameConfig::game_fraglimit : 0,


             PlayerInterface::getLocalPlayer()->getObjectivesHeld(),
             (GameConfig::game_gametype == _gametype_objective ||
              GameConfig::game_gametype == _gametype_objectiveANDfraglimit)
             ? (int) ObjectiveInterface::getObjectiveLimit() : 0,


             (int) GameManager::getGameTime() / 3600,
             (int) (GameManager::getGameTime() / 60) % 60,
             (GameConfig::game_gametype == _gametype_timelimit ||
              GameConfig::game_gametype == _gametype_fraglimitORtimelimit) ? GameConfig::game_timelimit / 60 : 0,
             (GameConfig::game_gametype == _gametype_timelimit ||
              GameConfig::game_gametype == _gametype_fraglimitORtimelimit) ? GameConfig::game_timelimit % 60 : 0,

             TimerInterface::getFPSAvg()
    );


    int posx = position.x + 2 + 4;
    int posy = position.y + 2;
    dest.bltStringShadowed(posx, posy, titles, titles_color, Color::black);
    dest.bltStringShadowed(posx, posy, bars, bars_color, Color::black);
    dest.bltStringShadowed(posx, posy, buf, format_color, Color::black);


    if (bar_on) {
        float getUpLastPing = PlayerInterface::getLocalPlayer()->getUpLastPing();

        float getDownLastPing = PlayerInterface::getLocalPlayer()->getDownLastPing();

        float getUpAvgPing = PlayerInterface::getLocalPlayer()->getUpAvgPing();

        float getDownAvgPing = PlayerInterface::getLocalPlayer()->getDownAvgPing();

        snprintf(buf2, sizeof(buf2),

                 format2,
                 float(getUpLastPing),
                 float(getDownLastPing),
                 float(getUpAvgPing),
                 float(getDownAvgPing)

        );

        static const char *na = "n/a";
        snprintf(buf2alt, sizeof(buf2alt),

                 format2alt,
                 na,
                 na,
                 na,
                 na

        );

        int pos2y = posy + Surface::getFontHeight();
        int pos2x = 0;

        iRect r2(0, pos2y, dest.getWidth(), pos2y + Surface::getFontHeight() + 2);
        dest.bltLookup(r2, Palette::darkGray256.getColorArray());

        dest.bltStringShadowed(pos2x + 2 + 4, pos2y + 1, titles2, titles_color2, Color::black);
        dest.bltStringShadowed(pos2x + 2 + 4, pos2y + 1, bars2, bars_color2, Color::black);
        if (getUpLastPing == 0 && getDownLastPing == 0) {
            // do nothing
            dest.bltStringShadowed(pos2x + 2 + 4, pos2y + 1, buf2alt, format_color2, Color::black);
        } else {
            dest.bltStringShadowed(pos2x + 2 + 4, pos2y + 1, buf2, format_color2, Color::black);
        }
    }
}
