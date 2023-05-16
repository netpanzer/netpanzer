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
#ifndef __GAMECONFIG_HPP
#define __GAMECONFIG_HPP

#include <assert.h>
#include <string.h>
#include <vector>
#include <string>
#include <stdexcept>

#include "Core/CoreTypes.hpp"
#include "Util/Log.hpp"

#include "2D/Surface.hpp"
#include "2D/Color.hpp"
#include "Util/NoCopy.hpp"
#include "ConfigVariable.hpp"
#include "Classes/PlayerUnitConfig.hpp"

#include "Util/StringUtil.hpp"

#define MAX_STYLES_NUM 12
#define DEFAULT_UNIT_PROFILES "Manta, Panther1, Titan, Stinger, Bobcat, Bear, Archer, Wolf, Drake, Spanzer"
#define DEFAULT_UNITS_STYLES "original, danisch, desert, metro, night, nva, platane, surpat, tiger, woodland"

enum { _mini_map_objective_draw_mode_solid_rect,
       _mini_map_objective_draw_mode_outline_rect,
       _mini_map_objective_draw_mode_last
     };

enum { _mini_map_unit_size_small,
       _mini_map_unit_size_large,
       _mini_map_unit_size_last
     };

enum { _unit_selection_box_draw_mode_rect,
       _unit_selection_box_draw_mode_rect_edges,
       _unit_selection_box_draw_mode_last
     };

enum { _gamemode_skirmish,
       _gamemode_multiplayer
     };

enum { _game_session_host,
       _game_session_join,
       _game_session_last
     };

enum { _gametype_objective,
       _gametype_fraglimit,
       _gametype_timelimit,
       _gametype_objectiveANDfraglimit,
       _gametype_fraglimitORtimelimit,
       _gametype_last
     };

enum { _connection_tcpip,
       _connection_direct_connect,
       _connection_modem
     };

enum { _game_config_respawn_type_round_robin,
       _game_config_respawn_type_random,
       _game_config_respawn_type_random_alt,
       _game_config_respawn_type_last
     };

enum { _game_config_standard_res_640x480,
       _game_config_standard_res_800x600,
       _game_config_standard_res_1024x768,
       _game_config_standard_res_1280x1024,
       _game_config_standard_res_max
     };

enum { _color_aqua,
       _color_blue,
       _color_dark_blue,
       _color_blue_gray,
       _color_red,
       _color_dark_red,
       _color_light_green,
       _color_green,
       _color_dark_green,
       _color_yellow,
       _color_light_orange,
       _color_orange,
       _color_black,
       _color_white,
       _color_gray,
       _color_dark_gray,
       _color_last
     };

namespace INI {
    class Section;
}

class GameConfig : public NoCopy
{
public:
    GameConfig(const std::string& luaconfigfile, bool usePhysFS = true);
    ~GameConfig();

    void loadConfig();
    void saveConfig();

    static unsigned int video_width;
    static unsigned int video_height;
    static bool         video_fullscreen;
    static bool         video_hardwaresurface;
    static bool         video_doublebuffer;
    static bool         video_shadows;
    static bool         video_blendsmoke;
#ifdef _WIN32
    static bool         video_usedirectx;
#endif

    static bool      interface_show_health;
    static bool      interface_show_flags;
    static bool      interface_show_names;

    static bool      game_enable_bases;
    static int       game_base_capture_mode; // 0=no capture, 1=normal, 2=...
    static int       game_base_limit;  // 0=no limit, other number max bases per player
    static int       game_autokicktime; // in minutes
    static bool      game_allowmultiip; // allow multi connect from same ip
    static NPString* game_unit_profiles; // "," or space separated list of profiles to load
    static NPString* game_unit_spawnlist; // "," or space separated list with the numbers of each unit to spawn with
    static NPString* game_adminpass;     // the secret password for admins
    static NPString* game_gamepass;      // the secret password for entering game
    static int       game_changeflagtime; // in minutes
    static int       game_gametype;
    static int       game_maxplayers;
    static int       game_maxunits;
    static int       game_timelimit;
    static int       game_fraglimit;
    static bool      game_powerups;
    static int       game_occupationpercentage;
    static bool      game_allowallies;
    static int       game_cloudcoverage;
    static int       game_respawntype;
    static int       game_windspeed;
    static int       game_lowscorelimit;
    static int       game_anticheat;
    //static bool      game_authentication;
    //static bool      game_bots_allowed;
    //static bool      game_scrambler;
    //static int       game_maxchatlines;
    static NPString* game_map;
    static NPString* game_mapcycle;
    static NPString* game_mapstyle;
    static NPString* game_units_styles;

    static Uint8 player_flag_data[FLAG_WIDTH*FLAG_HEIGHT];

    // game Settings (they are not saved to disk)
    ConfigInt       hostorjoin;         // 1=host, 2=join
    ConfigBool      quickConnect;
    ConfigBool      needPassword;
    ConfigString    serverConnect;      // server to connect to

    static NPString* player_name;


    // server settings
    static int       server_port;
    static NPString* server_bindaddress;
    static NPString* server_motd;
    static bool      server_logging;
    static bool      server_public;
    static NPString* server_masterservers;
    static NPString* server_name;
    static bool      server_interactive_console;
    static NPString* server_authserver;
    static bool      server_authentication;

