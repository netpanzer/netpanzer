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


#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include "Exception.hpp"

Exception::Exception(const char* msg, ...) throw()
{
    va_list args;
    va_start(args, msg);

    message = new char[255];
    vsnprintf(message, 255, msg, args);

    va_end(args);
}

Exception::Exception(const Exception& other) throw()
    : std::exception(other)
{
    size_t len = strlen(other.message);
    message = new char[len+1];
    memcpy(message, other.message, len+1);
}

Exception::~Exception() throw()
{
    delete[] message;
}

const char* Exception::what() const throw()
{
    return message;
}

