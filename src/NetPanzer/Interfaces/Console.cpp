/*
Copyright (C) 2004 by Matthias Braun <matze@braunis.de>

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

#include <sstream>
#include <time.h>

#include "Console.hpp"
#include "Interfaces/GameConfig.hpp"
#include "Util/FileSystem.hpp"
#include "Util/FileStream.hpp"
#include "Util/Log.hpp"

static inline void makeTimeStamp(char* buffer, size_t bufsize) {
    time_t curtime = time(0);
    struct tm* loctime = localtime(&curtime);
    strftime(buffer, bufsize, "<%Y-%m-%d %H:%M:%S>", loctime);
}
    

/** A streambuf implementation that wraps around an existing streambuf and
 * outputs the current timestampe after each newline
 */
class FileStampStreamBuf : public std::streambuf
{
public:
    FileStampStreamBuf(std::ostream* file)
        : needstamp(true)
    {
        stream = &std::cout;
        this->file = file;
        setp(buf, buf+sizeof(buf));
    }

    ~FileStampStreamBuf()
    {
        sync();
        if(file)
            delete file;
    }

    virtual int overflow(int c)
    {
        if(pbase() == pptr())
            return 0;
       
        if(needstamp)
            outputdate();
        needstamp = false;

        // check for \n in the buffer
        char* chunkstart = pbase();
        for(char* p = pbase(); p != pptr(); ++p) {
            if(*p == '\n') {
                // output data in buffer so far
                stream->write(chunkstart, p-chunkstart+1);
                if(file)
                    file->write(chunkstart, p-chunkstart+1);
                redisplay_prompt();
                if(p < pptr()-1)
                    outputdate();
                else
                    needstamp = true;
                chunkstart = p+1;
            }
        }
        // output the rest
        stream->write(chunkstart, pptr() - chunkstart);
        if(file)
            file->write(chunkstart, pptr() - chunkstart);

        if(c != traits_type::eof()) {
            *stream << (char) c;
            if(file)
                *file << (char) c;
            if(c == '\n') {
                needstamp = true;
                redisplay_prompt();
            }
        }
        setp(buf, buf+sizeof(buf));
        return 0;
    }

    virtual int sync()
    {
        overflow(traits_type::eof());
        stream->flush();
        if(file)
            file->flush();
        return 0;
    }

private:
    void outputdate()
    {
        char timestamp[64];
        time_t curtime = time(0);
        struct tm* loctime = localtime(&curtime);
        strftime(timestamp, sizeof(timestamp), "<%Y-%m-%d %H:%M:%S>", loctime);
        *stream << "\r" << timestamp;
        if(file)
            *file << timestamp;
    }

    void redisplay_prompt()
    {
    }

    std::ostream* stream;
    std::ostream* file;

    bool needstamp;
    char buf[1024];
};

class OFileStampStream : public std::ostream
{
public:
    OFileStampStream(std::ostream* file)
        : std::ostream(new FileStampStreamBuf(file))
    { }

    ~OFileStampStream()
    {
        delete rdbuf();
    }
};

//---------------------------------------------------------------------------

std::ostream* Console::server = 0;

void
Console::initialize()
{
    server = new OFileStampStream(0);
}

void
Console::shutdown()
{
    delete server;
}

void
Console::mapSwitch(const std::string& mapname)
{
    if ( GameConfig::server_logging )
    {
        if(!filesystem::exists("logs")) {
            filesystem::mkdir("logs");
        }
        if(!filesystem::isDirectory("logs")) {
            LOGGER.warning("logs is not a directory?!?");
            return;
        }

        char timestamp[64];
        time_t curtime = time(0);
        struct tm* loctime = localtime(&curtime);
        strftime(timestamp, sizeof(timestamp), "%Y-%m-%d_%H-%M-%S", loctime);
        
        std::stringstream filenamestr;
        filenamestr << "logs/" << timestamp << '_' << mapname << ".log";
        std::string filename = filenamestr.str();
        
        try {
            OFileStream* stream = new OFileStream(filenamestr.str());
            delete server;
            server = new OFileStampStream(stream);
        } catch(std::exception& e) {
            LOGGER.warning("Couldn't open logfile '%s': %s",
                filename.c_str(), e.what());
            return;
        }
    }
}
