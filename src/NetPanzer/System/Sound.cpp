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


#include "Sound.hpp"

Sound* sound = 0;

//-----------------------------------------------------------------
Sound::Sound()
{
    m_battleCount = 0;
    m_tankIdleChannel = -1;
}
//-----------------------------------------------------------------
/**
 * Play "tankidle" repeatedly.
 */
void Sound::playTankIdle()
{
    m_tankIdleChannel = playSoundRepeatedly("tankidle");
}

void Sound::stopTankIdle()
{
    stopChannel(m_tankIdleChannel);
}

void Sound::playPowerUpSound()
{
    playSound("powerup");
}

void Sound::playUnitSound(int )
{
    playSound("unitsound");
}

//-----------------------------------------------------------------
/**
 * When many weapons are firing the battle sound arise.
 */
void Sound::playBattle()
{
    static const int BATTLE_LIMIT = 16;

    //TODO: this is original behavior,
    // does it make sense?
    m_battleCount++;
    if (m_battleCount > BATTLE_LIMIT) {
        playSound("battle");
        m_battleCount = 0;
    }
}
