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
#ifndef __INI_STORE_HPP__
#define __INI_STORE_HPP__

#include <map>
#include <string>
#include <iostream>
#include "Section.hpp"

namespace INI
{

class Store
{
public:
    Store();
    ~Store();

    void load(std::istream& in);
    void save(std::ostream& out) const;

    Section& getSection(const std::string& name);
        
private:
    std::map<std::string, Section*> sections;
};

} // end of namespace INI

#endif