    static bool      sound_enable;
    static bool      sound_music;
    static int       sound_musicvol;
    static bool      sound_effects;
    static int       sound_effectsvol;

    static int       interface_attacknotificationtime;
    static int       interface_vehicleselectioncolor;
    static int       interface_unitselectionmode;
    static int       interface_unitinfodrawlayer;
    static int       interface_scrollrate;
    static int       interface_rankposition_x;
    static int       interface_rankposition_y;
    static int       interface_viewdrawbackgroundmode;

    // radar settings
    static int       radar_playerunitcolor;
    static int       radar_selectedunitcolor;
    static int       radar_alliedunitcolor;
    static int       radar_playeroutpostcolor;
    static int       radar_alliedoutpostcolor;
    static int       radar_enemyoutpostcolor;
    static int       radar_unitsize;

    // bot settings
    static int       bot_class;
    static bool      bot_allied;
    static int       bot_action_speed;



public:

    static NPString getUnitStyle(unsigned short style_vn)
    {
        std::vector<NPString> slist;
        NPString sl = *GameConfig::game_units_styles;
        string_to_params(sl, slist);
        NPString cstyle = slist[style_vn];
        return cstyle;
    }

    static unsigned short getUnitStylesNum()
    {
        std::vector<NPString> slist;
        NPString sl = *GameConfig::game_units_styles;
        unsigned short numc = 0;
        string_to_params(sl, slist);
        numc = slist.size();
        if ( numc > MAX_STYLES_NUM ) {
        LOGGER.warning("Too many unit styles provided by user. Max number is %d", MAX_STYLES_NUM);
        throw std::runtime_error("Stop!!!");
        }
        return numc;
    }


    const char* getGameTypeString() const
    {
        switch ( game_gametype )
        {
            case _gametype_objective :
                return( "Objective" );
            case _gametype_fraglimit :
                return( "Frag Limit" );
            case _gametype_timelimit :
                return( "Time Limit" );
            case _gametype_objectiveANDfraglimit :
                return( "Obj.+Frags" );
            case _gametype_fraglimitORtimelimit :
                return( "Frags+Time" );
        }
        return( "Unknown" );
    }


    int GetUnitsPerPlayer() const
    {
        return game_maxunits / game_maxplayers;
    }

    int GetTimeLimitSeconds() const
    {
        return game_timelimit * 60;
    }

    const char * getRespawnTypeString() const
    {
        switch( game_respawntype )
        {
            case _game_config_respawn_type_round_robin :
                return( "Round Robin" );
                break;

            case _game_config_respawn_type_random :
                return( "Random" );
                break;

            case _game_config_respawn_type_random_alt :
                return( "Random Alt" );
                break;

        } // ** switch

        assert(false);
        return( "Unknown" );
    }

    PIX getPlayerRadarUnitColor() const
    {
        return( colorEnumToPix( radar_playerunitcolor ) );
    }

    PIX getSelectedRadarUnitColor() const
    {
        return( colorEnumToPix( radar_selectedunitcolor ) );
    }

    PIX getAlliedRadarUnitColor() const
    {
        return( colorEnumToPix( radar_alliedunitcolor ) );
    }

    PIX getPlayerOutpostRadarColor() const
    {
        return( colorEnumToPix( radar_playeroutpostcolor ) );
    }

    PIX getAlliedOutpostRadarColor() const
    {
        return( colorEnumToPix( radar_alliedoutpostcolor ) );
    }

    PIX getEnemyOutpostRadarColor() const
    {
        return( colorEnumToPix( radar_enemyoutpostcolor ) );
    }

    PIX getVehicleSelectionBoxColor() const
    {
        return( colorEnumToPix( interface_vehicleselectioncolor ) );
    }

    const char *getMiniMapUnitSizeString() const
    {
        if (radar_unitsize == _mini_map_unit_size_small) {
            return( "Small" );
        } else if (radar_unitsize == _mini_map_unit_size_large) {
            return( "Large" );
        } else assert(false);
        return( "Undefined value" );
    }

private:
    friend class ScriptManager;
    static void registerScript(const NPString& table_name);

    std::string luaconfigfile;
    bool usePhysFS;

    PIX colorEnumToPix(int color_enum) const
    {
        switch( color_enum ) {
        case _color_aqua         : return( Color::unitAqua );
        case _color_yellow       : return( Color::unitYellow );
        case _color_red          : return( Color::unitRed );
        case _color_blue         : return( Color::unitBlue );
        case _color_dark_blue    : return( Color::unitDarkBlue );
        case _color_light_green  : return( Color::unitLightGreen );
        case _color_green        : return( Color::unitGreen );
        case _color_blue_gray    : return( Color::unitBlueGray );
        case _color_dark_red     : return( Color::unitDarkRed );
        case _color_black        : return( Color::unitBlack );
        case _color_dark_green   : return( Color::unitDarkGreen );
        case _color_white        : return( Color::unitWhite );
        case _color_light_orange : return( Color::unitLightOrange );
        case _color_orange       : return( Color::unitOrange );
        case _color_gray         : return( Color::unitGray );
        case _color_dark_gray    : return( Color::unitDarkGray );
        } // ** switch

        assert(false);
        return( Color::white );
    }
};

extern GameConfig* gameconfig;

#endif // ** __GAMECONFIG_HPP
