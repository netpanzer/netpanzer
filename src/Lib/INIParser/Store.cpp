/*
Copyright (C) 2004 Matthias Braun <matze@braunis.de>

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


#include "Store.hpp"

#include <ctype.h>
#include "Util/Log.hpp"

namespace INI
{

Store::Store()
{
}

Store::~Store()
{
    std::map<std::string, Section*>::iterator i;
    for(i = sections.begin(); i != sections.end(); ++i)
        delete i->second;
}

void
Store::load(std::istream& in)
{
    Section* currentsection = 0;
    
    while(!in.eof()) {
        std::string line;
        getline(in, line);

        const char *c = line.c_str();
        while(isspace(*c) && *c != 0) {
            c++;
        };

        // skip lines full with spaces
        if(*c == 0)
            continue;

        if(*c == '#') {
            // skipp comments
        } else if(*c == '[') { // section starts...
            ++c;
            std::string newsection;
            while(*c != 0 && *c != ']') {
                newsection += *c;
                ++c;
            }
            if(*c == 0) {
                LOGGER.warning("Malformed configline: %s", line.c_str());
            }
            currentsection = &getSection(newsection);
        } else { // a normal entry
            if(currentsection == 0) {
                LOGGER.warning("Key/Value outside section in configfile: %s",
                        line.c_str());
            }
            
            std::string key, value;

            while(*c != 0 && *c != '=') {
                key += *c;
                ++c;
            }
            if(*c != '=') {
                LOGGER.warning("Malformed configline: %s", line.c_str());
            }

            ++c;
            while(*c != 0) {
                if(*c != '\r') // win32 hack...
                    value += *c;
                ++c;
            }
            
            currentsection->setValue(key, value);
        }
    }
}

void
Store::save(std::ostream& out) const
{
    std::map<std::string, Section*>::const_iterator i;
    for(i = sections.begin(); i != sections.end(); ++i) {
        Section* section = i->second;
        out << "[" << section->getName() << "]\n";

        std::map<std::string, std::string>::iterator k;
        for(k = section->values.begin(); k != section->values.end(); ++k) {
            out << k->first << "=" << k->second << "\n";
        }
        out << "\n";
    }
}

Section&
Store::getSection(const std::string& name)
{
    std::map<std::string, Section*>::iterator i = sections.find(name);
    if(i != sections.end())
        return *(i->second);

    // create a new section
    Section* section = new Section(name);
    sections.insert(std::make_pair(name, section));

    return *section;
}

}
