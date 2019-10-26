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
   , m_defaultInputDeviceName(), m_defaultVoiceMessageEncoderSettings(), m_defaultVoiceMessageFileContainer()
   , m_voiceMessageCodecContainers(), m_voiceMessageCodecContainerExtensions()
{
  m_voiceMessageCodecContainers.insert( QLatin1String( "audio/pcm" ), QLatin1String( "x-wav" ) );
  m_voiceMessageCodecContainers.insert( QLatin1String( "audio/mpeg, mpegversion=(int)1, layer=(int)3" ), QLatin1String( "audio/x-matroska" ) );
  m_voiceMessageCodecContainers.insert( QLatin1String( "x-opus" ), QLatin1String( "audio/webm" ) );
  m_voiceMessageCodecContainers.insert( QLatin1String( "opus" ), QLatin1String( "audio/webm" ) );
  m_voiceMessageCodecContainers.insert( QLatin1String( "x-speex" ), QLatin1String( "audio/ogg" ) );
  m_voiceMessageCodecContainers.insert( QLatin1String( "speex" ), QLatin1String( "audio/ogg" ) );

  m_voiceMessageCodecContainerExtensions.insert( QLatin1String( "audio/wave" ), QLatin1String( "wav" ) );
  m_voiceMessageCodecContainerExtensions.insert( QLatin1String( "audio/wav" ), QLatin1String( "wav" ) );
  m_voiceMessageCodecContainerExtensions.insert( QLatin1String( "audio/x-wav" ), QLatin1String( "wav" ) );
  m_voiceMessageCodecContainerExtensions.insert( QLatin1String( "audio/x-pn-wav" ), QLatin1String( "wav" ) );
  m_voiceMessageCodecContainerExtensions.insert( QLatin1String( "audio/x-matroska" ), QLatin1String( "mka" ) );
  m_voiceMessageCodecContainerExtensions.insert( QLatin1String( "audio/webm" ), QLatin1String( "webm" ) );
  m_voiceMessageCodecContainerExtensions.insert( QLatin1String( "audio/ogg" ), QLatin1String( "ogg" ) );
  m_voiceMessageCodecContainerExtensions.insert( QLatin1String( "application/ogg" ), QLatin1String( "ogg" ) );
  m_voiceMessageCodecContainerExtensions.insert( QLatin1String( "audio/x-raw" ), QLatin1String( "raw" ) );

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
  checkAudioDevice( input_device, &m_defaultVoiceMessageEncoderSettings, &m_defaultVoiceMessageFileContainer );
}

bool AudioManager::findBestVoiceMessageCodecContainers( const QStringList& codecs, const QStringList& containers, QString* best_codec, QString* best_container ) const
{
  if( codecs.isEmpty() || containers.isEmpty() )
    return false;

  foreach( QString codec_name, m_voiceMessageCodecContainers.keys() )
  {
    if( codecs.contains( codec_name, Qt::CaseInsensitive ) )
    {
      QString codec_container = findBestVoiceMessageContainer( codec_name );
      if( containers.contains( codec_container, Qt::CaseInsensitive ) )
      {
        *best_codec = codec_name;
        *best_container = codec_container;
        return true;
      }
    }
  }
  return false;
}

