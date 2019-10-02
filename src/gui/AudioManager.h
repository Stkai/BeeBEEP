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

#ifndef BEEBEEP_AUDIOMANAGER_H
#define BEEBEEP_AUDIOMANAGER_H

#include "Config.h"
#ifdef BEEBEEP_USE_PHONON4
  #include <phonon/MediaObject>
#endif
class FileInfo;


class AudioManager
{
// Singleton Object
  static AudioManager* mp_instance;

public:
  bool isAudioDeviceAvailable();
  void playBeep();
  void clearBeep();

#if QT_VERSION >= 0x050000
  QAudioDeviceInfo defaultInputDevice();
  inline const QAudioFormat& defaultVoiceFormat() const;
  inline const QAudioEncoderSettings& defaultVoiceEncoderSettings() const;
  QString createDefaultVoiceMessageFilename();
#endif

  static AudioManager& instance()
  {
    if( !mp_instance )
      mp_instance = new AudioManager();
    return *mp_instance;
  }

  static void close()
  {
    if( mp_instance )
    {
      mp_instance->clearBeep();
      delete mp_instance;
      mp_instance = Q_NULLPTR;
    }
  }

protected:
  AudioManager();
#if QT_VERSION >= 0x050000
  void checkAudioDevice();
  QString defaultVoiceContainer();
  QString defaultVoiceContainerFilePrefix();
  QString defaultVoiceContainerFileSuffix();
#endif

private:
#if defined( Q_OS_OS2 )
  void* mp_sound;
#elif defined( BEEBEEP_USE_PHONON4 )
  Phonon::MediaObject *mp_sound;
#else
  QSound* mp_sound;
#endif
#if QT_VERSION >= 0x050000
  QAudioFormat m_defaultVoiceFormat;
  QAudioEncoderSettings m_defaultVoiceEncoderSettings;
  QString m_currentInputDeviceName;
#endif
};


// Inline Functions
#if QT_VERSION >= 0x050000
inline const QAudioFormat& AudioManager::defaultVoiceFormat() const { return m_defaultVoiceFormat; }
inline const QAudioEncoderSettings& AudioManager::defaultVoiceEncoderSettings() const { return m_defaultVoiceEncoderSettings; }
#endif
#endif // BEEBEEP_AUDIOMANAGER_H
