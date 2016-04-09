//
// $Id: command_line.hpp 967 2007-04-16 13:36:42Z kromxp $
//
//  optionmm::command_line
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
#ifndef OPTIONMM_command_line
#define OPTIONMM_command_line
#ifndef OPTIONMM_optionmm
#include <optionmm/option.hpp>
#endif
#ifndef __VECTOR__
#include <vector>
#endif
#ifndef __STRING__
#include <string>
#endif
#ifndef __IOSTREAM__
#include <iostream>
#endif

/** @file   command_line.hh
    @author Christian Holm
    @date   Sat Dec 28 19:02:59 2002
    @brief  Command line parser */

namespace optionmm
{
/** @brief Command line parser.
    The command line option manager is straight forward to use. 
    First, one declares on instance of the manager, parsing it 
    the proper arguments 
    
    @dontinclude demo.cc 
    @skip int
    @until ""

    Then, one creates option objects and add them to the manager. 
    @until if 

    After that, the manager should process the command line. 
    @until if 

    If the application should exit if the help option was given,
    then it may do so, and similar for the version option. 
    @until for 

    After processing, the option objects hold the value(s) of the
    command line options given. 
    @until return
 */
class command_line
{
public:
    /// The container of options.
    typedef std::vector<basic_option*> option_list;
private:
    /// List of options
    option_list _options;
    /// The program name (sans directory part)
    std::string _program_name;
    /// The number of arguments cached from command line
    int& _argc;
    /// The command line arguments.
    char** _argv;
    /// The title of the application
    std::string _title;
    /// The version number of the application
    std::string _version;
    /// The copyright of the application
    std::string _copyright;
    /// The copyright of the application
    std::string _usage;
    /// The help option.
    option<bool,false,false> _help_option;
    /// The version information option.
    option<bool,false,false> _version_option;
    /// Wether to fail on unknown options
    bool _fail_on_unknown;
    /** Handle the i'th short options
    @return true on success, false oterwise. */
    bool handle_short(int& i);
    /** Handle the i'th long option
    @return true on success, false oterwise. */
    bool handle_long(int& i);
    /// Clean up command line
    void cleanup();
public:
    /** Constructor.
    @param argc reference to the number of command line options
    from @c main
    @param argv the command line options from main 
    @param title the title of the application.
    @param version the version of the application 
    @param copy The copyright of the application.
    @param usage What to print as syntax (may be empty)
    @param fail Whether to fail on unknown options */
    command_line(const std::string title,
                 const std::string& version,
                 const std::string& copy,
                 const std::string& usage,
                 int& argc, char** argv, bool fail=true);
    /** Add an option to the command line
    @param option The option object to add to the manager. */
    void add(basic_option* option)
    {
        _options.push_back(option);
    }
    /** Print a help message
    If the manager saw the @c --help option, then print the
    help message to stream @a o, and return true. 
    One can use the return value to jump out of the main program
    if the @c --help option was given, like 
    @code 
    int main(int argc, char** argv) { 
     using namespace optionmm; 
     option_manager om("foo", "1.0", "bar", "", argc, argv);
     ...
     if (!om.process()) return 1;
     if (om.help()) return 0;
     ...
          }
        @endcode 
    @return true if the help option was given. */
    bool help(std::ostream& o=std::cout);
    /** Print version information.
    If the manager saw the @c --version option, then print the
    version information to stream @a o, and return true. 
    @param o The stream to write information to. 
    @return true if version option was given. */
    bool version(std::ostream& o=std::cout);
    /** Process the command line
    @return true on success, false oterwise. */
    bool process();
    /** Get the application name */
    const std::string& program_name() const
    {
        return _program_name;
    }
    /** Get the application title */
    const std::string& title() const
    {
        return _title;
    }
    /** Get the application version */
    const std::string& version() const
    {
        return _title;
    }
    /** Get the application copyright */
    const std::string& copyright() const
    {
        return _copyright;
    }
};

/// Explicit specialisation of helper function
template <>
struct string_to_value<int>
{
    int operator()(const char* arg);
};

/// Explicit specialisation of helper function
template <>
struct string_to_value<long>
{
    long operator()(const char* arg);
};

/// Explicit specialisation of helper function
template <>
struct string_to_value<float>
{
    float operator()(const char* arg);
};

/// Explicit specialisation of helper function
template <>
struct string_to_value<double>
{
    double operator()(const char* arg);
};

/// Explicit specialisation of helper function
template <>
struct string_to_value<bool>
{
    bool operator()(const char* arg);
};

/// Explicit specialisation  of helper function
template<>
struct toggle_value<bool>
{
    inline void operator()(bool& x)
    {
        x = !x;
    }
#if defined(__GNUC__) && __GNUC__ >= 2
    inline void operator()(std::_Bit_reference x)
    {
        x.flip();
    }
#endif
};

/// Explit specialisatation as a typedef
typedef option<int,true,true>          int_option;

/// Explit specialisatation as a typedef
typedef option<float,true,true>        float_option;

/// Explit specialisatation as a typedef
typedef option<bool,false,false>       bool_option;

/// Explit specialisatation as a typedef
typedef option<std::string,true,true>  string_option;
}

#endif
//____________________________________________________________________
//
// EOF
//
