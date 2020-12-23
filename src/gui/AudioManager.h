//////////////////////////////////////////////////////////////////////
//
// BeeBEEP Copyright (C) 2010-2021 Marco Mastroddi
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
// along with BeeBEEP. If not, see <http://www.gnu.org/licenses/>.
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
  bool loadBeepEffect();
  void playBeep( int loops = 1 );
  void clearBeep();
  void playBuzz();

#if defined( BEEBEEP_USE_VOICE_CHAT )
  void checkDefaultAudioDevice();
  QString voiceInputDeviceName() const;

  QAudioEncoderSettings voiceMessageEncoderSettings() const;
  QString voiceMessageFileContainer() const;
  inline const QString& defaultInputDeviceName() const;
  inline const QAudioEncoderSettings& defaultVoiceMessageEncoderSettings() const;
  inline const QString& defaultVoiceMessageFileContainer() const;
  QString createDefaultVoiceMessageFilename( const QString& container_name );
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
  void checkAudioDevice( const QAudioDeviceInfo& input_device, QAudioEncoderSettings* audio_settings, QString* file_container );
  QString defaultVoiceMessageContainerFilePrefix() const;
  QString defaultVoiceMessageContainerFileSuffix() const;
  bool findBestVoiceMessageCodecContainers( const QStringList& codecs, const QStringList& containers, QString* best_codec, QString* best_container ) const;
  inline QString findBestVoiceMessageContainer( const QString& codec ) const;
  inline QString voiceMessageContainerExtension( const QString& container ) const;
#endif

private:
#if defined( Q_OS_OS2 )
  void* mp_sound;
#elif defined( BEEBEEP_USE_PHONON4 )
  Phonon::MediaObject *mp_sound;
#elif QT_VERSION >= 0x050000
  QSoundEffect* mp_sound;
#else
  QSound* mp_sound;
#endif
#if defined( BEEBEEP_USE_VOICE_CHAT )
  QString m_defaultInputDeviceName;
  QAudioEncoderSettings m_defaultVoiceMessageEncoderSettings;
  QString m_defaultVoiceMessageFileContainer;
  QMap<QString, QString> m_voiceMessageCodecContainers;
  QMap<QString, QString> m_voiceMessageCodecContainerExtensions;
#endif
};


// Inline Functions
#if defined( BEEBEEP_USE_VOICE_CHAT )
inline const QString& AudioManager::defaultInputDeviceName() const { return m_defaultInputDeviceName; }
inline const QAudioEncoderSettings& AudioManager::defaultVoiceMessageEncoderSettings() const { return m_defaultVoiceMessageEncoderSettings; }
inline const QString& AudioManager::defaultVoiceMessageFileContainer() const { return m_defaultVoiceMessageFileContainer; }
inline QString AudioManager::findBestVoiceMessageContainer( const QString& codec ) const {return m_voiceMessageCodecContainers.value( codec, QString() ); }
inline QString AudioManager::voiceMessageContainerExtension( const QString& container ) const { return m_voiceMessageCodecContainerExtensions.value( container, QString() ); }
#endif

#endif // BEEBEEP_AUDIOMANAGER_H