void AudioManager::checkAudioDevice( const QAudioDeviceInfo& input_device, QAudioEncoderSettings* audio_settings, QString* file_container )
{
  QStringList supported_codecs = input_device.supportedCodecs();
  qDebug() << "AudioManager supports these input codecs:" << qPrintable( supported_codecs.join( "; " ) );
  QList<int> supported_sample_rates = input_device.supportedSampleRates();
  QStringList sl;
  foreach( int sr, supported_sample_rates) {
    sl.append( QString::number( sr ) );
  }
  qDebug() << "AudioManager supports these sample rates:" << qPrintable( sl.join( "; " ) );
  QList<int> supported_sample_sizes = input_device.supportedSampleSizes();
  sl.clear();
  foreach( int sr, supported_sample_sizes ) {
    sl.append( QString::number( sr ) );
  }
  qDebug() << "AudioManager supports these sample sizes:" << qPrintable( sl.join( "; " ) );
  QList<int> supported_channels = input_device.supportedChannelCounts();
  sl.clear();
  foreach( int sr, supported_channels ) {
    sl.append( QString::number( sr ) );
  }
  qDebug() << "AudioManager supports these channels:" << qPrintable( sl.join( "; " ) );

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
  QStringList supported_containers = audio_recorder.supportedContainers();
  qDebug() << "AudioManager supports these audio codecs (voice encoder):" << qPrintable( supported_codecs.join( "; " ) );
  qDebug() << "AudioManager supports these audio file containers:" << qPrintable( supported_containers.join( "; " ) );

  QString best_voice_encoder_codec;
  QString best_voice_encoded_container;
  bool best_codec_found = Settings::instance().useSystemVoiceEncoderSettings() ? findBestVoiceMessageCodecContainers( supported_codecs, supported_containers, &best_voice_encoder_codec, &best_voice_encoded_container ) : false;
  if( best_codec_found )
  {
    qDebug() << "AudioManager has selected this codec (voice encoder):" << qPrintable( best_voice_encoder_codec );
    audio_settings->setCodec( best_voice_encoder_codec );
    qDebug() << "AudioManager has selected this file cointainer (voice - mime type):" << qPrintable( best_voice_encoded_container );
    *file_container = best_voice_encoded_container;
  }
  else
  {
    audio_settings->setCodec( QString() );
    audio_settings->setSampleRate( audio_format.sampleRate() );
    *file_container = QLatin1String( "audio/x-wav" );
    qDebug() << "AudioManager does not encode voice message and saves it as" << qPrintable( *file_container );
  }

  audio_settings->setChannelCount( audio_format.channelCount() );
  audio_settings->setQuality( QMultimedia::NormalQuality );
  audio_settings->setEncodingMode( QMultimedia::ConstantQualityEncoding );
}

QString AudioManager::createDefaultVoiceMessageFilename( const QString& container_name )
{
  QString file_ext = voiceMessageContainerExtension( container_name );
  if( file_ext.isEmpty() )
    file_ext = QLatin1String( "raw" );
  QString valid_owner_name = Bee::removeInvalidCharactersForFilePath( Settings::instance().localUser().name().simplified() );
  valid_owner_name.replace( " ", "" );
  return QString( "beemsg-%2-%3.%4" ).arg( valid_owner_name ).arg( Bee::dateTimeStringSuffix( QDateTime::currentDateTime() ) ).arg( file_ext );
}

QString AudioManager::voiceInputDeviceName() const
{
  return Settings::instance().voiceInputDeviceName().isEmpty() ? defaultInputDeviceName() : Settings::instance().voiceInputDeviceName();
}

QString AudioManager::voiceMessageFileContainer() const
{
  return Settings::instance().voiceFileMessageContainer().isEmpty() ? defaultVoiceMessageFileContainer() : Settings::instance().voiceFileMessageContainer();
}

QAudioEncoderSettings AudioManager::voiceMessageEncoderSettings() const
{
  QAudioEncoderSettings aes;
  if( Settings::instance().useCustomVoiceEncoderSettings() )
  {
    aes.setCodec( Settings::instance().voiceCodec() );
    aes.setSampleRate( Settings::instance().voiceSampleRate() );
    aes.setBitRate( Settings::instance().voiceBitRate() );
    aes.setChannelCount( Settings::instance().voiceChannels() );
    aes.setEncodingMode( static_cast<QMultimedia::EncodingMode>( Settings::instance().voiceEncodingMode() ) );
    aes.setQuality( static_cast<QMultimedia::EncodingQuality>( Settings::instance().voiceEncodingQuality() ) );
  }
  else
    aes = defaultVoiceMessageEncoderSettings();
  return aes;
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
