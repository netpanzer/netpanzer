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

#include <fstream>
#include <sstream>

#include "Util/FileSystem.hpp"
#include "Util/FileStream.hpp"
#include "Util/Log.hpp"
#include "Util/Exception.hpp"
#include "INIParser/Store.hpp"
#include "INIParser/Section.hpp"
#include "Interfaces/GameConfig.hpp"
#include "Core/NetworkGlobals.hpp"
#include "Views/Game/MiniMapView.hpp"
#include "Views/Game/MiniMapViewAlt.hpp"
#include "Views/GameViewGlobals.hpp"

#include "Scripts/ScriptManager.hpp"
#include "Scripts/ScriptHelper.hpp"

unsigned int GameConfig::video_width = 800;
unsigned int GameConfig::video_height = 600;
bool         GameConfig::video_fullscreen = false;
bool         GameConfig::video_hardwaresurface = false;
bool         GameConfig::video_doublebuffer = false;
bool         GameConfig::video_shadows = true;
bool         GameConfig::video_blendsmoke = true;
#ifdef _WIN32
bool         GameConfig::video_usedirectx = false;
#endif

bool GameConfig::interface_show_health = true;
bool GameConfig::interface_show_flags = true;
bool GameConfig::interface_show_names = true;

bool      GameConfig::game_enable_bases = true;
int       GameConfig::game_base_capture_mode = 1; // normal capture;
int       GameConfig::game_base_limit = 0; // no limit
int       GameConfig::game_autokicktime = 20; // minutes;
bool      GameConfig::game_allowmultiip = true;
NPString* GameConfig::game_unit_profiles = 0;
NPString* GameConfig::game_unit_spawnlist = 0;
NPString* GameConfig::game_adminpass = 0;
NPString* GameConfig::game_gamepass = 0;
int       GameConfig::game_changeflagtime = 1; // minutes
int       GameConfig::game_gametype = 0;
int       GameConfig::game_maxplayers = 8;
int       GameConfig::game_maxunits = 8*36;
int       GameConfig::game_timelimit = 30;
int       GameConfig::game_fraglimit = 300;
bool      GameConfig::game_powerups = true;
int       GameConfig::game_occupationpercentage = 75;
bool      GameConfig::game_allowallies = true;
int       GameConfig::game_cloudcoverage = 0;
int       GameConfig::game_respawntype = 0;
int       GameConfig::game_windspeed = 30;
int       GameConfig::game_lowscorelimit = -45;
int       GameConfig::game_anticheat = 3;
//bool      GameConfig::game_authentication = false;
//bool      GameConfig::game_bots_allowed = false;
//bool      GameConfig::game_scrambler = true;
//int       GameConfig::game_maxchatlines = 7;
NPString* GameConfig::game_map = 0;
NPString* GameConfig::game_mapcycle = 0;
NPString* GameConfig::game_mapstyle = 0;
NPString* GameConfig::game_units_styles = 0;

bool      GameConfig::sound_enable = true;
bool      GameConfig::sound_music = true;
int       GameConfig::sound_musicvol = 100;
bool      GameConfig::sound_effects = true;
int       GameConfig::sound_effectsvol = 100;

int       GameConfig::radar_playerunitcolor = _color_aqua;
int       GameConfig::radar_selectedunitcolor = _color_white;
int       GameConfig::radar_alliedunitcolor = _color_orange;
int       GameConfig::radar_playeroutpostcolor = _color_blue;
int       GameConfig::radar_alliedoutpostcolor = _color_orange;
int       GameConfig::radar_enemyoutpostcolor = _color_red;
int       GameConfig::radar_unitsize = _mini_map_unit_size_small;

int       GameConfig::interface_attacknotificationtime = 5;
int       GameConfig::interface_vehicleselectioncolor = _color_blue;
int       GameConfig::interface_unitselectionmode = _unit_selection_box_draw_mode_rect_edges;
int       GameConfig::interface_unitinfodrawlayer = 0;
int       GameConfig::interface_scrollrate = 1000;
int       GameConfig::interface_rankposition_x = 100;
int       GameConfig::interface_rankposition_y = 100;
int       GameConfig::interface_viewdrawbackgroundmode = (int)VIEW_BACKGROUND_DARK_GRAY_BLEND;

