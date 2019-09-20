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
#include "BeeUtils.h"
#include "Settings.h"


AudioManager* AudioManager::mp_instance = Q_NULLPTR;

AudioManager::AudioManager()
  : mp_sound( Q_NULLPTR )
{
}

QAudioFormat AudioManager::defaultAudioFormat()
{
  QAudioFormat audio_format;
  audio_format.setSampleRate( 8000 );
  audio_format.setChannelCount( 1 );
  audio_format.setSampleSize( 16 );
  audio_format.setSampleType( QAudioFormat::SignedInt );
  audio_format.setByteOrder( QAudioFormat::LittleEndian );
  audio_format.setCodec( "audio/x-speex" );
  QAudioDeviceInfo test_device_info( defaultInputDevice() );
  if( !test_device_info.isFormatSupported( audio_format ) )
  {
    qWarning() << "AudioManager: default format is not supported. Trying to use nearest...";
    audio_format = test_device_info.nearestFormat( audio_format );
  }
  return audio_format;
}

QAudioDeviceInfo AudioManager::defaultInputDevice()
{
  return QAudioDeviceInfo::defaultInputDevice();
}

#if QT_VERSION >= 0x050000
QAudioEncoderSettings AudioManager::defaultAudioEncoderSettings()
{
  QAudioEncoderSettings audio_encoder_settings;
  audio_encoder_settings.setCodec( "audio/x-speex" );
  audio_encoder_settings.setQuality( QMultimedia::NormalQuality );
  audio_encoder_settings.setEncodingMode( QMultimedia::ConstantQualityEncoding );
  return audio_encoder_settings;
}
#endif

QString AudioManager::defaultAudioContainer()
{
  return QLatin1String( "audio/ogg" );
}

QString AudioManager::defaultAudioContainerFileSuffix()
{
  return QLatin1String( "ogg" );
}

#if defined( Q_OS_OS2 )
  bool AudioManager::isAudioDeviceAvailable() { return true; }
  void AudioManager::clearBeep() {}
  void AudioManager::playBeep() { QApplication::beep(); }
#else

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
    mp_sound = Q_NULLPTR;
  }
}

void AudioManager::playBeep()
{
  if( !mp_sound )
  {
    if( isAudioDeviceAvailable() )
    {
      QString beep_file_path = Settings::instance().beepFilePath();
      if( !QFile::exists( beep_file_path ) )
      {
        qDebug() << "AudioManager did not find BEEP file" << qPrintable( beep_file_path );
        beep_file_path = Settings::instance().defaultBeepFilePath();
      }

      if( QFile::exists( beep_file_path ) )
      {
#ifdef BEEBEEP_USE_PHONON4
        qDebug() << "AudioManager create PHONON sound object from" << qPrintable( beep_file_path );
        Phonon::MediaSource media_source( QUrl::fromLocalFile( beep_file_path ) );
        mp_sound = Phonon::createPlayer( Phonon::MusicCategory, media_source );
#else
        qDebug() << "AudioManager create sound object from" << qPrintable( beep_file_path );
        mp_sound = new QSound( beep_file_path );
#endif
      }
    }
  }

  if( mp_sound )
    mp_sound->play();
  else
    QApplication::beep();
}
#endif
