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
#ifndef BEEBEEP_AUDIOCODEC_H
#define BEEBEEP_AUDIOCODEC_H

#include "Config.h"
class AudioCodec
{
public:
  virtual ~AudioCodec() {}
  virtual qint64 encode( QDataStream* input, QDataStream* output ) = 0;
  virtual qint64 decode( QDataStream* input, QDataStream* output ) = 0;
};

// G.711 a-law PCM codec
class G711aCodec : public AudioCodec
{
public:
  G711aCodec( int clockrate );
  qint64 encode( QDataStream* input, QDataStream* output ) override;
  qint64 decode( QDataStream* input, QDataStream* output ) override;
private:
  int m_frequency;
};

// G.711 u-law PCM codec
class G711uCodec : public AudioCodec
{
public:
  G711uCodec( int clockrate );
  qint64 encode( QDataStream* input, QDataStream* output ) override;
  qint64 decode( QDataStream* input, QDataStream* output ) override;
private:
  int m_frequency;
};

#ifdef QXMPP_USE_SPEEX
typedef struct SpeexBits SpeexBits;

class SpeexCodec : public AudioCodec
{
public:
  SpeexCodec(int clockrate);
  ~SpeexCodec();

  qint64 encode(QDataStream* input, QDataStream* output );
  qint64 decode(QDataStream* input, QDataStream* output );

private:
  SpeexBits* encoder_bits;
  void* encoder_state;
  SpeexBits* decoder_bits;
  void* decoder_state;
  int frame_samples;
};
#endif

#endif // BEEBEEP_AUDIOCODEC_H
#endif // BEEBEEP_USE_VOICE_CHAT
