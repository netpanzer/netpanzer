//
// $Id: option.cpp 1182 2010-01-19 16:08:17Z kromxp $
//
//  option::option
//  Copyright (C) 2002 Christian Holm Christensen <cholm@nbi.dk>
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public License
//  as published by the Free Software Foundation; either version 2.1
//  of the License, or (at your option) any later version.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free
//  Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
//  02111-1307 USA
//
/** @file   option.cc
    @author Christian Holm
    @date   Sat Dec 28 19:10:36 2002
    @brief  Command line option  */


#ifndef OPTIONMM_option
#include "option.hpp"
#endif
#ifndef __IOSTREAM__
#include <iostream>
#endif
#ifndef __IOMANIP__
#include <iomanip>
#endif
#if defined(__GNUC__) && __GNUC__ <= 2
#define CMP_ARG(s,x) x, size_t(s), x.size()
#else
#define CMP_ARG(s,x) size_t(s), x.size(), x
#endif

//____________________________________________________________________
optionmm::basic_option::basic_option(char s,
                                     const std::string& l,
                                     const std::string& h)
        : _short_name(s), _long_name(l), _help_string(h), _got_one(false)
{}

//______________________________________________________________________
int
optionmm::basic_option::position(size_t i) const
{
    if (i >= _positions.size()) return 0;
    if (!many_values()) return _positions[0];
    return _positions[i];
}

//____________________________________________________________________
void
optionmm::basic_option::print(int ll, std::ostream& o) const
{
    if (ll <= 0) ll=16;
    if (_short_name != '\0') o << "-" << _short_name <<", ";
    else                     o << "    ";
    if (!_long_name.empty()) {
        o << "--" << _long_name;
        if (need_argument()) o << "=VALUE";
        else                 o << "      ";
        for (int i = _long_name.size(); i < ll; i++) o << " ";
    } else {
        o << std::setw(ll+2) << " ";
        if (need_argument()) o << " VALUE";
        else                 o << "      ";
    }
    o << "\t" << _help_string;
    if (many_values()) o << " (*)";
}

//____________________________________________________________________
int
optionmm::basic_option::handle(const std::string& arg, int pos)
{
    if (_long_name.empty() || arg.compare(CMP_ARG(2,_long_name))) return 0;
    if (need_argument()) {
        std::string::size_type eq = arg.find_last_of('=');
        if (eq == std::string::npos) {
            std::cerr << "Option --" << _long_name << " need an argument"
            << std::endl;
            return -1;
        }
        std::string value(arg);
        value.erase(0, eq+1);
        push_arg(value.c_str(), pos);
    } else
        push_arg(pos);
    return 1;
}

//____________________________________________________________________
int
optionmm::basic_option::handle(char*& opt, char*& arg, int pos)
{
    if (_short_name == '\0' || opt[0] != _short_name) return 0;
    if (need_argument()) {
        if (opt[1] != '\0') {
            push_arg(&(opt[1]), pos);
            opt[1] = '\0';
        } else if (arg) {
            push_arg(arg, pos);
            // Flag argument as used.
            arg = 0;
        } else {
            std::cerr << "Option -" << _short_name << " need an argument"
            << std::endl;
            return -1;
        }
    } else
        push_arg(pos);
    return 1;
}

//____________________________________________________________________
//
// EOF
//
