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

#ifdef BEEBEEP_USE_VOICE_CHAT
#include "AudioInfo.h"
#include "AudioManager.h"


AudioInfo::AudioInfo( const QAudioFormat& audio_format, QObject* parent )
 : QIODevice( parent ), m_format( audio_format ), m_maxAmplitude( 0 ), m_level( 0.0 )
{
  if( !checkAudioFormat() )
  {
    m_format = AudioManager::instance().defaultAudioFormat();
    checkAudioFormat();
  }
}

bool AudioInfo::checkAudioFormat()
{
  m_maxAmplitude = 0;
  switch( m_format.sampleSize() )
  {
  case 8:
    switch( m_format.sampleType() )
    {
    case QAudioFormat::UnSignedInt:
      m_maxAmplitude = 255;
      break;
    case QAudioFormat::SignedInt:
      m_maxAmplitude = 127;
      break;
    default:
      break;
    }
    break;
  case 16:
    switch( m_format.sampleType() )
    {
    case QAudioFormat::UnSignedInt:
      m_maxAmplitude = 65535;
      break;
    case QAudioFormat::SignedInt:
      m_maxAmplitude = 32767;
      break;
    default:
      break;
    }
    break;
  case 32:
    switch( m_format.sampleType() )
    {
    case QAudioFormat::UnSignedInt:
      m_maxAmplitude = 0xffffffff;
      break;
    case QAudioFormat::SignedInt:
      m_maxAmplitude = 0x7fffffff;
      break;
    case QAudioFormat::Float:
      m_maxAmplitude = 0x7fffffff; // Kind of
    default:
      break;
    }
    break;
  default:
    break;
  }

  return m_maxAmplitude > 0;
}

void AudioInfo::start()
{
  open( QIODevice::WriteOnly );
}

void AudioInfo::stop()
{
  close();
}

qint64 AudioInfo::readData( char* data, qint64 maxlen )
{
  Q_UNUSED(data)
  Q_UNUSED(maxlen)
  return 0;
}

qint64 AudioInfo::writeData( const char *data, qint64 data_size )
{
  if( m_maxAmplitude > 0 )
  {
    if( m_format.sampleSize() % 8 != 0 )
      qWarning() << "Invalid audio format (sample size) found in AudioInfo::writeData(...)";
    const int channel_bytes = qMin( 1, m_format.sampleSize() / 8 );
    const int sample_bytes = qMin( 1, static_cast<int>(m_format.channelCount() * channel_bytes) );
    if( data_size % sample_bytes != 0 )
      qWarning() << "Invalid audio format (sample bytes and data size) found in AudioInfo::writeData(...)";
    const int num_samples = data_size / sample_bytes;

    quint32 max_value = 0;
    const unsigned char *p_uchar = reinterpret_cast<const unsigned char *>( data );

    for( int i = 0; i < num_samples; ++i )
    {
      for( int j = 0; j < m_format.channelCount(); ++j )
      {
        quint32 value = 0;
        if( m_format.sampleSize() == 8 && m_format.sampleType() == QAudioFormat::UnSignedInt )
        {
          value = *reinterpret_cast<const quint8*>( p_uchar);
        }
        else if( m_format.sampleSize() == 8 && m_format.sampleType() == QAudioFormat::SignedInt )
        {
          value = qAbs(*reinterpret_cast<const qint8*>( p_uchar));
        }
        else if( m_format.sampleSize() == 16 && m_format.sampleType() == QAudioFormat::UnSignedInt )
        {
          if( m_format.byteOrder() == QAudioFormat::LittleEndian )
            value = qFromLittleEndian<quint16>( p_uchar );
          else
            value = qFromBigEndian<quint16>( p_uchar );
        }
        else if( m_format.sampleSize() == 16 && m_format.sampleType() == QAudioFormat::SignedInt )
        {
          if( m_format.byteOrder() == QAudioFormat::LittleEndian )
            value = qAbs( qFromLittleEndian<qint16>( p_uchar ) );
          else
            value = qAbs( qFromBigEndian<qint16>( p_uchar ) );
        }
        else if( m_format.sampleSize() == 32 && m_format.sampleType() == QAudioFormat::UnSignedInt )
        {
          if( m_format.byteOrder() == QAudioFormat::LittleEndian )
            value = qFromLittleEndian<quint32>( p_uchar );
          else
            value = qFromBigEndian<quint32>( p_uchar );
        }
        else if( m_format.sampleSize() == 32 && m_format.sampleType() == QAudioFormat::SignedInt )
        {
          if( m_format.byteOrder() == QAudioFormat::LittleEndian )
            value = qAbs( qFromLittleEndian<qint32>( p_uchar ) );
          else
            value = qAbs( qFromBigEndian<qint32>( p_uchar ) );
        }
        else if( m_format.sampleSize() == 32 && m_format.sampleType() == QAudioFormat::Float )
        {
          value = qAbs(*reinterpret_cast<const float*>(p_uchar) * 0x7fffffff); // assumes 0-1.0
        }

        max_value = qMax( value, max_value );
        p_uchar += channel_bytes;
      }
    }

    max_value = qMin( max_value, m_maxAmplitude );
    m_level = qreal( max_value ) / m_maxAmplitude;
  }
  else
  {
    qWarning() << "Invalid max amplitude found in AudioInfo::writeData(...)";
    m_level = 0.0;
  }

  emit update();
  return data_size;
}
#endif // BEEBEEP_USE_VOICE_CHAT
