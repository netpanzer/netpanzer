/*
Copyright (C) 2010 by Aaron Perez <aaronps@gmail.com>

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
 *
 * Created on June 6, 2010, 08:58 AM
 */

#include "ScriptManager.hpp"
#include "ScriptHelper.hpp"

#include "Interfaces/ChatInterface.hpp"
#include "Interfaces/GameManager.hpp"
#include "Interfaces/ConsoleInterface.hpp"
#include "Interfaces/PlayerInterface.hpp"
#include "Interfaces/MapsManager.hpp"
#include "Interfaces/GameControlRulesDaemon.hpp"
#include "Units/UnitInterface.hpp"
#include "Classes/Network/NetworkServer.hpp"
#include "Util/FileSystem.hpp"

#include <sstream>

static int npmodule_say (lua_State *L)
{
    ChatInterface::say(lua_tolstring(L,1,0));
    return 0;
}

static int npmodule_teamsay (lua_State *L)
{
    ChatInterface::teamsay(lua_tolstring(L,1,0));
    return 0;
}

static int npmodule_serversay (lua_State *L)
{
    ChatInterface::serversay(lua_tolstring(L,1,0));
    return 0;
}

static int npmodule_serversayto (lua_State *L)
{
    int n = lua_tonumber(L, 1);
    if ( !n && ! lua_isnumber(L, 1) )
    {
        // TODO it is missing a parameter or it is not a number, do something
    }
    else
    {
        const char * str = lua_tolstring(L,2,0);
        if ( str )
        {
            ChatInterface::serversayTo(n, str);
        }
    }
    return 0;
}

static int npmodule_scriptmessage (lua_State *L)
{
    ConsoleInterface::postMessage(Color::cyan, false, 0, "* %s", lua_tolstring(L,1,0) );
    return 0;
}

static int npmodule_quit (lua_State *L)
{
    GameManager::exitNetPanzer();
    return 0;
}

static int npmodule_kick (lua_State *L)
{
    int n = lua_tonumber(L, 1);
    if ( !n && ! lua_isnumber(L, 1) )
    {
        // TODO it is missing a parameter or it is not a number, do something
    }
    else
    {
        SERVER->kickClient(SERVER->getClientSocketByPlayerIndex((unsigned short)n));
    }
    return 0;
}

static int npmodule_listPlayers (lua_State *L)
{
    int tindex = 0; // will push ++tindex
    lua_newtable(L);


    for ( int n = 0; n < PlayerInterface::getMaxPlayers(); n++)
    {
        PlayerState *p = PlayerInterface::getPlayer(n);
        if ( ! p->isFree() )
        {
            lua_newtable(L);
            lua_pushinteger(L, n);
            lua_setfield(L, -2, "id");

            lua_pushstring(L, p->isAllocated() ? "<preconnect>" : p->getName().c_str());
            lua_setfield(L, -2, "name");

            lua_pushstring(L, PlayerInterface::isLocalPlayer(n) ? "local" : SERVER->getIP(n).c_str());
            lua_setfield(L, -2, "ip");


            char str_buffer [16];
            sprintf (str_buffer, "%.1f", p->getDownAvgPing());
            lua_pushstring(L, p->getDownAvgPing() != 0 ? str_buffer : "n/a");
            lua_setfield(L, -2, "s_ping");

            lua_rawseti(L, -2, ++tindex);
        }
    }

    return 1;
}

static int npmodule_map (lua_State *L)
{
    const char *mname = lua_tolstring(L,1,0);
    std::stringstream ss("");

    if ( mname )
    {
        NPString map_name(mname);

        if ( !MapsManager::existsMap(map_name) )
        {
            ss << "Map '" << map_name << "' doesn't exists";
        }
        else
        {
            ss << "Switching to map '" << map_name << "'";
            GameControlRulesDaemon::forceMapChange(map_name);
        }
    }
    else
    {
        ss << "Missing map name parameter";
    }

    lua_pushstring( L, ss.str().c_str() );
    return 1;
}

static int npmodule_listMaps (lua_State *L)
{
    const char mapsPath[] = "maps/";

    // scan directory for .npm files
    std::string suffix = ".npm";
    char **list = filesystem::enumerateFiles(mapsPath);

    int tindex = 0; // will push ++tindex
    lua_newtable(L);

    for (char **i = list; *i != NULL; i++)
    {
        std::string filename = mapsPath;
        filename.append(*i);

        if ( !filesystem::isDirectory(filename.c_str()) )
        {
            if ( filename.size() >= suffix.size()
                && (filename.compare( filename.size() - suffix.size(),
                                      suffix.size(), suffix) == 0) )
            {
                std::string mapname;
                size_t p = 0;
                char c;
                while( (c = (*i)[p++]) != 0) {
                    if(c == '.')
                        break;
                    mapname += c;
                }

                lua_pushstring( L, mapname.c_str() );
                lua_rawseti(L, -2, ++tindex);
            }
        }
    }
    filesystem::freeList(list);

    return 1;
}

static int npmodule_listProfiles (lua_State *L)
{
    const char profilesPath[] = "units/profiles/";

    // scan directory for .upf files
    std::string suffix = ".upf";
    char **list = filesystem::enumerateFiles(profilesPath);

    int tindex = 0; // will push ++tindex
    lua_newtable(L);

    for (char **i = list; *i != NULL; i++)
    {
        std::string filename = profilesPath;
        filename.append(*i);

        if ( !filesystem::isDirectory(filename.c_str()) )
        {
            if ( filename.size() >= suffix.size()
                && (filename.compare( filename.size() - suffix.size(),
                                      suffix.size(), suffix) == 0) )
            {
                std::string profilename;
                size_t p = 0;
                char c;
                while( (c = (*i)[p++]) != 0) {
                    if(c == '.')
                        break;
                    profilename += c;
                }

                lua_pushstring( L, profilename.c_str() );
                lua_rawseti(L, -2, ++tindex);
            }
        }
    }
    filesystem::freeList(list);

    return 1;
}

static int npmodule_SyncFlagTimer (lua_State *L)
{
    PlayerInterface::SyncFlagTimer();
    return 0;
}

static int npmodule_destroyPlayerUnits(lua_State *L)
{
    int n = lua_tonumber(L, 1);
    if ( !n && ! lua_isnumber(L, 1) )
    {
        // TODO it is missing a parameter or it is not a number, do something
    }
    else
    {
        if ( n >= 0 && n < PlayerInterface::getMaxPlayers() )
        {
            UnitInterface::destroyPlayerUnits( n );
        }
    }
    return 0;
}

static const luaL_Reg npmodule[] =
{
    {"say",           npmodule_say},
    {"teamsay",       npmodule_teamsay},
    {"serversay",     npmodule_serversay},
    {"serversayto",   npmodule_serversayto},
    {"scriptmessage", npmodule_scriptmessage},
    {"quit",          npmodule_quit},
    {"kick",          npmodule_kick},
    {"listplayers",   npmodule_listPlayers},
    {"map",           npmodule_map},
    {"listmaps",      npmodule_listMaps},
    {"listprofiles",  npmodule_listProfiles},
    {"syncflagtimer", npmodule_SyncFlagTimer},
    {"destroyplayerunits", npmodule_destroyPlayerUnits},
    {NULL, NULL}
};

int npmodule_load (lua_State *L)
{
  luaL_register(L, "netpanzer", npmodule);
  return 1;
}
