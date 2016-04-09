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
#ifndef __INI_SECTION_HPP__
#define __INI_SECTION_HPP__

#include <map>
#include <string>

namespace INI
{

/**
 * This class represents a section in an ini file. It contains a key/value map
 * for the settings
 */
    
    typedef std::map<std::string, std::string> valuesMap;
    typedef valuesMap::const_iterator valuesIterator;
    
class Section
{
public:
    ~Section();

    const std::string& getName() const;
    
    const std::string& getValue(const std::string& key) const;
    void setValue(const std::string& key, const std::string& value);

    int getIntValue(const std::string& key) const;
    float getFloatValue(const std::string& key) const;

    void setIntValue(const std::string& key, int value);
    void setFloatValue(const std::string& key, float value);

    valuesIterator getValuesBegin() const
    {
        return values.begin();
    }
    
    valuesIterator getValuesEnd() const
    {
        return values.end();
    }
    
private:
    friend class Store;
    Section(const std::string& name);
    
    std::string name;
    valuesMap values;
};

} // end of namespace INI

#endif