int       GameConfig::server_port = NETPANZER_DEFAULT_PORT_TCP;
NPString* GameConfig::server_bindaddress = 0;
NPString* GameConfig::server_motd = 0;
bool      GameConfig::server_logging = false;
bool      GameConfig::server_public = true;
NPString* GameConfig::server_masterservers = 0;
NPString* GameConfig::server_name = 0;
bool      GameConfig::server_interactive_console = true;
NPString* GameConfig::server_authserver = 0;
bool      GameConfig::server_authentication = false;

int       GameConfig::bot_class = 1;
bool      GameConfig::bot_allied = true;
int       GameConfig::bot_action_speed = 1;


NPString* GameConfig::player_name = 0;

Uint8 GameConfig::player_flag_data[FLAG_WIDTH*FLAG_HEIGHT] = {0};

#define WRITE_BOOL(v) ((v)?"true":"false")

// This generates the tables needed for script binding
static const ScriptVarBindRecord video_getters[] =
{
    { "width",           GETSVTYPE_INT,     &GameConfig::video_width },
    { "height",          GETSVTYPE_INT,     &GameConfig::video_height },
    { "fullscreen",      GETSVTYPE_BOOLEAN, &GameConfig::video_fullscreen },
    { "hardwaresurface", GETSVTYPE_BOOLEAN, &GameConfig::video_hardwaresurface },
    { "doublebuffer",    GETSVTYPE_BOOLEAN, &GameConfig::video_doublebuffer },
    { "shadows",         GETSVTYPE_BOOLEAN, &GameConfig::video_shadows },
    { "blendsmoke",      GETSVTYPE_BOOLEAN, &GameConfig::video_blendsmoke },
#ifdef _WIN32
    { "usedirectx",      GETSVTYPE_BOOLEAN, &GameConfig::video_usedirectx },
#endif
    {0,0}
};

static const ScriptVarBindRecord video_setters[] =
{
    { "width",           SETSVTYPE_INT,     &GameConfig::video_width },
    { "height",          SETSVTYPE_INT,     &GameConfig::video_height },
    { "fullscreen",      SETSVTYPE_BOOLEAN, &GameConfig::video_fullscreen },
    { "hardwaresurface", SETSVTYPE_BOOLEAN, &GameConfig::video_hardwaresurface },
    { "doublebuffer",    SETSVTYPE_BOOLEAN, &GameConfig::video_doublebuffer },
    { "shadows",         SETSVTYPE_BOOLEAN, &GameConfig::video_shadows },
    { "blendsmoke",      SETSVTYPE_BOOLEAN, &GameConfig::video_blendsmoke },
#ifdef _WIN32
    { "usedirectx",      SETSVTYPE_BOOLEAN, &GameConfig::video_usedirectx },
#endif
    {0,0}
};


static const ScriptVarBindRecord interface_getters[] =
{
    { "show_health",            GETSVTYPE_BOOLEAN, &GameConfig::interface_show_health },
    { "show_flags",             GETSVTYPE_BOOLEAN, &GameConfig::interface_show_flags },
    { "show_names",             GETSVTYPE_BOOLEAN, &GameConfig::interface_show_names },
    { "attacknotificationime",  GETSVTYPE_INT,     &GameConfig::interface_attacknotificationtime},
    { "vehicleselectioncolor",  GETSVTYPE_INT,     &GameConfig::interface_vehicleselectioncolor},
    { "unitselectionmode",      GETSVTYPE_INT,     &GameConfig::interface_unitselectionmode},
    { "unitinfodrawlayer",      GETSVTYPE_INT,     &GameConfig::interface_unitinfodrawlayer},
    { "scrollrate",             GETSVTYPE_INT,     &GameConfig::interface_scrollrate},
    { "rankposition_x",         GETSVTYPE_INT,     &GameConfig::interface_rankposition_x},
    { "rankposition_y",         GETSVTYPE_INT,     &GameConfig::interface_rankposition_y},
    { "viewdrawbackgroundmode", GETSVTYPE_INT,     &GameConfig::interface_viewdrawbackgroundmode},
    {0,0}
};

