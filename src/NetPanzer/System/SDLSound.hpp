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
#ifndef __UILIB_SDLSOUND_HPP__
#define __UILIB_SDLSOUND_HPP__

#include <string>
#include <vector>
#include <map>
#include <SDL2/SDL_mixer.h>
#include "System/Sound.hpp"

class SoundData;

typedef std::multimap<std::string,SoundData*> chunks_t;
typedef std::vector<std::string> musics_t;

class SDLSound : public Sound
{
public:
    SDLSound();
    virtual ~SDLSound();

    virtual void playSound(const char* name);
    virtual void playAmbientSound(const char* name, long distance);
    virtual int playSoundRepeatedly(const char* name);
    virtual void stopChannel(int channel);
    virtual void setSoundVolume(unsigned int volume);

    virtual void playMusic(const char* directory);
    virtual void stopMusic();
    virtual void setMusicVolume(unsigned int volume);

private:
    chunks_t m_chunks;

    SoundData* findChunk(const char *name);
    int getSoundVolume(long distance);
    void loadSound(const char* directory);
    std::string getIdName(const char* filename);

    static void nextSong();
    static musics_t musicfiles;
    static musics_t::iterator currentsong;
};

#endif

