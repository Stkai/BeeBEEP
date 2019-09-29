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

#include "VoiceCodec.h"
// BeeBEEP uses OPUS Codec

VoiceCodec::VoiceCodec()
 : m_sampleRate( 8000 ), m_channels( 1 ), m_validFrameSize(), m_samples( 0 ), m_sampleBuffer()
{
  // Opus only supports fixed frame durations from 2.5ms to 60ms.
  //
  // NOTE: https://mf4.xiph.org/jenkins/view/opus/job/opus/ws/doc/html/group__opus__encoder.html
  m_validFrameSize << 2.5e-3 << 5e-3 << 10e-3 << 20e-3 << 40e-3 << 60e-3;
  // nSamples = t * sampleRate
  for( int i = 0; i < m_validFrameSize.size(); i++ )
    m_validFrameSize[ i ] *= m_sampleRate;
  // Maxmimum number of samples for the audio buffer.
  m_samples = m_validFrameSize.last();
}

VoiceCodec::~VoiceCodec()
{
}

bool VoiceCodec::init( int sample_rate, int channels )
{
  m_sampleRate = sample_rate;
  m_channels = channels;
  return true;
}

QString VoiceCodec::fileExtension()
{
  return QLatin1String( "opus" );
}
