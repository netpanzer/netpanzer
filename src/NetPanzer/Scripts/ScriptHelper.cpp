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

 * Created on October 8, 2008, 10:53 PM
 */

#include <iostream>
#include <stdio.h>
#include <string.h>

#include "ScriptHelper.hpp"
#include "Util/Log.hpp"

#include "Core/CoreTypes.hpp"

static void notExistsError(lua_State *L )
{
    lua_Debug ar;
    lua_getstack(L,1,&ar);
    lua_getinfo(L, "Sl", &ar);
    LOGGER.warning("ERROR in '%s':%d: member '%s' doesn't exists.",
                   ar.source,
                   ar.currentline,
                   lua_tostring(L,2));
}

static void notModifiedError(lua_State *L, const char *what)
{
    lua_Debug ar;
    lua_getstack(L,1,&ar);
    lua_getinfo(L, "Sl", &ar);
    LOGGER.warning("ERROR in '%s':%d: '%s' not modified, value is not a %s.",
                   ar.source,
                   ar.currentline,
                   lua_tostring(L,2),
                   what);
}

int
ScriptHelper::get_byte (lua_State *L, void *v)
{
    lua_pushnumber(L, *(unsigned char*)v);
    return 1;
}

int
ScriptHelper::set_byte (lua_State *L, void *v)
{
    unsigned char c = lua_tonumber(L, 3);
    if ( !c && ! lua_isnumber(L, 3) )
    {
        notModifiedError(L,"number (byte)");
    }
    else
    {
        *(unsigned char*)v = c;
    }
    return 0;
}

int
ScriptHelper::get_int (lua_State *L, void *v)
{
    lua_pushnumber(L, *(int*)v);
    return 1;
}

int
ScriptHelper::set_int (lua_State *L, void *v)
{
    int n = lua_tonumber(L, 3);
    if ( !n && ! lua_isnumber(L, 3) )
    {
        notModifiedError(L,"number (int)");
    }
    else
    {
        *(int*)v = n;
    }
    return 0;
}

int
ScriptHelper::get_number (lua_State *L, void *v)
{
    lua_pushnumber(L, *(lua_Number*)v);
    return 1;
}

int
ScriptHelper::set_number (lua_State *L, void *v)
{
    lua_Number n = lua_tonumber(L, 3);
    if ( !n && ! lua_isnumber(L, 3) )
    {
        notModifiedError(L,"number (lua)");
    }
    else
    {
        *(lua_Number*)v = n;
    }
    return 0;
}

int
ScriptHelper::get_string (lua_State *L, void *v)
{
    NPString** strp = (NPString**)v;
    if ( strp )
    {
        lua_pushstring(L, (*strp)->c_str() );
    }
    else
    {
        lua_pushstring(L, "" );
    }
    return 1;
}

int
ScriptHelper::set_string (lua_State *L, void *v)
{
    const char * s = lua_tostring(L, 3);
    if ( !s )
    {
        notModifiedError(L,"string");
    }
    else
    {
        NPString** strp = (NPString**)v;
        if ( ! *strp )
        {
            *strp = new NPString(s);
        }
        else
        {
            (*strp)->assign(s);
        }
    }
    return 0;
}

int
ScriptHelper::get_boolean (lua_State *L, void *v)
{
    lua_pushboolean(L, *(bool*)v);
    return 1;
}

int
ScriptHelper::set_boolean (lua_State *L, void *v)
{
    *(bool*)v = lua_toboolean(L, 3);
    return 0;
}

int
ScriptHelper::staticVarCall(lua_State *L)
{
    /* for get: stack has userdata, index, lightuserdata */
    /* for set: stack has userdata, index, value, lightuserdata */
    ScriptVarBindRecord * m = (ScriptVarBindRecord *)lua_touserdata(L, -1);  /* member info */
    lua_pop(L, 1);                               /* drop lightuserdata */
    return m->func(L, (void *)(m->data));
}

int
ScriptHelper::index_handler (lua_State *L)
{
    /* stack has table, index */
    lua_pushvalue(L, 2);                     /* dup index */
    lua_rawget(L, lua_upvalueindex(1));      /* lookup member by name */
    if (!lua_islightuserdata(L, -1))
    {
        lua_pop(L, 1);                         /* drop value */
        lua_pushvalue(L, 2);                   /* dup index */
        lua_gettable(L, lua_upvalueindex(1));  /* else try methods */
        if (lua_isnil(L, -1))                  /* invalid member */
        {
            notExistsError(L);
        }
        return 1;
    }
    return staticVarCall(L);                      /* call get function */
}

int
ScriptHelper::newindex_handler (lua_State *L)
{
    /* stack has table, index, value */
    lua_pushvalue(L, 2);                     /* dup index */
    lua_rawget(L, lua_upvalueindex(1));      /* lookup member by name */
    if (!lua_islightuserdata(L, -1))         /* invalid member */
    {
        notExistsError(L);
        return 0;
    }
    return staticVarCall(L);                      /* call set function */
}

int
ScriptHelper::next_handler(lua_State* L)
{
    /* stack has table, key */
    ScriptVarBindRecord * record = (ScriptVarBindRecord *)lua_topointer(L, lua_upvalueindex(1));
    if ( ! lua_isnil(L,2) )
    {
        // find this key
        const char * key = lua_tostring(L, 2);

        while ( record->name && strcmp(record->name, key) )
        {
            ++record;
        }

        // want next key
        if ( record->name )
        {
            ++record;
        }
    }

    if ( record->name )
    {
        lua_pushstring(L, record->name);

        record->func(L, (void *)(record->data));

        return 2;
    }

    lua_pushnil(L);
    return 1;
}

int
ScriptHelper::autotable_indexhandler(lua_State* L)
{
    // stack has mtable, index
    lua_pushvalue(L, 2);
    // stack has mtable, index, index
    lua_rawget(L, 1);
    // stack has mtable, index, result
    if ( lua_isnil(L, -1) )
    {
        lua_pop(L, 1);
        // stack has mtable, index
        lua_createtable(L,0,0);
        // stack has mtable, index, ntable
        lua_pushvalue(L, -1);
        // stack has mtable, index, ntable, ntable
        lua_insert(L, 2);
        // stack has mtable, ntable, index, ntable
        lua_rawset(L, 1);
        // stack has mtable, ntable
        lua_createtable(L, 0, 1);
        // stack has mtable, ntable, meta
        lua_pushliteral(L, "__index");
        lua_pushcfunction(L, autotable_indexhandler);
        lua_rawset(L, -3);
        // stack has mtable, ntable, meta
        lua_setmetatable(L, -2);
        // stack has mtable, ntable
    }
    return 1;
}
