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
#if defined( BEEBEEP_USE_VOICE_CHAT )
   , m_defaultInputDeviceName(), m_defaultVoiceEncoderSettings(), m_defaultVoiceFileContainer( "audio/x-wav" )
{
  checkDefaultAudioDevice();
}
#else
{}
#endif

#if defined( BEEBEEP_USE_VOICE_CHAT )
void AudioManager::checkDefaultAudioDevice()
{
  QAudioDeviceInfo input_device = QAudioDeviceInfo::defaultInputDevice();
  m_defaultInputDeviceName = input_device.deviceName();
  qDebug() << "AudioManager uses default input device:" << qPrintable( input_device.deviceName() );
  checkAudioDevice( input_device, &m_defaultVoiceEncoderSettings, &m_defaultVoiceFileContainer );
}

void AudioManager::checkAudioDevice( const QAudioDeviceInfo& input_device, QAudioEncoderSettings* audio_settings, QString* file_container )
{
  QStringList supported_codecs = input_device.supportedCodecs();
  qDebug() << "AudioManager supports these input codecs:" << qPrintable( supported_codecs.join( ", " ) );
  QList<int> supported_sample_rates = input_device.supportedSampleRates();
  QStringList sl;
  foreach( int sr, supported_sample_rates) {
    sl.append( QString::number( sr ) );
  }
  qDebug() << "AudioManager supports these sample rates:" << qPrintable( sl.join( ", " ) );
  QList<int> supported_sample_sizes = input_device.supportedSampleSizes();
  sl.clear();
  foreach( int sr, supported_sample_sizes ) {
    sl.append( QString::number( sr ) );
  }
  qDebug() << "AudioManager supports these sample sizes:" << qPrintable( sl.join( ", " ) );
  QList<int> supported_channels = input_device.supportedChannelCounts();
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
  if( !input_device.isFormatSupported( audio_format ) )
  {
    qWarning() << "AudioManager: default format is not supported. Trying to use nearest...";
    audio_format = input_device.nearestFormat( audio_format );
  }
  qDebug() << "AudioManager uses audio format as default:" << qPrintable( audio_format.codec() ) << audio_format.sampleRate()
           << audio_format.channelCount() << audio_format.sampleSize() << audio_format.sampleType() << audio_format.byteOrder();

  QAudioRecorder audio_recorder;
  supported_codecs = audio_recorder.supportedAudioCodecs();
  QString best_voice_encoder_codec;
  bool use_speex = false;
  if( !supported_codecs.isEmpty() )
  {
    qDebug() << "AudioManager supports these audio codecs (voice encoder):" << qPrintable( supported_codecs.join( ", " ) );
    // Looking for SPEEX codec
    if( best_voice_encoder_codec.isEmpty() )
    {
      foreach( QString supported_codec, supported_codecs )
      {
        if( supported_codec.contains( QLatin1String( "speex" ), Qt::CaseInsensitive ) )
        {
          best_voice_encoder_codec = supported_codec;
          use_speex = true;
          break;
        }
      }
    }
  }

  if( best_voice_encoder_codec.isEmpty() )
    best_voice_encoder_codec = audio_format.codec(); // usually audio/pcm

  qDebug() << "AudioManager has selected this codec (voice encoder):" << qPrintable( best_voice_encoder_codec );
  *audio_settings = QAudioEncoderSettings();
  audio_settings->setCodec( best_voice_encoder_codec );
  if( use_speex )
  {
    audio_settings->setSampleRate( audio_format.sampleRate() );
    *file_container = "audio/ogg";
  }
  else
  {
    audio_settings->setSampleRate( audio_format.sampleRate() );
    audio_settings->setChannelCount( audio_format.channelCount() );
    *file_container = "audio/x-wav";
  }
  audio_settings->setQuality( QMultimedia::NormalQuality );
  audio_settings->setEncodingMode( QMultimedia::ConstantQualityEncoding );
  qDebug() << "AudioManager has selected this voice container (file):" << qPrintable( *file_container );
}

QString AudioManager::defaultVoiceContainerFilePrefix()
{
  return QLatin1String( "beemsg" );
}

QString AudioManager::defaultVoiceContainerFileSuffix()
{
  QString current_file_container = voiceFileContainer();
  if( current_file_container == "audio/ogg" )
    return QLatin1String( "ogg" );
  else if( current_file_container == "audio/x-wav" )
    return QLatin1String( "wav" );
  else
    return QLatin1String( "raw" );
}

QString AudioManager::createDefaultVoiceMessageFilename()
{
  QString valid_owner_name = Bee::removeInvalidCharactersForFilePath( Settings::instance().localUser().name().simplified() );
  valid_owner_name.replace( " ", "" );
  return QString( "%1-%2-%3.%4" ).arg( AudioManager::defaultVoiceContainerFilePrefix() ).arg( valid_owner_name ).arg( Bee::dateTimeStringSuffix( QDateTime::currentDateTime() ) ).arg( defaultVoiceContainerFileSuffix() );
}

QString AudioManager::voiceInputDeviceName() const
{
  return Settings::instance().voiceInputDeviceName().isEmpty() ? defaultInputDeviceName() : Settings::instance().voiceInputDeviceName();
}

QAudioEncoderSettings AudioManager::voiceEncoderSettings() const
{
  QAudioEncoderSettings aes;
  aes.setCodec( Settings::instance().voiceCodec().isEmpty() ? AudioManager::instance().defaultVoiceEncoderSettings().codec() : Settings::instance().voiceCodec() );
  aes.setSampleRate( Settings::instance().voiceSampleRate() <= 0 ? AudioManager::instance().defaultVoiceEncoderSettings().sampleRate() : Settings::instance().voiceSampleRate() );
  aes.setBitRate( Settings::instance().voiceBitRate() <= 0 ? AudioManager::instance().defaultVoiceEncoderSettings().bitRate() : Settings::instance().voiceBitRate() );
  aes.setChannelCount( Settings::instance().voiceChannels() < 0 ? AudioManager::instance().defaultVoiceEncoderSettings().channelCount() : Settings::instance().voiceChannels() );
  aes.setEncodingMode( Settings::instance().voiceEncodingMode() < 0 ? AudioManager::instance().defaultVoiceEncoderSettings().encodingMode() : static_cast<QMultimedia::EncodingMode>( Settings::instance().voiceEncodingMode() ) );
  aes.setQuality( Settings::instance().voiceEncodingQuality() < 0 ? AudioManager::instance().defaultVoiceEncoderSettings().quality() : static_cast<QMultimedia::EncodingQuality>( Settings::instance().voiceEncodingQuality() ) );
  return aes;
}

QString AudioManager::voiceFileContainer() const
{
  return Settings::instance().voiceFileContainer().isEmpty() ? defaultVoiceFileContainer() : Settings::instance().voiceFileContainer();
}

#endif

#if defined( Q_OS_OS2 )
  bool AudioManager::isAudioDeviceAvailable() { return true; }
  void AudioManager::clearBeep() {}
  void AudioManager::playBeep() { QApplication::beep(); }
  void AudioManager::playBuzz() { playBeep(); }
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

void AudioManager::playBeep( int loops )
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
  {
#ifdef BEEBEEP_USE_PHONON4
    if( loops > 1 )
      mp_sound->play();
#else
    mp_sound->setLoops( loops );
#endif
    mp_sound->play();
  }
  else
    QApplication::beep();
}

void AudioManager::playBuzz()
{
  playBeep( 2 );
}
#endif
