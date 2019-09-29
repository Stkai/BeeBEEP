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

#include "VoiceEncoder.h"
#include "opus.h"


VoiceEncoder::VoiceEncoder()
 : VoiceCodec()
{
}

VoiceEncoder::~VoiceEncoder()
{
  if( mp_encoder )
  {
    opus_encoder_destroy( mp_encoder );
    mp_encoder = Q_NULLPTR;
  }
}

bool VoiceEncoder::init( int sample_rate, int channels )
{
  VoiceCodec::init( sample_rate, channels );
  int error;
  mp_encoder = opus_encoder_create( m_sampleRate, m_channels, OPUS_APPLICATION_VOIP, &error );
  if( mp_encoder || error == OPUS_OK )
  {
    // Add some options for error correction.
    opus_encoder_ctl( mp_encoder, OPUS_SET_INBAND_FEC(1) );
    opus_encoder_ctl( mp_encoder, OPUS_SET_PACKET_LOSS_PERC(20) );
    opus_encoder_ctl( mp_encoder, OPUS_SET_DTX(1) );
#ifdef OPUS_SET_PREDICTION_DISABLED
    opus_encoder_ctl( mp_encoder, OPUS_SET_PREDICTION_DISABLED(1) );
#endif
    return true;
  }
  else
  {
    qWarning() << "VoiceEncoder initialization error (opus encoder):" << opus_strerror( error );
    return false;
  }
}

int VoiceEncoder::readWindow( int buffer_size )
{
  // WARNING: We are expecting 2 bytes signed samples, but this is wrong since
  // input stream can have a different sample formats.

  // Get the number of frames in the buffer.
  int samples = buffer_size / m_channels / 2;

  // Find an appropriate number of samples to read, according to Opus specs.
  for( int i = m_validFrameSize.size() - 1; i >= 0; i-- )
  {
    if( m_validFrameSize[ i ] <= samples )
      return m_validFrameSize[ i ];
  }
  return 0;
}

QByteArray VoiceEncoder::encode( const QByteArray& pcm_buffer )
{
  if( pcm_buffer.isEmpty() )
    return QByteArray();
  m_sampleBuffer.append( pcm_buffer );
  // Get the maximum number of samples to encode. It must be a number
  // accepted by the Opus encoder
  int samples = readWindow( m_sampleBuffer.size() );
  if( samples < 1 )
    return 0;
  // The encoded stream is supposed to be smaller than the raw stream, so

  QByteArray opus_buffer( m_sampleBuffer.size(), 0 );
  int length = opus_encode( mp_encoder, (opus_int16*)m_sampleBuffer.constData(), samples, (uchar*)opus_buffer.data(), opus_buffer.size() );
  if( length < 1 )
  {
    qWarning() << "VoiceEncoder encoding error (opus_encode):" << opus_strerror( length );
    opus_buffer = QByteArray();
  }
  // Remove the frame from the sample buffer.
  m_sampleBuffer.remove( 0, samples * m_channels * 2 );
  return opus_buffer;
}
