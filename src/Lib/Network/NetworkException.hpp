/*
Copyright (C) 2007 by Aaron Perez <aaronps@gmail.com>

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

#ifndef _NETWORKEXCEPTION_HPP
#define _NETWORKEXCEPTION_HPP

#include <exception>
#include <string>

class NetworkException : public std::exception
{
protected:
    std::string errormsg;
public:
    NetworkException(const std::string& s) : errormsg(s) {};
    virtual ~NetworkException() {};
    virtual const char * what() const throw() { return errormsg.c_str(); };
};

#endif
