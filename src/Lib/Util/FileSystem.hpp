/*
Copyright (C) 2003 Matthias Braun <matze@braunis.de>
                                                                                
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
/** This is a thin C++ wrapper around libphysfs */
#ifndef __LIB_FILESYSTEM_HPP__
#define __LIB_FILESYSTEM_HPP__

#include <stdexcept>
#include <SDL2/SDL.h>
#include <string>
#include <stdlib.h>
#include "physfs/physfs.h"

namespace filesystem
{

class File
{
public:
    ~File();

    bool eof();
    int64_t tell();
    void seek(uint64_t position);
    int64_t fileLength();
    // conveniance function, since I think this name is more c++ typic
    int64_t length()
    {
        return fileLength();
    }

    void setBuffer(uint64_t bufsize);
    void flush();

protected:
    File(PHYSFS_file* file);

    PHYSFS_file* file;
};

class FileSystem;

class ReadFile : public File
{
public:
    bool isEOF();	

    void read(void* buffer, size_t objsize, size_t objcount);
    size_t _read(void* buffer, size_t objsize, size_t objcount);

    Sint8 read8();

    Sint16 readSLE16();
    Uint16 readULE16();
    Sint16 readSBE16();
    Uint16 readUBE16();

    Sint32 readSLE32();
    Uint32 readULE32();
    Sint32 readSBE32();
    Uint32 readUBE32();

    int64_t readSLE64();
    uint64_t readULE64();
    int64_t readSBE64();
    uint64_t readUBE64();

    void readLine(std::string& buffer);

    // Returns the SDL_RWops structure which can be used in several SDL
    // commands. Note that you have to free this structure with SDL_FreeRWops.
    // (Most SDL commands also have a freesrc parameter in their calls which you
    // can simply set to 1)
    SDL_RWops* getSDLRWOps();

    /** for internal use only */
    ReadFile(PHYSFS_file* file);

private:
    static size_t RWOps_Read(SDL_RWops* context, void* ptr, size_t size, size_t maxnum);
    static Sint64 RWOps_Seek(SDL_RWops* context, Sint64 offset, int whence);
    static int RWOps_Close(SDL_RWops* context);
};

//---------------------------------------------------------------------------

class WriteFile : public File
{
public:
    void write(const void* buffer, size_t objsize, size_t objcount);

    void write8(Sint8 val);

    void writeSLE16(Sint16 val);
    void writeULE16(Uint16 val);
    void writeSBE16(Sint16 val);
    void writeUBE16(Uint16 val);

    void writeSLE32(Sint32 val);
    void writeULE32(Uint32 val);
    void writeSBE32(Sint32 val);
    void writeUBE32(Uint32 val);

    void writeSLE64(int64_t val);
    void writeULE64(uint64_t val);
    void writeSBE64(int64_t val);
    void writeUBE64(uint64_t val);

    /// writes the text in the buffer and an additional newline
    void writeLine(const std::string& line);

    /** for inernal use only */
    WriteFile(PHYSFS_file* file);
};

//---------------------------------------------------------------------------

/** This class is a c++ wrapper for the physfs library. See physfs
 * documentation for details about the functions here. Most function names are
 * exactly the same as in physfs
 */
void initialize(const char* argv0, const char* company,
        const char* applicationname);
void shutdown();

void addToSearchPath(const char* dir, bool append = true);
void removeFromSearchPath(const char* dir);

const char* getRealDir(const char* filename);
std::string getRealName(const char* filename);
std::string getRealWriteName(const char* filename);
/// remember to call freeLisT
char** enumerateFiles(const char* directory);
static inline char** enumerateFiles(const std::string& directory)
{ return enumerateFiles(directory.c_str()); }
void freeList(char** list);

ReadFile* openRead(const char* filename);
static inline ReadFile* openRead(const std::string& filename)
{ return openRead(filename.c_str()); }
WriteFile* openAppend(const char* filename);
static inline WriteFile* openAppend(const std::string& filename)
{ return openAppend(filename.c_str()); }
WriteFile* openWrite(const char* filename);
static inline WriteFile* openWrite(const std::string& filename)
{ return openWrite(filename.c_str()); }

void mkdir(const char* dirname);
static inline void mkdir(const std::string& dirname)
{ mkdir(dirname.c_str()); }
void remove(const char* filename);
static inline void remove(const std::string& filename)
{ remove(filename.c_str()); }

bool exists(const char* filename);
static inline bool exists(const std::string& filename)
{ return exists(filename.c_str()); }
bool isDirectory(const char* filename);
static inline bool isDirectory(const std::string& filename)
{ return isDirectory(filename.c_str()); }
bool isSymbolicLink(const char* filename);
static inline bool isSymbolicLink(const std::string& filename)
{ return isSymbolicLink(filename.c_str()); }
int64_t getLastModTime(const char* filename);
static inline int64_t getLastModTime(const std::string& filename)
{ return getLastModTime(filename.c_str()); }

//---------------------------------------------------------------------------

class FileReadException : public std::range_error
{
public:
    FileReadException(size_t objread, size_t objrequested,
	    const std::string& msg) throw();
    virtual ~FileReadException() throw();
    
    size_t getReadCount() const throw()
    { return objread; }
    size_t getRequestCount() const throw()
    { return objrequested; }
private:
    size_t objread, objrequested;
};

}

#endif
