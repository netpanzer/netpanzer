/*
Copyright (C) 2003 Matthias Braun <matze@braunis.de>
                                                                                
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
#ifndef __UILIB_DUMMYSOUND_HPP__
#define __UILIB_DUMMYSOUND_HPP__

#include "Sound.hpp"

/** This is an interface to sound functions in the game */
class DummySound : public Sound
{
public:
    DummySound();
    virtual ~DummySound();

    virtual void playSound(const char* name);
    virtual void playAmbientSound(const char *name, long distance);
    virtual int playSoundRepeatedly(const char* )
    {
        return -1;
    }
    virtual void stopChannel(int )
    {}
    virtual void setSoundVolume(unsigned int );

    virtual void playMusic(const char* directory);
    virtual void stopMusic();
    virtual void setMusicVolume(unsigned int volume);
};

#endif

