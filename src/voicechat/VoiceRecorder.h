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

#ifndef BEEBEEP_VOICERECORDER_H
#define BEEBEEP_VOICERECORDER_H

#include "VoiceEncoder.h"
#include "VoiceFile.h"

class VoiceRecorder : public QObject
{
  Q_OBJECT
public:
  enum Error { NoError, ResourceError, FormatError, OutOfSpaceError };
  enum State { StoppedState, RecordingState, PausedState, IdleState, NumStates };

  VoiceRecorder( const QString& file_path, QObject* parent = Q_NULLPTR );
  ~VoiceRecorder();

  inline const QString& filePath() const;
  inline Error error() const;
  inline const QString& errorString() const;
  inline State state() const;

  bool setVolume( int );
  int volume() const;

signals:
  void error( VoiceRecorder::Error );
  void stateChanged( VoiceRecorder::State );
  void levelChanged( qreal );
  void durationChanged( qint64 );

public slots:
  void record();
  void pause();
  void stop();

protected slots:
  void onAudioInputStateChanged( QAudio::State );
  void onAudioInputError( QAudio::Error );
  void updateTimeElapsed();

protected:
  bool initAudioInput();
  void closeAudioInput();
  bool initOutputFile();
  void closeOutputFile();
  void setError( VoiceRecorder::Error, const QString& );

private:
  QAudioInput* mp_audioInput;
  QString m_filePath;
  VoiceFile* mp_file;
  Error m_error;
  QString m_errorString;
  State m_state;
  qint64 m_duration;
  QAudioFormat m_audioFormat;

};

// Inline Functions
inline const QString& VoiceRecorder::filePath() const { return m_filePath; }
inline VoiceRecorder::Error VoiceRecorder::error() const { return m_error; }
inline const QString& VoiceRecorder::errorString() const { return m_errorString; }
inline VoiceRecorder::State VoiceRecorder::state() const { return m_state; }

#endif // BEEBEEP_VOICERECORDER_H
