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

#include "VoiceDecoder.h"
#include "opus.h"


VoiceDecoder::VoiceDecoder()
 : VoiceCodec()
{

}

bool VoiceDecoder::init( int sample_rate, int channels )
{
  VoiceCodec::init( sample_rate, channels );
  int error;
  mp_decoder = opus_decoder_create( m_sampleRate, m_channels, &error );
  if( !mp_decoder || error != OPUS_OK )
  {
    qWarning() << "VoiceDecoder initialization error (opus decoder):" << opus_strerror( error );
    return false;
  }
  else
    return true;
}

VoiceDecoder::~VoiceDecoder()
{
  if( mp_decoder )
  {
    opus_decoder_destroy( mp_decoder );
    mp_decoder = Q_NULLPTR;
  }
}

QByteArray VoiceDecoder::decode( const QByteArray& encoded_data )
{
  // Audio frame is nSamples at maximum, so
  QByteArray pcm_buffer( m_samples * m_channels * 2, 0 );
  // The last argumment must be 1 to enable FEC, but I don't why it results
  // in a SIGSEV.
  int samples = opus_decode( mp_decoder, (uchar*)encoded_data.constData(), encoded_data.size(), (opus_int16*)pcm_buffer.data(), pcm_buffer.size(), 0 );
  if( samples < 1 )
  {
    qWarning() << "Opus decoding error:" << opus_strerror(samples);
    return QByteArray();
  }
  if( samples != pcm_buffer.size() )
    pcm_buffer.resize( samples );
  return pcm_buffer;
}

