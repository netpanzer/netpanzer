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
#ifndef __UTIL_STRINGTOKENIZER_HPP__
#define __UTIL_STRINGTOKENIZER_HPP__

#include <string>

/** a class that splits a string into multiple tokens that are separated by a
 * \ delimiter character
 */
class StringTokenizer
{
public:
    StringTokenizer(const std::string& newbuffer, char newtoken)
        : buffer(newbuffer), token(newtoken), pos(0)
    {
    }

    std::string getNextToken()
    {
        if(pos >= buffer.length())
            return "";

        // skip \ chars
        while(pos < buffer.length() && buffer[pos] == token)
            ++pos;
        std::string::size_type start = pos;
        // search next \ char
        while(pos < buffer.length() && buffer[pos] != token)
            ++pos;
        
        return std::string(buffer, start, pos-start);
    }

private:
    const std::string& buffer;
    char token;
    std::string::size_type pos;
};

#endif

