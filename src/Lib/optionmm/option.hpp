//
// $Id: option.hpp 1220 2010-06-05 14:54:18Z kromxp $
//
//  optionmm::option
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
#ifndef OPTIONMM_option
#define OPTIONMM_option

#ifndef __VECTOR__
#include <vector>
#endif
#ifndef __STRING__
#include <string>
#endif
#ifndef __IOSTREAM__
#include <iostream>
#endif

/** @file   option.hh
    @author Christian Holm
    @date   Sat Dec 28 19:10:36 2002
    @brief  Command line option  */

namespace optionmm
{
//====================================================================
/** Base class for options. This is needed to use the @c std::vector
    container in the class command_line below. */
class basic_option
{
public:
    typedef std::vector<size_t> position_list;
protected:
    /// Short name of the option
    char _short_name;
    /// Long name of the option
    std::string _long_name;
    /// The help string
    std::string _help_string;
    /// Flag whether we got at least one argument from user
    bool _got_one;
    /// List of positions
    position_list _positions;
public:
    /** CTOR
    @param s The short name (possibly '\\0')
    @param l The long name (possibly "");
    @param h The help string. */
    basic_option(char s,
                 const std::string& l,
                 const std::string& h);
    /// DTOR
    virtual ~basic_option()
    {}
    /// Get the short name
    char short_name() const
    {
        return _short_name;
    }
    /// Get the long name
    const std::string& long_name() const
    {
        return _long_name;
    }
    /// Get the help string
    const std::string& help_string() const
    {
        return _help_string;
    }
    /// Test whether we need an argument
    virtual bool need_argument() const
    {
        return true;
    }
    /// Test if this option can have many values
    virtual bool many_values() const
    {
        return true;
    }
    /// Set one value
    virtual void push_arg(const char*, int pos) = 0;
    /// Toggle the option.
    virtual void push_arg(int pos) = 0;
    /// Get the position of the @a i'th argument
    virtual int position(size_t i=0) const;
    /// Print this option help line to a stream
    void print(int ll, std::ostream& o=std::cout) const;
    /** Process this as a long option.
    @param arg is the option with leading "--" 
    @param pos The position of the option on the command line.
    @return positive on success, 0 on if no match, negative on failure. */
    virtual int handle(const std::string& arg, int pos);
    /** Handle this option as a short.
    @param opt The option 
    @param arg The possible argument. 
    @param pos The position of the option on the command line.
    @return positive on success, 0 on if no match, negative on failure. */
    virtual int handle(char*& opt, char*& arg, int pos);
};

//====================================================================
/** @brief Turns a C string into a value of @a Type
    @param Type The type to convert into */
template <typename Type>
struct string_to_value
{
    /** Convert the C string to a value of @a Type
    @param arg the C string to convert. 
    @return @a arg as a value of @a Type. */
    inline Type operator()(const char* arg)
    {
        return Type(arg);
    }
};

//====================================================================
/** @brief Toggles the value of an arbitriary type.
    The default implementation does nothing to the argument.  This
    means that if an option type is declared as having only one
    value, then this function will overwrite the previous value of
    the option. 
    @param Type The type to toggle. */
template <typename Type>
struct toggle_value
{
    /** Toggles the value of a type.
    @param x a reference the value to toggle. */
    inline void operator()(Type& )
    { }
}
;

//====================================================================
/** Command line option class.
    The argument @c Type is the type of the value, @c argument
    specifies whether this option needs an argument, and 
    @c multivalue says whether the option can take multiple values. */
template <typename Type,
bool argument=true,
bool multivalue=true,
typename Convert=string_to_value<Type>,
typename Toggle=toggle_value<Type> >
class option : public basic_option
{
public:
    /// The value type
    typedef Type value_type;
    /// Type of the converter structure
    typedef Convert convert_type;
    /// Type of the toggler  structure
    typedef Toggle toggle_type;
    /// Type of value container
    typedef std::vector<value_type> value_list;
protected:
    /// A list of the values
    value_list _values;
public:
    /** Constructor.
    @param s The short name (possibly '\\0')
    @param l The long name (possibly "");
    @param h The help string. 
    @param d The default value. */
    option(char s,
           const std::string& l,
           const std::string& h,
           value_type d);
    /** Destructor. */
    virtual ~option()
    {}
    /** Whether this option need an argument */
    bool need_argument() const
    {
        return argument;
    }
    /** Whether this optin can take many values */
    bool many_values() const
    {
        return multivalue;
    }
    /** Number of values of this option */
    int size() const
    {
        return  _values.size();
    }
    /** Get value
    @param i The value number to get. 
    @return the @p i value, or if that is out of bounds, the first
    value. */
    value_type value(size_t i=0) const;
    /** Get all values */
    const value_list& values() const
    {
        return _values;
    }
    /** Add value.
    If meny_values() tests false, the first value is changed,
    otherwise the option gets a new value. 
    @param v The value to add.  
    @param pos The position on the command line the option was
    given at.
    @return The value addded. */
    void push(const value_type& v, int pos);
    /** Add a value from a string.  This is used by the command_line
    class to set the values of the options, in case need_argument 
    tests true. 
    @param pos The position on the command line the option was
    given at.
    @param arg The value added. */
    void push_arg(const char* arg, int pos);
    /** Add a value.  This is used by the command_line
    class to set the values of the options, in case need_argument 
    tests false. 
    @param pos The position on the command line the option was
    given at. */
    void push_arg(int pos);
};
//____________________________________________________________________
template<typename Type, bool arg, bool multi,
typename Convert, typename Toggle>
inline option<Type,arg,multi,Convert,Toggle>::option(char s,
        const std::string& l,
        const std::string& h,
        value_type d)
        : basic_option(s, l, h)
{
    _values.push_back(d);
}
//____________________________________________________________________
template<typename Type, bool arg, bool multi,
typename Convert, typename Toggle>
inline Type option<Type,arg,multi,Convert,Toggle>::value(size_t i) const
{
    if (!many_values() || i >= _values.size())
        return _values[0];
    return _values[i];
}

//____________________________________________________________________
template<typename Type, bool arg, bool multi,
typename Convert, typename Toggle>
inline void option<Type,arg,multi,Convert,Toggle>::push(const value_type& v,
        int pos)
{
    if (!many_values() || (_values.size() == 1 && !_got_one) ) {
        _got_one = true;
        _values[0] = v;
        if (_positions.size() <= 0) _positions.push_back(pos);
        _positions[0] = pos;
    } else {
        _values.push_back(v);
        _positions.push_back(pos);
    }
}

//____________________________________________________________________
template<typename Type, bool arg, bool multi,
typename Convert, typename Toggle>
inline void option<Type,arg,multi,Convert,Toggle>::push_arg(const char* a,
        int pos)
{
    value_type v = string_to_value<value_type>()(a);
    push(v, pos);
}

//____________________________________________________________________
template<typename Type, bool arg, bool multi,
typename Convert, typename Toggle>
inline void option<Type,arg,multi,Convert,Toggle>::push_arg(int pos)
{
    if (many_values())
        push(_values[0], pos);
    else {
        // value_type& v = _values[0];
        toggle_value<value_type>()(_values[0]);
        if (_positions.size() <= 0) _positions.push_back(pos);
        _positions[0] = pos;
    }
}
}

#endif
//____________________________________________________________________
//
// EOF
//

