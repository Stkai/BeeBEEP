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

#ifndef BEEBEEP_VOICECODEC_H
#define BEEBEEP_VOICECODEC_H

#include "Config.h"

class VoiceCodec
{
public:
  VoiceCodec();
  virtual ~VoiceCodec();

  bool init( int sample_rate, int channels );

  static QString fileExtension();

protected:
  int m_sampleRate;
  int m_channels;
  QList<float> m_validFrameSize;
  int m_samples;
  QByteArray m_sampleBuffer;

};

#endif // BEEBEEP_VOICECODEC_H
