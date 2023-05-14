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


#include <memory>
#include "Util/FileSystem.hpp"
#include "Util/Exception.hpp"
#include "TipOfDay.hpp"

TipOfDay::TipOfDay(const std::string& filename)
{
    try {
        std::unique_ptr<filesystem::ReadFile> file(
                filesystem::openRead(filename));

        std::string currenttip;
        while(!file->eof()) {
            std::string line;
            file->readLine(line);
            if(line == "%%") {
                tips.push_back(currenttip);
                currenttip = "";
            } else {
                currenttip += " ";
                currenttip += line;
            }
        }
        if(currenttip != "")
            tips.push_back(currenttip);
    } catch(std::exception& e) {
        throw Exception("Couldn't open tipofdayfile '%s': %s",
                filename.c_str(), e.what());
    }
}

TipOfDay::~TipOfDay()
{
}

size_t TipOfDay::getTipCount() const
{
    return tips.size();
}

const std::string& TipOfDay::getTip(size_t number)
{
    return tips.at(number);
}

