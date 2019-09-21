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

#ifndef BEEBEEP_VOICEPLAYER_H
#define BEEBEEP_VOICEPLAYER_H

#include "Config.h"


class VoicePlayer : public QObject
{
  Q_OBJECT
public:
  VoicePlayer( QObject* parent = Q_NULLPTR );

  inline bool isPlaying() const;
  inline bool isStopped() const;
  inline const QString& currentFilePath() const;

  inline bool canPlay() const;
  bool playFile( const QString& );
  void stop();

signals:
  void playing( const QString& );
  void finished( const QString& );

protected slots:
  void onError( QMediaPlayer::Error );


private:
  QString m_currentFilePath;
  QMediaPlayer* mp_voicePlayer;

};

// Inline Functions
inline const QString& VoicePlayer::currentFilePath() const { return m_currentFilePath; }
inline bool VoicePlayer::canPlay() const { return mp_voicePlayer; }
inline bool VoicePlayer::isPlaying() const { return mp_voicePlayer && mp_voicePlayer->state() == QMediaPlayer::PlayingState; }
inline bool VoicePlayer::isStopped() const { return mp_voicePlayer && mp_voicePlayer->state() == QMediaPlayer::StoppedState; }
#endif // BEEBEEP_VOICEPLAYER_H
