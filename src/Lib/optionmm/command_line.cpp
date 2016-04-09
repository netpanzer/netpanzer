//
// $Id: command_line.cpp 1182 2010-01-19 16:08:17Z kromxp $
//
//  option::command_line
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
/** @file   command_line.cc
    @author Christian Holm
    @date   Sat Dec 28 19:02:59 2002
    @brief  Command line parser */


#ifndef OPTIONMM_command_line
#include "command_line.hpp"
#endif
#ifndef __CSTDLIB__
#include <cstdlib>
#endif
#ifndef __IOSTREAM__
#include <iostream>
#endif

//____________________________________________________________________
optionmm::command_line::command_line(const std::string title,
                                     const std::string& version,
                                     const std::string& copy,
                                     const std::string& usage,
                                     int& argc, char** argv, bool fail)
        : _program_name(argv[0]),
        _argc(argc),
        _argv(argv),
        _title(title),
        _version(version),
        _copyright(copy),
        _usage(usage),
        _help_option('h',"help","Show this help",false),
        _version_option('\0',"version", "Show version information",false),
        _fail_on_unknown(fail)
{
    std::string::size_type slash = _program_name.find_last_of('/');
    _program_name.erase(0, slash+1);
    add(&_help_option);
    add(&_version_option);
}

//____________________________________________________________________
bool
optionmm::command_line::help(std::ostream& out)
{
    if (!_help_option.value())
        return false;
    _version_option.push(true, _help_option.position());
    version();
    out << std::endl << "Usage: " << _program_name;
    if (_usage.empty()) out << " [OPTIONS]";
    else                out << " " << _usage;
    out << std::endl << std::endl;

    /// Figure out the longest long_name
    size_t ll = 0;
    for (option_list::iterator o = _options.begin(); o != _options.end(); ++o)
        if ((*o)->long_name().length() >= ll) ll = (*o)->long_name().length();
    for (option_list::iterator p = _options.begin(); p != _options.end(); ++p) {
        out << "    ";
        (*p)->print(ll, out);
        out << std::endl;
    }
    out << std::endl;
    return true;
}

//____________________________________________________________________
bool
optionmm::command_line::version(std::ostream& o)
{
    if (!_version_option.value()) return false;
    o << _title << " version " << _version << std::endl
    << _copyright << std::endl;
    return true;
}

//____________________________________________________________________
bool
optionmm::command_line::process()
{
    for (int i = 1; i < _argc; i++) {
        if (_argv[i] && _argv[i][0] == '-') {
            // Got an option
            bool ret;
            if (_argv[i][1] != '-') ret = handle_short(i);
            else ret = handle_long(i);
            if (!ret) return false;
        }
    }
    cleanup();
    return true;
}

//____________________________________________________________________
void
optionmm::command_line::cleanup()
{
    int n = 1;
    for (int i = 1; i < _argc; i++) {
        int j = i;
        while (!_argv[j] && j < _argc-1) {
            j++;
        }
        if (i != j) {
            _argv[i] = _argv[j];
            _argv[j] = 0;
        }
        if (_argv[i]) n++;
    }
    _argc = n;
}


//____________________________________________________________________
bool
optionmm::command_line::handle_short(int& i)
{
    int  j     = 1;
    int  ret   = 0;
    bool gotit = false;
    while (_argv[i] && _argv[i][j] && _argv[i][j] != '-') {
        option_list::iterator o;
        for (o = _options.begin(); o < _options.end(); o++) {
            char* arg = &(_argv[i][j]);
            if ((ret = (*o)->handle(arg, _argv[i+1], i)) > 0) {
                int k = j;
                // Eat away this argument.
                while (_argv[i][k] != '\0') {
                    // I really wanted to have `_argv[i][++k]' on the right hand
                    // side of this assignment, but GCC 3.0 optimises that
                    // incrementation away when -O1 or higher is specified, so
                    // we pin it out a bit more.
                    _argv[i][k] = _argv[i][k+1];
                    k++;
                }
                j--;
                if (!_argv[i+1])
                    gotit = true;
                break;
            }
            if (ret < 0) return false;
        }
        if (gotit) break;
        if (o == _options.end() && _fail_on_unknown) {
            std::cerr << "Option `-" << _argv[i][j] << "' unknown, try `"
            << _program_name << " --help'" << std::endl;
            return false;
        }
        j++;
    }
    if (_argv[i][0] == '-' && _argv[i][1] == '\0') _argv[i] = 0;
    if (gotit) i++;
    return true;
}

//____________________________________________________________________
bool
optionmm::command_line::handle_long(int& i)
{
    int ret = 0;
    std::string n(_argv[i]);
    option_list::iterator o;
    for (o = _options.begin(); o < _options.end(); o++) {
        if ((ret = (*o)->handle(n, i)) > 0) {
            _argv[i] = 0;
            break;
        } else if (ret < 0) return false;
    }
    if (o == _options.end() && _fail_on_unknown) {
        std::cerr << "Option `" << n << "' unknown, try `"
        << _program_name << " --help'" << std::endl;
        return false;
    }
    return true;
}

//____________________________________________________________________
int
optionmm::string_to_value<int>::operator()(const char* arg)
{
    return strtol(arg, NULL, 0);
}

//____________________________________________________________________
long
optionmm::string_to_value<long>::operator()(const char* arg)
{
    return strtol(arg, NULL, 0);
}

//____________________________________________________________________
float
optionmm::string_to_value<float>::operator()(const char* arg)
{
    return strtod(arg, NULL);
}

//____________________________________________________________________
double
optionmm::string_to_value<double>::operator()(const char* arg)
{
    return strtod(arg, NULL);
}

//____________________________________________________________________
bool
optionmm::string_to_value<bool>::operator()(const char* arg)
{
    if (arg[0] == 'f' || arg[0] == '0') return false;
    return true;
}



//____________________________________________________________________
//
// EOF
//