static const ScriptVarBindRecord interface_setters[] =
{
    { "show_health",            SETSVTYPE_BOOLEAN, &GameConfig::interface_show_health },
    { "show_flags",             SETSVTYPE_BOOLEAN, &GameConfig::interface_show_flags },
    { "show_names",             SETSVTYPE_BOOLEAN, &GameConfig::interface_show_names },
    { "attacknotificationime",  SETSVTYPE_INT,     &GameConfig::interface_attacknotificationtime},
    { "vehicleselectioncolor",  SETSVTYPE_INT,     &GameConfig::interface_vehicleselectioncolor},
    { "unitselectionmode",      SETSVTYPE_INT,     &GameConfig::interface_unitselectionmode},
    { "unitinfodrawlayer",      SETSVTYPE_INT,     &GameConfig::interface_unitinfodrawlayer},
    { "scrollrate",             SETSVTYPE_INT,     &GameConfig::interface_scrollrate},
    { "rankposition_x",         SETSVTYPE_INT,     &GameConfig::interface_rankposition_x},
    { "rankposition_y",         SETSVTYPE_INT,     &GameConfig::interface_rankposition_y},
    { "viewdrawbackgroundmode", SETSVTYPE_INT,     &GameConfig::interface_viewdrawbackgroundmode},
    {0,0}
};

static const ScriptVarBindRecord game_getters[] =
{
    { "enable_bases",       GETSVTYPE_BOOLEAN, &GameConfig::game_enable_bases },
    { "base_capture_mode",  GETSVTYPE_INT,     &GameConfig::game_base_capture_mode },
    { "base_limit",         GETSVTYPE_INT,     &GameConfig::game_base_limit },
    { "autokicktime",       GETSVTYPE_INT,     &GameConfig::game_autokicktime },
    { "allowmultiip",       GETSVTYPE_BOOLEAN, &GameConfig::game_allowmultiip },
    { "unit_profiles",      GETSVTYPE_STRING,  &GameConfig::game_unit_profiles},
    { "unit_spawnlist",     GETSVTYPE_STRING,  &GameConfig::game_unit_spawnlist},
    { "adminpass",          GETSVTYPE_STRING,  &GameConfig::game_adminpass},
    { "gamepass",           GETSVTYPE_STRING,  &GameConfig::game_gamepass},
    { "changeflagtime",     GETSVTYPE_INT,     &GameConfig::game_changeflagtime},
    { "gametype",           GETSVTYPE_INT,     &GameConfig::game_gametype },
    { "maxplayers",         GETSVTYPE_INT,     &GameConfig::game_maxplayers },
    { "maxunits",           GETSVTYPE_INT,     &GameConfig::game_maxunits },
    { "timelimit",          GETSVTYPE_INT,     &GameConfig::game_timelimit },
    { "fraglimit",          GETSVTYPE_INT,     &GameConfig::game_fraglimit },
    { "powerups",           GETSVTYPE_BOOLEAN, &GameConfig::game_powerups },
    { "occupationpercentage",GETSVTYPE_INT,    &GameConfig::game_occupationpercentage },
    { "allowallies",        GETSVTYPE_BOOLEAN, &GameConfig::game_allowallies },
    { "cloudcoverage",      GETSVTYPE_INT,     &GameConfig::game_cloudcoverage },
    { "respawntype",        GETSVTYPE_INT,     &GameConfig::game_respawntype },
    { "windspeed",          GETSVTYPE_INT,     &GameConfig::game_windspeed },
    { "lowscorelimit",      GETSVTYPE_INT,     &GameConfig::game_lowscorelimit },
    { "anticheat",          GETSVTYPE_INT,     &GameConfig::game_anticheat },
    //{ "authentication",     GETSVTYPE_BOOLEAN, &GameConfig::game_authentication },
    //{ "bots_allowed",       GETSVTYPE_BOOLEAN, &GameConfig::game_bots_allowed },
    //{ "scrambler",          GETSVTYPE_BOOLEAN, &GameConfig::game_scrambler },
    //{ "maxchatlines",       GETSVTYPE_INT,     &GameConfig::game_maxchatlines },
    { "map",                GETSVTYPE_STRING,  &GameConfig::game_map },
    { "mapcycle",           GETSVTYPE_STRING,  &GameConfig::game_mapcycle },
    { "mapstyle",           GETSVTYPE_STRING,  &GameConfig::game_mapstyle },
    { "units_styles",       GETSVTYPE_STRING,  &GameConfig::game_units_styles },
    {0,0}
};

