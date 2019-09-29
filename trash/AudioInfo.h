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
#ifndef BEEBEEP_AUDIOINFO_H
#define BEEBEEP_AUDIOINFO_H

#include "Config.h"

class AudioInfo : public QIODevice
{
  Q_OBJECT
public:
  AudioInfo( const QAudioFormat&, QObject* parent = Q_NULLPTR );

  void start();
  void stop();

  inline qreal level() const;

  qint64 readData(char *data, qint64 maxlen) override;
  qint64 writeData(const char *data, qint64 len) override;

signals:
  void update();

protected:
  bool checkAudioFormat();

private:
  QAudioFormat m_format;
  quint32 m_maxAmplitude;
  qreal m_level; // 0.0 <= m_level <= 1.0

};


// Inline Functions
inline qreal AudioInfo::level() const { return m_level; }

#endif // BEEBEEP_AUDIOINFO_H
#endif // BEEBEEP_USE_VOICE_CHAT
