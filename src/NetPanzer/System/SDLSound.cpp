/*
   Copyright (C) 2003 Matthias Braun <matze@braunis.de>,
   Ivo Danihelka <ivo@danihelka.net>
 
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


#include <sys/types.h>
#include <dirent.h>
#include <errno.h>
#include <algorithm>

#include <SDL2/SDL.h>
#define USE_RWOPS // we want Mix_LOadMUS_RW
#include <SDL2/SDL_mixer.h>
#include "Util/Log.hpp"
#include "Util/Exception.hpp"
#include "Util/FileSystem.hpp"
#include "Util/NTimer.hpp"
#include "SDLSound.hpp"
#include "Interfaces/GameConfig.hpp"

#if (SDL_MIXER_MAJOR_VERSION > 1) || (SDL_MIXER_MINOR_VERSION > 2) || \
    ((SDL_MIXER_MINOR_VERSION == 2) && (SDL_MIXER_PATCHLEVEL >= 6))
#define HAS_LOADMUS_RW
#endif

#define SOUND_REPLAY_PROTECTION_TIME 50

class SoundData
{
private:
    Mix_Chunk* chunk;
public:
    NTimer last_played;

    SoundData() : chunk(0), last_played(SOUND_REPLAY_PROTECTION_TIME) {}
    SoundData(Mix_Chunk* c) : chunk(c), last_played(SOUND_REPLAY_PROTECTION_TIME) {}
    ~SoundData()
    {
        if ( chunk )
        {
            Mix_FreeChunk(chunk);
            chunk = 0;
        }
    }

    Mix_Chunk* getData() const { return chunk; }
};


musics_t SDLSound::musicfiles;
musics_t::iterator SDLSound::currentsong;

//-----------------------------------------------------------------
SDLSound::SDLSound()
        : Sound(), m_chunks()
{
    if(SDL_InitSubSystem(SDL_INIT_AUDIO) < 0)
        throw Exception("SDL_Init audio error: %s", SDL_GetError());

    if(Mix_OpenAudio(22050, MIX_DEFAULT_FORMAT, 2, 1024) < 0)
        throw Exception("Couldn't open audio device: %s", Mix_GetError());

    loadSound("sound/");
    Mix_AllocateChannels(12);
}
//-----------------------------------------------------------------
SDLSound::~SDLSound()
{
    stopMusic();
    Mix_CloseAudio();
    for (chunks_t::iterator i = m_chunks.begin(); i != m_chunks.end(); i++)
    {
        delete i->second;
    }

    SDL_QuitSubSystem(SDL_INIT_AUDIO);
}
//-----------------------------------------------------------------
/**
 * Find a chunk for this name.
 * @param name sound name
 * @return the chunk or NULL
 */
SoundData *SDLSound::findChunk(const char *name)
{
    chunks_t::size_type count = m_chunks.count(name);
    if (count == 0)
    {
        LOG (("Silent sound '%s'", name));
        return 0;
    }

    chunks_t::iterator it = m_chunks.find(name);
    for (int i = rand() % count; i > 0; i--)
    {
        it++;
    }
    
    return it->second;
}
//-----------------------------------------------------------------
/**
 * Play sound once.
 * @param name sound name
 */
void SDLSound::playSound(const char* name)
{
    SoundData *sdata = findChunk(name);
    if (sdata)
    {
        if ( sdata->last_played.isTimeOut() )
        {
            if (Mix_PlayChannel(-1, sdata->getData(), 0) == -1)
            {
                //LOG (("Couldn't play sound '%s': %s", name, Mix_GetError()));
            }
            sdata->last_played.reset();
        }
        else
        {
//            LOGGER.debug("Skipped sound '%s' due to timeout", name);
        }
        
    }
}
//-----------------------------------------------------------------
/**
 * Play sound once.
 * @param name sound name
 * @param distance mag2 distance
 */
void SDLSound::playAmbientSound(const char* name, long distance)
{
    SoundData *sdata = findChunk(name);
    if (sdata)
    {
        if ( sdata->last_played.isTimeOut() )
        {
            int oldVolume = Mix_VolumeChunk(sdata->getData(), getSoundVolume(distance));
            if (Mix_PlayChannel(-1, sdata->getData(), 0) == -1)
            {
                //LOG (("Couldn't play sound '%s': %s", name, Mix_GetError()));
            }
            Mix_VolumeChunk(sdata->getData(), oldVolume);
            sdata->last_played.reset();
        }
        else
        {
//            LOGGER.debug("Skipped ambient sound '%s' due to timeout", name);
        }
    }
}
//-----------------------------------------------------------------
/**
 * Play sound repeatedly.
 * @param name sound name
 * @return the channel the sample is played on. On any errors, -1 is returned.
 */
int SDLSound::playSoundRepeatedly(const char* name)
{
    int channel = -1;
    SoundData *sdata = findChunk(name);
    if (sdata) {
        if ((channel = Mix_PlayChannel(-1, sdata->getData(), -1)) == -1) {
            LOG (("Couldn't play sound '%s': %s", name, Mix_GetError()));
        }
    }

    return channel;
}
//-----------------------------------------------------------------
/**
 * Stop playing the channel.
 * @param channel channel to stop
 */