static const ScriptVarBindRecord game_setters[] =
{
    { "enable_bases",       SETSVTYPE_BOOLEAN, &GameConfig::game_enable_bases },
    { "base_capture_mode",  SETSVTYPE_INT,     &GameConfig::game_base_capture_mode },
    { "base_limit",         SETSVTYPE_INT,     &GameConfig::game_base_limit },
    { "autokicktime",       SETSVTYPE_INT,     &GameConfig::game_autokicktime },
    { "allowmultiip",       SETSVTYPE_BOOLEAN, &GameConfig::game_allowmultiip },
    { "unit_profiles",      SETSVTYPE_STRING,  &GameConfig::game_unit_profiles},
    { "unit_spawnlist",     SETSVTYPE_STRING,  &GameConfig::game_unit_spawnlist},
    { "adminpass",          SETSVTYPE_STRING,  &GameConfig::game_adminpass},
    { "gamepass",           SETSVTYPE_STRING,  &GameConfig::game_gamepass},
    { "changeflagtime",     SETSVTYPE_INT,     &GameConfig::game_changeflagtime},
    { "gametype",           SETSVTYPE_INT,     &GameConfig::game_gametype },
    { "maxplayers",         SETSVTYPE_INT,     &GameConfig::game_maxplayers },
    { "maxunits",           SETSVTYPE_INT,     &GameConfig::game_maxunits },
    { "timelimit",          SETSVTYPE_INT,     &GameConfig::game_timelimit },
    { "fraglimit",          SETSVTYPE_INT,     &GameConfig::game_fraglimit },
    { "powerups",           SETSVTYPE_BOOLEAN, &GameConfig::game_powerups },
    { "occupationpercentage",SETSVTYPE_INT,    &GameConfig::game_occupationpercentage },
    { "allowallies",        SETSVTYPE_BOOLEAN, &GameConfig::game_allowallies },
    { "cloudcoverage",      SETSVTYPE_INT,     &GameConfig::game_cloudcoverage },
    { "respawntype",        SETSVTYPE_INT,     &GameConfig::game_respawntype },
    { "windspeed",          SETSVTYPE_INT,     &GameConfig::game_windspeed },
    { "lowscorelimit",      SETSVTYPE_INT,     &GameConfig::game_lowscorelimit },
    { "anticheat",          SETSVTYPE_INT,     &GameConfig::game_anticheat },
    //{ "authentication",     SETSVTYPE_BOOLEAN, &GameConfig::game_authentication },
    //{ "bots_allowed",       SETSVTYPE_BOOLEAN, &GameConfig::game_bots_allowed },
    //{ "scrambler",          SETSVTYPE_BOOLEAN, &GameConfig::game_scrambler },
    //{ "maxchatlines",       SETSVTYPE_INT,     &GameConfig::game_maxchatlines },
    { "map",                SETSVTYPE_STRING,  &GameConfig::game_map },
    { "mapcycle",           SETSVTYPE_STRING,  &GameConfig::game_mapcycle },
    { "mapstyle",           SETSVTYPE_STRING,  &GameConfig::game_mapstyle },
    { "units_styles",       SETSVTYPE_STRING,  &GameConfig::game_units_styles },
    {0,0}
};

