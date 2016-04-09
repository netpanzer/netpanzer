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

 * Created on October 8, 2008, 10:48 PM
 */

#ifndef _SCRIPTHELPER_HPP
#define	_SCRIPTHELPER_HPP

#include "lua/lua.hpp"

#define GETSVTYPE_BYTE ScriptHelper::get_byte
#define SETSVTYPE_BYTE ScriptHelper::set_byte
#define GETSVTYPE_INT ScriptHelper::get_int
#define SETSVTYPE_INT ScriptHelper::set_int
#define GETSVTYPE_NUMBER ScriptHelper::get_number
#define SETSVTYPE_NUMBER ScriptHelper::set_number
#define GETSVTYPE_STRING ScriptHelper::get_string
#define SETSVTYPE_STRING ScriptHelper::set_string
#define GETSVTYPE_BOOLEAN ScriptHelper::get_boolean
#define SETSVTYPE_BOOLEAN ScriptHelper::set_boolean

typedef int (*ScriptBindFunction) (lua_State *L, void *v);

/* member info for get and set handlers */
typedef const struct s_ScriptVarBindRecord
{
    const char *name;        /* var name */
    ScriptBindFunction func; /* get or set function for type of data */
    void * data;             /* pointer to your data */
}  ScriptVarBindRecord;

class ScriptHelper
{
public:
    static int get_byte (lua_State *L, void *v);
    static int set_byte (lua_State *L, void *v);

    static int get_int (lua_State *L, void *v);
    static int set_int (lua_State *L, void *v);

    static int get_number (lua_State *L, void *v);
    static int set_number (lua_State *L, void *v);

    static int get_string (lua_State *L, void *v);
    static int set_string (lua_State *L, void *v);

    static int get_boolean (lua_State *L, void *v);
    static int set_boolean (lua_State *L, void *v);

private:
    friend class ScriptManager;
    
    static int staticVarCall(lua_State *L);
    static int index_handler (lua_State *L);
    static int newindex_handler (lua_State *L);
    static int next_handler(lua_State *L);
    static int autotable_indexhandler(lua_State *L);
};

#endif	/* _SCRIPTHELPER_HPP */

