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
#ifndef __FILESTREAM_HPP__
#define __FILESTREAM_HPP__

#include "FileSystem.hpp"
#include <streambuf>
#include <iostream>

/** This class implements a C++ streambuf object for physfs files.
 * So that you can use normal istream operations on them
 */
class IFileStreambuf : public std::streambuf
{
public:
    IFileStreambuf(filesystem::ReadFile* newfile)
        : file(newfile)
    {
        // start reading
        underflow();
    }

    ~IFileStreambuf()
    {
        delete file;
    }
    
protected:
    virtual int underflow()
    {
        if(file->isEOF())
            return traits_type::eof();
        
        size_t bytesread = file->_read(buf, 1, sizeof(buf));
        if(bytesread == 0)
            return traits_type::eof();
        setg(buf, buf, buf + bytesread);

        return buf[0];
    }

private:
    filesystem::ReadFile* file;
    char buf[1024];
};

class OFileStreambuf : public std::streambuf
{
public:
    OFileStreambuf(filesystem::WriteFile* newfile)
        : file(newfile)
    {
        setp(buf, buf+sizeof(buf));
    }

    ~OFileStreambuf()
    {
        sync();
        delete file;
    }

protected:
    virtual int overflow(int c)
    {
        if(pbase() == pptr())
            return 0;

        try {
            size_t size = pptr() - pbase();
            file->write(pbase(), size, 1);
            if(c != traits_type::eof())
                file->write8(c);
        } catch(std::exception& e) {
            return traits_type::eof();
        }

        setp(buf, buf+sizeof(buf));
        return 0;
    }

    virtual int sync()
    {
        return overflow(traits_type::eof());
    }

private:
    filesystem::WriteFile* file;
    char buf[1024];
};

class IFileStream : public std::istream
{
public:
    IFileStream(filesystem::ReadFile* file)
        : std::istream(new IFileStreambuf(file))
    { }

    IFileStream(const std::string& filename)
        : std::istream(new IFileStreambuf(filesystem::openRead(filename)))
    { }

    ~IFileStream()
    {
        delete rdbuf();
    }
};

class OFileStream : public std::ostream
{
public:
    OFileStream(filesystem::WriteFile* file)
        : std::ostream(new OFileStreambuf(file))
    { }

    OFileStream(const std::string& filename)
        : std::ostream(new OFileStreambuf(filesystem::openWrite(filename)))
    { }

    ~OFileStream()
    {
        delete rdbuf();
    }
};

#endif