static const ScriptVarBindRecord sound_getters[] =
{
    { "enable",             GETSVTYPE_BOOLEAN, &GameConfig::sound_enable},
    { "music",              GETSVTYPE_BOOLEAN, &GameConfig::sound_music},
    { "musicvol",           GETSVTYPE_INT,     &GameConfig::sound_musicvol},
    { "effects",            GETSVTYPE_BOOLEAN, &GameConfig::sound_effects},
    { "effectsvol",         GETSVTYPE_INT,     &GameConfig::sound_effectsvol},
    {0,0}
};

static const ScriptVarBindRecord sound_setters[] =
{
    { "enable",             SETSVTYPE_BOOLEAN, &GameConfig::sound_enable},
    { "music",              SETSVTYPE_BOOLEAN, &GameConfig::sound_music},
    { "musicvol",           SETSVTYPE_INT,     &GameConfig::sound_musicvol},
    { "effects",            SETSVTYPE_BOOLEAN, &GameConfig::sound_effects},
    { "effectsvol",         SETSVTYPE_INT,     &GameConfig::sound_effectsvol},
    {0,0}
};

static const ScriptVarBindRecord radar_getters[] =
{
    { "playerunitcolor",    GETSVTYPE_INT,     &GameConfig::radar_playerunitcolor},
    { "selectedunitcolor",  GETSVTYPE_INT,     &GameConfig::radar_selectedunitcolor},
    { "alliedunitcolor",    GETSVTYPE_INT,     &GameConfig::radar_alliedunitcolor},
    { "playeroutpostcolor", GETSVTYPE_INT,     &GameConfig::radar_playeroutpostcolor},
    { "alliedoutpostcolor", GETSVTYPE_INT,     &GameConfig::radar_alliedoutpostcolor},
    { "enemyoutpostcolor",  GETSVTYPE_INT,     &GameConfig::radar_enemyoutpostcolor},
    { "unitsize",           GETSVTYPE_INT,     &GameConfig::radar_unitsize},
    {0,0}
};

static const ScriptVarBindRecord radar_setters[] =
{
    { "playerunitcolor",    SETSVTYPE_INT,     &GameConfig::radar_playerunitcolor},
    { "selectedunitcolor",  SETSVTYPE_INT,     &GameConfig::radar_selectedunitcolor},
    { "alliedunitcolor",    SETSVTYPE_INT,     &GameConfig::radar_alliedunitcolor},
    { "playeroutpostcolor", SETSVTYPE_INT,     &GameConfig::radar_playeroutpostcolor},
    { "alliedoutpostcolor", SETSVTYPE_INT,     &GameConfig::radar_alliedoutpostcolor},
    { "enemyoutpostcolor",  SETSVTYPE_INT,     &GameConfig::radar_enemyoutpostcolor},
    { "unitsize",           SETSVTYPE_INT,     &GameConfig::radar_unitsize},
    {0,0}
};

static const ScriptVarBindRecord server_getters[] =
{
    { "port",                       GETSVTYPE_INT,     &GameConfig::server_port },
    { "bindaddress",                GETSVTYPE_STRING,  &GameConfig::server_bindaddress },
    { "motd",                       GETSVTYPE_STRING,  &GameConfig::server_motd },
    { "logging",                    GETSVTYPE_BOOLEAN, &GameConfig::server_logging },
    { "public",                     GETSVTYPE_BOOLEAN, &GameConfig::server_public },
    { "masterservers",              GETSVTYPE_STRING,  &GameConfig::server_masterservers },
    { "name",                       GETSVTYPE_STRING,  &GameConfig::server_name },
    { "interactive_console",        GETSVTYPE_BOOLEAN, &GameConfig::server_interactive_console },
    { "authserver",                 GETSVTYPE_STRING,  &GameConfig::server_authserver },
    { "authentication",             GETSVTYPE_BOOLEAN, &GameConfig::server_authentication },
    {0,0}
};

