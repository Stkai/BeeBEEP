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
#include "FileInfo.h"
#include "Settings.h"


AudioManager* AudioManager::mp_instance = Q_NULLPTR;

AudioManager::AudioManager()
  : mp_sound( Q_NULLPTR )
{
#ifdef BEEBEEP_USE_VOICE_CHAT
  checkAudioDevice();
#endif
}

#if QT_VERSION >= 0x050000
void AudioManager::checkAudioDevice()
{
  QAudioDeviceInfo dev_info = QAudioDeviceInfo::defaultInputDevice();
  QStringList supported_codecs = dev_info.supportedCodecs();
  qDebug() << "AudioManager supports these input codecs:" << qPrintable( supported_codecs.join( ", " ) );

  QList<int> supported_sample_rates = dev_info.supportedSampleRates();
  QStringList sl;
  foreach( int sr, supported_sample_rates) {
    sl.append( QString::number( sr ) );
  }
  qDebug() << "AudioManager supports these sample rates:" << qPrintable( sl.join( ", " ) );

  QList<int> supported_sample_sizes = dev_info.supportedSampleSizes();
  sl.clear();
  foreach( int sr, supported_sample_sizes ) {
    sl.append( QString::number( sr ) );
  }
  qDebug() << "AudioManager supports these sample sizes:" << qPrintable( sl.join( ", " ) );

  QList<int> supported_channels = dev_info.supportedChannelCounts();
  sl.clear();
  foreach( int sr, supported_channels ) {
    sl.append( QString::number( sr ) );
  }
  qDebug() << "AudioManager supports these channels:" << qPrintable( sl.join( ", " ) );

  QAudioFormat audio_format;
  audio_format.setSampleRate( 8000 );
  audio_format.setChannelCount( 1 );
  audio_format.setSampleSize( 16 );
  audio_format.setSampleType( QAudioFormat::SignedInt );
  audio_format.setByteOrder( QAudioFormat::LittleEndian );
  audio_format.setCodec( "audio/pcm" );
  QAudioDeviceInfo test_device_info( defaultInputDevice() );
  if( !test_device_info.isFormatSupported( audio_format ) )
  {
    qWarning() << "AudioManager: default format is not supported. Trying to use nearest...";
    m_defaultAudioFormat = test_device_info.nearestFormat( audio_format );
  }
  else
    m_defaultAudioFormat = audio_format;
  qDebug() << "AudioManager uses audio format as default:" << qPrintable( m_defaultAudioFormat.codec() ) << m_defaultAudioFormat.sampleRate()
           << m_defaultAudioFormat.channelCount() << m_defaultAudioFormat.sampleSize() << m_defaultAudioFormat.sampleType() << m_defaultAudioFormat.byteOrder();

  QAudioRecorder audio_recorder;
  QAudioEncoderSettings audio_encoder_settings;
  supported_codecs = audio_recorder.supportedAudioCodecs();
  qDebug() << "AudioManager supports these audio codecs (encoder):" << qPrintable( supported_codecs.join( ", " ) );
  if( supported_codecs.contains( "audio/x-speex" ) )
    audio_encoder_settings.setCodec( "audio/x-speex" );
  else
    audio_encoder_settings.setCodec( audio_format.codec() );
  audio_encoder_settings.setChannelCount( m_defaultAudioFormat.channelCount() );
  audio_encoder_settings.setSampleRate( m_defaultAudioFormat.sampleRate() );
  audio_encoder_settings.setQuality( QMultimedia::NormalQuality );
  audio_encoder_settings.setEncodingMode( QMultimedia::ConstantQualityEncoding );
  m_defaultAudioEncoderSettings = audio_encoder_settings;
  qDebug() << "AudioManager uses audio encoder format as default:" << qPrintable( m_defaultAudioEncoderSettings.codec() );
}

QAudioDeviceInfo AudioManager::defaultInputDevice()
{
  return QAudioDeviceInfo::defaultInputDevice();
}

QString AudioManager::defaultAudioContainer()
{
  if( m_defaultAudioEncoderSettings.codec() == "audio/x-speex" )
    return QLatin1String( "audio/ogg" );
  else
    return QLatin1String( "audio/wav" );
}

QString AudioManager::defaultAudioContainerFilePrefix()
{
  return QLatin1String( "beemsg" );
}

QString AudioManager::defaultAudioContainerFileSuffix()
{
  if( m_defaultAudioEncoderSettings.codec() == "audio/x-speex" )
    return QLatin1String( "ogg" );
  else if( m_defaultAudioEncoderSettings.codec() == "audio/x-wav" )
    return QLatin1String( "wav" );
  else
    return QLatin1String( "raw" );
}

QString AudioManager::createDefaultAudioContainerFilename()
{
  return QString( "%1-%2.%3" ).arg( AudioManager::defaultAudioContainerFilePrefix() ).arg( Bee::dateTimeStringSuffix( QDateTime::currentDateTime() ) ).arg( AudioManager::defaultAudioContainerFileSuffix() );
}

#else
void AudioManager::checkAudioDevice() {}
#endif

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
