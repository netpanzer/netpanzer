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
#ifndef __LIB_LOG_HPP__
#define __LIB_LOG_HPP__

#include <stdarg.h>
#include <string>

namespace filesystem {
class WriteFile;
}

class Logger
{
public:
    static const int LEVEL_DEBUG;
    static const int LEVEL_INFO;
    static const int LEVEL_WARNING;

    Logger();
    ~Logger();

    void openLogFile(const char* filename);
    void closeLogFile();

    void setLogLevel(int logLevel) { m_logLevel = logLevel; }
    int getLogLevel() { return m_logLevel; }

    void debug(const char *fmt, ...)
        __attribute__((format (__printf__, 2, 3)));
    void info(const char *fmt, ...)
        __attribute__((format (__printf__, 2, 3)));
    void warning(const char *fmt, ...)
        __attribute__((format (__printf__, 2, 3)));

private:
    void log(int priority, const char *fmt, va_list ap);
    void cleanLogs();
    void openNext();

    std::string baselogname;
    std::string prefix;
    int used_size;
    int m_logLevel;
    filesystem::WriteFile* m_logfile;
};

extern Logger LOGGER;
#define LOG(x)         LOGGER.info x

#endif

