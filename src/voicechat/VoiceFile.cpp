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
#include "VoiceFile.h"
#include "VoiceDecoder.h"
#include "VoiceEncoder.h"


VoiceFile::VoiceFile( QObject* parent )
 : QFile( parent ), mp_decoder( Q_NULLPTR ), mp_encoder( Q_NULLPTR ),
   m_info(), m_audioFormat( AudioManager::instance().defaultAudioFormat() )
{
}

VoiceFile::VoiceFile( const QString& file_path, QObject* parent )
 : QFile( file_path, parent ), mp_decoder( Q_NULLPTR ), mp_encoder( Q_NULLPTR ),
   m_info(), m_audioFormat( AudioManager::instance().defaultAudioFormat() )
{
}

VoiceFile::~VoiceFile()
{
  if( mp_decoder )
  {
    delete mp_decoder;
    mp_decoder = Q_NULLPTR;
  }

  if( mp_encoder )
  {
    delete mp_encoder;
    mp_encoder = Q_NULLPTR;
  }
}

bool VoiceFile::open( QFile::OpenMode om )
{
  if( om.testFlag( QFile::ReadOnly ) )
  {
    if( !mp_decoder )
    {
      mp_decoder = new VoiceDecoder();
      mp_decoder->init( m_audioFormat.sampleRate(), m_audioFormat.channelCount() );
      m_info.setAudioFormat( m_audioFormat );
    }
    return QFile::open( QFile::ReadOnly );
  }
  else if( om.testFlag( QFile::WriteOnly ) )
  {
    if( !mp_encoder )
    {
      mp_encoder = new VoiceEncoder();
      mp_encoder->init( m_audioFormat.sampleRate(), m_audioFormat.channelCount() );
      m_info.setAudioFormat( m_audioFormat );
    }
    return QFile::open( QFile::WriteOnly | QFile::Truncate );
  }
  else
  {
    qWarning() << "VoiceFile: invalid open mode found (only read or write accepted)";
    return false;
  }
}

void VoiceFile::close()
{
  if( mp_decoder )
  {
    delete mp_decoder;
    mp_decoder = Q_NULLPTR;
  }

  if( mp_encoder )
  {
    delete mp_encoder;
    mp_encoder = Q_NULLPTR;
  }
}

qint64 VoiceFile::readData( char* data, qint64 data_size )
{
  qint64 data_size_read = QFile::read( data, data_size );
  if( data_size_read > 0 )
  {
    QByteArray encoded_data( data, data_size );
    QByteArray pcm_buffer = mp_decoder->decode( encoded_data );
    data = pcm_buffer.data();
    return pcm_buffer.size();
  }
  else
    return data_size_read;
}

qint64 VoiceFile::writeData( const char* data, qint64 data_size )
{
  qreal audio_level = m_info.processData( data, data_size );
  if( audio_level >= 0 )
    emit levelChanged( audio_level );
  QByteArray pcm_buffer( data, data_size );
  QByteArray opus_buffer = mp_encoder->encode( pcm_buffer );
  if( !opus_buffer.isEmpty() )
    return QFile::writeData( opus_buffer.constData(), opus_buffer.size() );
  else
    return 0;
}
