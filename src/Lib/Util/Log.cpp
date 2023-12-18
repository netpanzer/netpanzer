/*
Copyright (C) 2003 Matthias Braun <matze@braunis.de>,
Ivo Danihelka <ivo@danihelka.net>
                                                                                
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


#include <stdarg.h>
#include <errno.h>
#include <string.h>
#include <string>
#include <sstream>
#include <vector>
#include <algorithm>
#include <exception>
#include <iomanip>
#include <time.h>

#include "Exception.hpp"
#include "FileSystem.hpp"
#include "Log.hpp"
#include <SDL2/SDL.h>

#define MAX_LOG_FILES 25
#define MAX_LOGFILE_SIZE 10000000

Logger LOGGER;

// like syslog levels
const int Logger::LEVEL_DEBUG = 7;
const int Logger::LEVEL_INFO = 6;
const int Logger::LEVEL_WARNING = 4;
//-----------------------------------------------------------------
/**
 * Default log level is INFO.
 */
Logger::Logger()
{
    m_logLevel = LEVEL_WARNING;
    m_logfile = 0;
    used_size = 0;
}
//-----------------------------------------------------------------
Logger::~Logger()
{
    delete m_logfile;
}
//-----------------------------------------------------------------
void
Logger::openLogFile(const char* filename)
{
    if ( filename && SDL_strlen(filename) )
        prefix = filename;
    else
        prefix = "netpanzer";
        
    openNext();
}
//-----------------------------------------------------------------
void
Logger::cleanLogs()
{
    info("Cleaning old log files");
    char** list = filesystem::enumerateFiles("/");
    
    std::vector<std::string> filenames;
    for(char** file = list; *file != 0; file++) {
        std::string name = *file;
        if( !name.compare(0, prefix.length(), prefix)
            && ! name.compare(name.length()-4, 4, ".log") ) {
            filenames.push_back(name);
        }
    }

    filesystem::freeList(list);

    int todelete = filenames.size() - MAX_LOG_FILES;
    if ( todelete > 0 ) {
        std::sort(filenames.begin(), filenames.end());
        for (int n=0; n<todelete; n++) {
            try {
                info("Deleting '%s'", filenames.at(n).c_str());
                filesystem::remove(filenames.at(n).c_str());
            } catch (std::exception &e) {
                warning("Some error happened cleaning logs '%s'", e.what());
            }
        }
    }
}
//-----------------------------------------------------------------
void
Logger::closeLogFile()
{
    info("Closing logfile.");
    delete m_logfile;
    m_logfile = 0;
}
//-----------------------------------------------------------------
void
Logger::openNext()
{
    std::string filename=prefix;
    
    char buf[256];
    time_t curtime = time(0);
    struct tm* loctime = localtime(&curtime);
    int timelen = strftime(buf, sizeof(buf), "-%Y%m%d_%H%M%S", loctime);

    if ( timelen )
        filename += buf;
        
    filename += ".log";

    if ( m_logfile ) {
        info("Closing log file for switch");
        delete m_logfile;
        m_logfile=0;
    }
    
    try {
        m_logfile = filesystem::openWrite(filename.c_str());

        used_size = 0;
        info("Log file open");
        cleanLogs();

    } catch(std::exception& e) {
        fprintf(stderr, "cannot open log file '%s': %s\n", filename.c_str(), e.what());
        m_logfile = 0;
    }    

}
//-----------------------------------------------------------------
void
Logger::log(int priority, const char *fmt, va_list ap)
{
    char buf[2048];
    time_t curtime = time(nullptr);
    struct tm* loctime = localtime(&curtime);
    int timelen = strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S ", loctime);
    
    vsnprintf(buf+timelen, sizeof(buf)-timelen-1, fmt, ap);
    strcat(buf, "\n");
    
    if (m_logLevel >= priority) {
        fprintf(stderr, "%s", buf);
    }
    
    if (m_logfile != 0) {
	try {
	    int blen = strlen(buf);
	    m_logfile->write(buf, blen, 1);
        m_logfile->flush();
        used_size += blen;
        if ( used_size >= MAX_LOGFILE_SIZE ) {
            used_size=0;
            openNext();
        }
	} catch(std::exception& e) {
            fprintf(stderr, "Error while writing logfile: %s", e.what());
            m_logfile = 0;
        }
    }
}
//-----------------------------------------------------------------
void
Logger::debug(const char *fmt, ...)
{
    va_list ap;

    std::string buffer("DEBUG: ");
    buffer.append(fmt);
    va_start(ap, fmt);
    log(Logger::LEVEL_DEBUG, buffer.c_str(), ap);
    va_end(ap);
}
//-----------------------------------------------------------------
void
Logger::info(const char *fmt, ...)
{
    va_list ap;

    va_start(ap, fmt);
    log(Logger::LEVEL_INFO, fmt, ap);
    va_end(ap);
}
//-----------------------------------------------------------------
void
Logger::warning(const char *fmt, ...)
{
    va_list ap;

    va_start(ap, fmt);
    log(Logger::LEVEL_WARNING, fmt, ap);
    va_end(ap);
}

