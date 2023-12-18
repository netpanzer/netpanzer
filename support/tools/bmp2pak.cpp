/*
Copyright (C) 2012 by Aaron Perez <aaronps@gmail.com>

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

#include <cstdlib>

#include <SDL2/SDL.h>

#include "Util/FileSystem.hpp"
#include "2D/PackedSurface.hpp"
#include "2D/Surface.hpp"
#include "2D/Palette.hpp"

#include <string>
#include <sstream>
#include <iomanip>

using namespace std;
/*******************************************************************/
// BEGIN hacks to avoid compiling everything just for this converter

typedef const struct s_ScriptVarBindRecord ScriptVarBindRecord;
struct lua_State;
struct luaL_Reg;

class ScriptManager
{
public:
    static void runFileInTable(const NPString& filename, const NPString& table);
    static void registerLib(const NPString& libname, const luaL_Reg * functions);
    static void bindStaticVariables(const NPString& objectName,
                                    const NPString& metaName,
                                    ScriptVarBindRecord * getters,
                                    ScriptVarBindRecord * setters);
};

void ScriptManager::runFileInTable(const NPString &filename, const NPString &table)
{
    return;
}

void ScriptManager::registerLib(const NPString &libname, const luaL_Reg *functions)
{
    return;
}

void ScriptManager::bindStaticVariables(const NPString &objectName, const NPString &metaName, ScriptVarBindRecord *getters, ScriptVarBindRecord *setters)
{
    return;
}

class ScriptHelper
{
public:
    static int get_byte (lua_State *L, void *v);
    static int set_byte (lua_State *L, void *v);
};

int ScriptHelper::get_byte(lua_State *L, void *v)
{
    return 0;
}

int ScriptHelper::set_byte(lua_State *L, void *v)
{
    return 0;
}

// END hacks
/*******************************************************************/

int main ( int argc, char** argv )
{
    printf("bmp2pak for NetPanzer V 1.0\n");
    if (argc < 3 )
    {
        printf("use: %s <filename.def> <outfile.pak>\n", argv[0]);
        printf("note: even on windows the path must be separated by '/'\n");
        printf("note2: the .bmp files has to be in same folder as .def file\n");
        return 1;
    }

    string filename = argv[1];
    string filename_noext = filename.substr(0,filename.size()-4);
    string outfile = argv[2];

    if ( ! PHYSFS_init(argv[0]) )
    {
        printf("Error initializing filesystem: %s", PHYSFS_getLastError());
        return 1;
    }

    PHYSFS_addToSearchPath(PHYSFS_getBaseDir(), 1);
    PHYSFS_setWriteDir(PHYSFS_getBaseDir());

    FILE *f = fopen(filesystem::getRealWriteName(filename.c_str()).c_str(), "r");
    if ( !f )
    {
        printf("Error: cannot open def file %s\n", filename.c_str());
        PHYSFS_deinit();
        return 1;
    }

    int width = -1, height = -1;
    int frames = -1;
    float fps = -1;
    int offset_x = -1, offset_y = -1;

    bool isError = false;

    isError = fscanf(f,"size=%d,%d\n", &width, &height) != 2;
    isError |= fscanf(f,"frames=%d\n", &frames) != 1;
    isError |= fscanf(f,"fps=%f\n", &fps) != 1;
    isError |= fscanf(f,"offset=%d,%d\n", &offset_x, &offset_y) != 2;

    fclose(f);

    if ( isError )
    {
        printf("Error loading def file, check it\n");
        PHYSFS_deinit();
        return 1;
    }

    printf("size=%d,%d\n", width, height);
    printf("frames=%d\n", frames);
    printf("fps=%f\n", fps);
    printf("offset=%d,%d\n", offset_x, offset_y);

    Surface unpacked(width, height, frames);

    unpacked.setFPS(fps);
    unpacked.setOffsetX(offset_x);
    unpacked.setOffsetY(offset_y);

    Surface bmp;

    for ( int n = 0; n < frames; n++ )
    {
        std::stringstream bfile;
        bfile << filename_noext << "_" << n << ".bmp";

        bmp.loadBMP(bfile.str().c_str());

        printf("Loaded: %s\n", bfile.str().c_str());

        unpacked.setFrame(n);
        unpacked.fill(0);
        bmp.blt(unpacked, 0, 0);
    }


    PackedSurface pak;
    pak.pack(unpacked);
    pak.setFPS(fps);
    pak.save(outfile);

    printf("Wrote to: %s\n", filesystem::getRealWriteName(outfile.c_str()).c_str());

    PHYSFS_deinit();
    printf("Exited cleanly\n");
    return 0;
}
