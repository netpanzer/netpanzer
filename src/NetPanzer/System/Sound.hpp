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
#ifndef __UILIB_SOUND_HPP__
#define __UILIB_SOUND_HPP__

/** This is an interface to sound functions in the game */
class Sound
{
public:
    enum Event{
        _selected, _deselected, _move_to, _target_enemy, _no_line_of_fire,
        _under_attack, _fire_gun, _hit_target, _miss_target, _scream,
        _random_battle, _blow_up_tank
    };

    Sound();
    virtual ~Sound()
    { }

    void playTankIdle();
    void stopTankIdle();
    void playPowerUpSound();
    void playUnitSound(int unit_type);
    void playBattle();

    virtual void playSound(const char* name) = 0;
    virtual void playAmbientSound(const char *name, long distance)=0;
    virtual int playSoundRepeatedly(const char* name) = 0;
    virtual void stopChannel(int channel) = 0;
    /// set sound volume (0-100)
    virtual void setSoundVolume(unsigned int volume) = 0;

    virtual void playMusic(const char* directory) = 0;
    virtual void stopMusic() = 0;
    /// set music volume (0-100)
    virtual void setMusicVolume(unsigned int volume) = 0;

private:
    int m_tankIdleChannel;
    int m_battleCount;
};

extern Sound* sound;

#endif

