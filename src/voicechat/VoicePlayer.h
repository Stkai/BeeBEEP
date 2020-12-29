//////////////////////////////////////////////////////////////////////
//
// BeeBEEP Copyright (C) 2010-2021 Marco Mastroddi
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
// along with BeeBEEP. If not, see <http://www.gnu.org/licenses/>.
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
  inline VNumber chatId() const;

  inline bool canPlay() const;
  bool playFile( const QString&, VNumber );
  void setPosition( qint64 );
  void stop();

signals:
  void playing( const QString&, VNumber );
  void finished( const QString&, VNumber );
  void durationChanged( const QString&, VNumber, qint64 );
  void positionChanged( const QString&, VNumber, qint64 );
  void openWithExternalPlayer( const QUrl&, VNumber );

protected slots:
  void onError( QMediaPlayer::Error );
  void onDurationChanged( qint64 );
  void onPositionChanged( qint64 );

private:
  VNumber m_chatId;
  QString m_currentFilePath;
  QMediaPlayer* mp_voicePlayer;

};

// Inline Functions
inline VNumber VoicePlayer::chatId() const { return m_chatId; }
inline const QString& VoicePlayer::currentFilePath() const { return m_currentFilePath; }
inline bool VoicePlayer::canPlay() const { return mp_voicePlayer; }
inline bool VoicePlayer::isPlaying() const { return mp_voicePlayer && mp_voicePlayer->state() == QMediaPlayer::PlayingState; }
inline bool VoicePlayer::isStopped() const { return mp_voicePlayer && mp_voicePlayer->state() == QMediaPlayer::StoppedState; }
#endif // BEEBEEP_VOICEPLAYER_H
