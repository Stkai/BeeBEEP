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
  void playBeep( int loops = 1 );
  void clearBeep();
  void playBuzz();

#if defined( BEEBEEP_USE_VOICE_CHAT )
  void checkAudioDevice( const QAudioDeviceInfo& input_device, QAudioEncoderSettings* audio_settings, QString* file_container );
  QString voiceInputDeviceName() const;
  QAudioEncoderSettings voiceEncoderSettings() const;
  QString voiceFileContainer() const;
  inline const QString& defaultInputDeviceName() const;
  inline const QAudioEncoderSettings& defaultVoiceEncoderSettings() const;
  inline const QString& defaultVoiceFileContainer() const;
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
#if defined( BEEBEEP_USE_VOICE_CHAT )
  void checkDefaultAudioDevice();
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
#if defined( BEEBEEP_USE_VOICE_CHAT )
  QString m_defaultInputDeviceName;
  QAudioEncoderSettings m_defaultVoiceEncoderSettings;
  QString m_defaultVoiceFileContainer;
#endif
};


// Inline Functions
#if defined( BEEBEEP_USE_VOICE_CHAT )
inline const QString& AudioManager::defaultInputDeviceName() const { return m_defaultInputDeviceName; }
inline const QAudioEncoderSettings& AudioManager::defaultVoiceEncoderSettings() const { return m_defaultVoiceEncoderSettings; }
inline const QString& AudioManager::defaultVoiceFileContainer() const { return m_defaultVoiceFileContainer; }
#endif
#endif // BEEBEEP_AUDIOMANAGER_H