static const ScriptVarBindRecord server_setters[] =
{
    { "port",                SETSVTYPE_INT,     &GameConfig::server_port },
    { "bindaddress",         SETSVTYPE_STRING,  &GameConfig::server_bindaddress },
    { "motd",                SETSVTYPE_STRING,  &GameConfig::server_motd },
    { "logging",             SETSVTYPE_BOOLEAN, &GameConfig::server_logging },
    { "public",              SETSVTYPE_BOOLEAN, &GameConfig::server_public },
    { "masterservers",       SETSVTYPE_STRING,  &GameConfig::server_masterservers },
    { "name",                SETSVTYPE_STRING,  &GameConfig::server_name },
    { "interactive_console", SETSVTYPE_BOOLEAN, &GameConfig::server_interactive_console },
    { "authserver",          SETSVTYPE_STRING,  &GameConfig::server_authserver },
    { "authentication",      SETSVTYPE_BOOLEAN, &GameConfig::server_authentication },
    {0,0}
};

static const ScriptVarBindRecord player_getters[] =
{
    { "name",           GETSVTYPE_STRING,  &GameConfig::player_name },
    {0,0}
};

static const ScriptVarBindRecord player_setters[] =
{
    { "name",           SETSVTYPE_STRING,  &GameConfig::player_name },
    {0,0}
};

static const ScriptVarBindRecord bot_getters[] =
{
    { "class",           GETSVTYPE_INT,  &GameConfig::bot_class },
    { "allied",          GETSVTYPE_BOOLEAN,  &GameConfig::bot_allied },
    { "action_speed",    GETSVTYPE_INT,  &GameConfig::bot_action_speed },
    {0,0}
};

static const ScriptVarBindRecord bot_setters[] =
{
    { "class",           SETSVTYPE_INT,  &GameConfig::bot_class },
    { "allied",          SETSVTYPE_BOOLEAN,  &GameConfig::bot_allied },
    { "action_speed",    SETSVTYPE_INT,  &GameConfig::bot_action_speed },
    {0,0}
};




void GameConfig::registerScript(const NPString& table_name)
{
    if ( ! game_unit_profiles )
    {
        game_unit_profiles = new NPString(DEFAULT_UNIT_PROFILES);
    }

    if ( ! game_unit_spawnlist )
    {
        game_unit_spawnlist = new NPString("1, 1, 1, 1, 1, 1, 1, 1, 1, 1");
    }

    if ( ! game_adminpass )
    {
        game_adminpass = new NPString("");
    }

    if ( ! game_gamepass )
    {
        game_gamepass = new NPString("");
    }

    if ( ! game_map )
    {
        game_map = new NPString("");
    }

    if ( ! game_mapcycle )
    {
        game_mapcycle = new NPString("Two clans, Bullet Hole");
    }

    if ( ! game_mapstyle )
    {
        game_mapstyle = new NPString("SummerDay");
    }

    if ( ! game_units_styles )
    {
        game_units_styles = new NPString(DEFAULT_UNITS_STYLES);
    }

    if ( ! server_bindaddress )
    {
        server_bindaddress = new NPString("");
    }

    if ( ! server_motd )
    {
        server_motd = new NPString("Welcome to NetPanzer Server");
    }

    if ( ! server_masterservers )
    {
        server_masterservers = new NPString("netpanzer.io");
    }

    if ( ! server_name )
    {
        server_name = new NPString("NetPanzer Server");
    }

    if ( ! server_authserver )
    {
        server_authserver = new NPString("authserver.netpanzer.info");
    }

    if ( ! player_name )
    {
        player_name = new NPString("Player");
    }

    ScriptManager::bindStaticVariables(table_name + ".video",
                                       "ConfigVideoMetaTable",
                                       video_getters, video_setters);

    ScriptManager::bindStaticVariables(table_name + ".interface",
                                       "ConfigInterfaceMetaTable",
                                       interface_getters, interface_setters);

    ScriptManager::bindStaticVariables(table_name + ".game",
                                       "ConfigGameMetaTable",
                                       game_getters, game_setters);

    ScriptManager::bindStaticVariables(table_name + ".sound",
                                       "ConfigSoundMetaTable",
                                       sound_getters, sound_setters);

    ScriptManager::bindStaticVariables(table_name + ".radar",
                                       "ConfigRadarMetaTable",
                                       radar_getters, radar_setters);

    ScriptManager::bindStaticVariables(table_name + ".interface",
                                       "ConfigInterfaceMetaTable",
                                       interface_getters, interface_setters);

    ScriptManager::bindStaticVariables(table_name + ".server",
                                       "ConfigServerMetaTable",
                                       server_getters, server_setters);

    ScriptManager::bindStaticVariables(table_name + ".player",
                                       "ConfigPlayerMetaTable",
                                       player_getters, player_setters);

    ScriptManager::bindStaticVariables(table_name + ".bot",
                                       "ConfigBotMetaTable",
                                       bot_getters, bot_setters);

}

