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
    printf("pak2bmp for NetPanzer V 1.1\n");
    if (argc < 3 )
    {
        printf("use: %s <filename> <output_folder> [palette_file_optional]\n", argv[0]);
        printf("note: if using palette file, use only the name without extension or path\n");
        printf("note2: even on windows the path must be separated by '/'\n");
        printf("note3: YOU have to create the output directory\n");
        printf("example for using palete (default is netp):\n");
        printf("%s units/pics/pak/TitaHNSD.pak titan_body netp\n", argv[0]);
        return 1;
    }

    string filename = argv[1];

    string::size_type last_slash = filename.find_last_of('/');

    string filename_noext = filename.substr(last_slash != string::npos ? last_slash : 0);
    filename_noext = filename_noext.substr(0,filename_noext.size()-4);
    string palettefile = "netp";
    if ( argc >= 4 )
    {
        palettefile = argv[3];
    }
    string outdir = argv[2];


    printf("out dir is '%s'\n", outdir.c_str());

    if ( ! PHYSFS_init(argv[0]) )
    {
        printf("Error initializing filesystem: %s", PHYSFS_getLastError());
        return 1;
    }

    PHYSFS_addToSearchPath(PHYSFS_getBaseDir(), 1);
    PHYSFS_setWriteDir(PHYSFS_getBaseDir());

    PackedSurface pak;
    pak.load(filename);


    std::stringstream deffile;
    deffile << "/" << outdir << "/" << filename_noext << ".def";

    FILE *f = fopen(filesystem::getRealWriteName(deffile.str().c_str()).c_str(), "w");
    if ( !f )
    {
        printf("Error: cannot write files in %s\n", outdir.c_str());
        PHYSFS_deinit();
        return 1;
    }

    fprintf(f,"size=%d,%d\n", pak.getWidth(), pak.getHeight());
    fprintf(f,"frames=%d\n", pak.getFrameCount());
    fprintf(f,"fps=%f\n", pak.getFPS());
    fprintf(f,"offset=%d,%d\n", pak.getOffsetX(), pak.getOffsetY());

    fclose(f);

    printf("def file=%s\n",filesystem::getRealWriteName(deffile.str().c_str()).c_str());

    Surface unpacked(pak.getWidth(), pak.getHeight(), 1);

    SDL_Surface *surf = SDL_CreateRGBSurfaceFrom( unpacked.getFrame0(),
                                                  unpacked.getWidth(),
                                                 unpacked.getHeight(),
                                                 8,
                                                 unpacked.getPitch(),
                                                 0,0,0,0);


    if ( ! surf )
    {
        printf("surface is null! we will die.");
    }

    Palette::loadACT(palettefile);
    SDL_SetColors(surf, Palette::color, 0, 256);

    printf("There are %d frames\n", pak.getFrameCount());

    for ( int n = 0; n < pak.getFrameCount(); n++ )
    {
        unpacked.fill(0);
        pak.setFrame(n);
        pak.blt(unpacked, 0, 0);
        std::stringstream ofile;
        ofile << "/" << outdir << "/" << filename_noext << "_" << n << ".bmp";
        SDL_SaveBMP(surf, filesystem::getRealWriteName(ofile.str().c_str()).c_str());
        printf("Wrote to: %s\n", filesystem::getRealWriteName(ofile.str().c_str()).c_str());
    }

    SDL_FreeSurface(surf);

    PHYSFS_deinit();
    printf("Exited cleanly\n");
    return 0;
}