void SDLSound::stopChannel(int channel)
{
    if (channel != -1) {
        Mix_HaltChannel(channel);
    }
}
//-----------------------------------------------------------------
int SDLSound::getSoundVolume(long distance)
{
    //0 to 2 800x600 screen widths away--
    if( (distance < 640000)) return MIX_MAX_VOLUME;

    //2 to 4 800x600 screen widths away--
    if( (distance < 10240000)) return int(0.75 * MIX_MAX_VOLUME);

    //4 to 8 800x600 screen widths away--
    if( (distance < 40960000)) return int(0.5 * MIX_MAX_VOLUME);

    //8 to 12 800x600 screen widths away--
    if( (distance < 92760000)) return int(0.25 * MIX_MAX_VOLUME);

    //12 to 16 800x600 screen widths away--
    if( (distance < 163840000)) return int(0.05 * MIX_MAX_VOLUME);

    //anything further away--
    return 0;
}
//-----------------------------------------------------------------
/**
 * Load all *.wav from directory.
 * @param directory path to the directory
 */
void SDLSound::loadSound(const char* directory)
{
    char **list = filesystem::enumerateFiles(directory);

    for (char **i = list; *i != NULL; i++)
    {
        std::string filename = directory;
        filename.append(*i);
        if (!filesystem::isDirectory(filename.c_str()))
        {
            try
            {
                filesystem::ReadFile *file 
                    = filesystem::openRead(filename.c_str());
                Mix_Chunk *chunk = Mix_LoadWAV_RW(file->getSDLRWOps(), 1);
                if (chunk)
                {
                    std::string idName = getIdName(*i);
                    m_chunks.insert( std::pair<std::string,SoundData*>(idName, new SoundData(chunk)) );
                }
                else
                {
                    LOGGER.info("Couldn't load wav_rw '%s': %s",
                          filename.c_str(), Mix_GetError());
                }
            }
            catch (Exception &e)
            {
                LOGGER.info("Couldn't load wav '%s': %s", filename.c_str(), e.what());
            }
        }
    }
    filesystem::freeList(list);
    setSoundVolume(GameConfig::sound_effectsvol);
}
//-----------------------------------------------------------------
/**
 * Hash filename to idName.
 * @return id name
 */
std::string SDLSound::getIdName(const char *filename)
{
    std::string name = filename;
    std::string::size_type pos = name.find_first_of("._");

    return name.substr(0, pos);
}

void SDLSound::setSoundVolume(unsigned int volume)
{
    if ( volume > 100 ) volume = 100;
    unsigned int sdlvol = (volume*100)/MIX_MAX_VOLUME;
    chunks_t::iterator i = m_chunks.begin();
    while ( i != m_chunks.end() ) {
        Mix_VolumeChunk(i->second->getData(), sdlvol);
        ++i;
    }
}

//---------------------------------------------------------------------------
// Music part
//---------------------------------------------------------------------------

void SDLSound::playMusic(const char* directory)
{
    // Part1: scan directory for music files
    char **list = filesystem::enumerateFiles(directory);
    setMusicVolume(GameConfig::sound_musicvol);

    musicfiles.clear();
    for (char **i = list; *i != NULL; i++) {
        std::string filename = directory;
        filename.append(*i);
        if (!filesystem::isDirectory(filename.c_str())) {
            musicfiles.push_back(filename);
        }
    }
    filesystem::freeList(list);

    if(musicfiles.size() == 0) {
        LOGGER.info("Couldn't find any music in '%s'", directory);
        return;
    }

    // Part2: play music :)
    currentsong = musicfiles.end();
    nextSong();
    Mix_HookMusicFinished(nextSong);
}

void SDLSound::stopMusic()
{
    // nicely fade the music out for 1 second
    if(Mix_PlayingMusic()) {
        Mix_HookMusicFinished(0);
        Mix_FadeOutMusic(500);
    }
}

void SDLSound::setMusicVolume(unsigned int volume)
{
    Mix_VolumeMusic((volume*100)/MIX_MAX_VOLUME);
}

void SDLSound::nextSong()
{
    static Mix_Music *music = 0;
    if (music != 0) {
        Mix_HaltMusic();
        Mix_FreeMusic(music);
        music = 0;
    }

    if(currentsong == musicfiles.end()) {
        // create a new random playlist
        std::random_shuffle(musicfiles.begin(), musicfiles.end());
        currentsong = musicfiles.begin();
    }

    musics_t::iterator lastsong = currentsong;
    do {
        const char* toplay = currentsong->c_str();
        currentsong++;
//#ifdef HAS_LOADMUS_RW
        /*
         * use LoadMUS_RW from newer SDL_mixers
         */
        try {
            filesystem::ReadFile *file = filesystem::openRead(toplay);
            music = Mix_LoadMUS_RW(file->getSDLRWOps(), 1);
            if (music) {
                if (Mix_PlayMusic(music, 1) == 0) {
                    LOG (("Start playing song '%s'", toplay));
                    break; // break while cycle
                } else {
                    LOG (("Failed to play song '%s': %s",
                          toplay, Mix_GetError()));
                }
            } else {
                LOG (("Failed to load mus_rw '%s': %s",
                      toplay, Mix_GetError()));
            }
        } catch (Exception &e) {
            LOG (("Failed to load song '%s': %s",
                  toplay, e.what()));
        }
#if 0
        music = Mix_LoadMUS(filesystem::getRealName(toplay).c_str());
        if (music) {
            if (Mix_PlayMusic(music, 1) == 0) {
                LOG (("Start playing song '%s'", toplay));
                break; // break while cycle
            } else {
                LOG (("Failed to play song '%s': %s",
                      toplay, Mix_GetError()));
            }
        } else {
            LOG (("Failed to load song '%s': %s",
                  toplay, Mix_GetError()));
        }
#endif

        if(currentsong == musicfiles.end()) {
            currentsong = musicfiles.begin();
        }
    } while(currentsong != lastsong);
}