GameConfig::GameConfig(const std::string& luaconfigfile,bool usePhysFS)
    // VariableName("Name", value [, minimum, maximum])
    :
      hostorjoin("hostorjoin", _game_session_join, 0, _game_session_last-1),
      quickConnect("quickconnect", false),
      needPassword("needpassword", false),
      serverConnect("serverconnect", "")

{
    this->luaconfigfile = luaconfigfile;
    this->usePhysFS = usePhysFS;

    std::stringstream default_player;
    default_player << "Player" << (rand()%1000);
    if ( ! player_name )
    {
        player_name = new NPString();
    }

    player_name->assign(default_player.str());

    try
    {
        loadConfig();
    }
    catch(std::exception& e)
    {
        LOG(("couldn't read game configuration: %s", e.what()));
        LOG(("Using default config. (this is normal on first startup)"));
    }
}

GameConfig::~GameConfig()
{
    try
    {
        saveConfig();
    }
    catch(std::exception& e)
    {
        LOG(("couldn't save game configuration: %s", e.what()));
    }
}

void GameConfig::loadConfig()
{
    ScriptManager::loadConfigFile(luaconfigfile.c_str(), "config");

// these lines might be usefull infuture? 2012-01-18
//    if(usePhysFS)
//    {
//        IFileStream in(configfile);
//        inifile.load(in);
//    } else {
//        std::ifstream in(configfile.c_str());
//        inifile.load(in);
//    }

    if ( player_name->length() == 0 )
    {
        std::stringstream default_player;
        default_player << "Player" << (rand()%1000);
        player_name->assign(default_player.str());
    }

    if ( game_mapcycle->length() == 0 )
    {
        game_mapcycle->assign("Two clans");
    }

    // TODO throw if max_players > 255 players (player id is int8)
}

void GameConfig::saveConfig()
{
    lua_State *L = ScriptManager::getLuavm();

    lua_getglobal(L,"config");
    if ( ! lua_istable(L, -1) )
    {
        LOGGER.warning("ERROR: Can't save configuration, config doesn't exist.");
        return;
    }

    lua_pushliteral(L, "dump");
    lua_rawget(L, -2);
    if ( ! lua_isfunction(L, -1) )
    {
        LOGGER.warning("ERROR: Can't save configuration, config.dump function doesn't exist.");
        lua_pop(L, 2);
        return;
    }

    lua_pushvalue(L, -2);
    lua_remove(L, -3);

    if ( lua_pcall(L, 1, 1, 0) )
    {
        LOGGER.warning("ERROR: Can't save configuration, Lua error: '%s'", lua_tostring(L, -1));
        lua_pop(L, 1);
        return;
    }

    if ( ! lua_isstring(L, -1) )
    {
        LOGGER.warning("ERROR: Can't save configuration, dump result is not a string.");
        lua_pop(L, 1);
        return;
    }

    OFileStream out(luaconfigfile.c_str());
    out << lua_tostring(L, -1) << std::endl;
    lua_pop(L, 1);

}

GameConfig* gameconfig = 0;
