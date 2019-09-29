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


#ifndef BEEBEEP_VOICEDECODER_H
#define BEEBEEP_VOICEDECODER_H

#include "VoiceCodec.h"
typedef struct OpusDecoder OpusDecoder;

class VoiceDecoder : protected VoiceCodec
{
public:
  VoiceDecoder();
  ~VoiceDecoder();

  bool init( int sample_rate, int channels );

  QByteArray decode( const QByteArray& );

private:
  OpusDecoder* mp_decoder;

};

#endif // BEEBEEP_VOICEDECODER_H
