//////////////////////////////////////////////////////////////////////
//
// This file is part of BeeBEEP.
//
// BeeBEEP is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published
// by the Free Software Foundation, either version 3 of the License,
// or (at your option) any later version.
//
// BeeBEEP is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with BeeBEEP.  If not, see <http://www.gnu.org/licenses/>.
//
// Author: Marco Mastroddi <marco.mastroddi(AT)gmail.com>
//
// $Id$
//
//////////////////////////////////////////////////////////////////////

#include "AudioManager.h"
#include "Settings.h"


AudioManager* AudioManager::mp_instance = NULL;

AudioManager::AudioManager()
  : mp_sound( 0 )
{
}

bool AudioManager::isAudioDeviceAvailable()
{
#if QT_VERSION >= 0x050000
  return !QAudioDeviceInfo::availableDevices( QAudio::AudioOutput ).isEmpty();
#else
  #ifdef Q_OS_UNIX
    return true; // doesn't work -> !Phonon::BackendCapabilities::availableAudioOutputDevices().isEmpty();
  #else
    return QSound::isAvailable();
  #endif
#endif
}

void AudioManager::clearBeep()
{
  if( mp_sound )
  {
    qDebug() << "AudioManager clear sound object";
    mp_sound->deleteLater();
    mp_sound = 0;
  }
}

void AudioManager::playBeep()
{
  if( !mp_sound )
  {
    if( QFile::exists( Settings::instance().beepFilePath() ) && isAudioDeviceAvailable() )
    {
#ifdef BEEBEEP_USE_PHONON4
      qDebug() << "AudioManager create PHONON sound object from" << Settings::instance().beepFilePath();
      Phonon::MediaSource media_source( Settings::instance().beepFilePath() );
      mp_sound = Phonon::createPlayer( Phonon::MusicCategory, media_source );
#else
      qDebug() << "AudioManager create sound object from" << Settings::instance().beepFilePath();
      mp_sound = new QSound( Settings::instance().beepFilePath() );
#endif
    }
  }

  if( mp_sound )
    mp_sound->play();
  else
    QApplication::beep();
}
