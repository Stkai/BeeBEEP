//////////////////////////////////////////////////////////////////////
//
// BeeBEEP Copyright (C) 2010-2020 Marco Mastroddi
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

#ifndef BEEBEEP_GUIRECORDVOICEMESSAGE_H
#define BEEBEEP_GUIRECORDVOICEMESSAGE_H

#include "ui_GuiRecordVoiceMessage.h"
#include "Config.h"
class GuiAudioLevel;


class GuiRecordVoiceMessage : public QDialog, private Ui::GuiRecordVoiceMessageDialog
{
  Q_OBJECT

public:
  explicit GuiRecordVoiceMessage( QWidget *parent = Q_NULLPTR );

  void setRecipient( const QString& );
  inline const QString& filePath() const;
  inline qint64 duration() const;

protected slots:
  void sendVoiceMessage();
  void cancelVoiceMessage();
  void toggleRecord();
  void processAudioBuffer( const QAudioBuffer& );
  void updateRecorderProgress( qint64 );
  void onRecorderStatusChanged( QMediaRecorder::Status );
  void onRecorderStateChanged( QMediaRecorder::State );
  void showRecorderError( QMediaRecorder::Error );
  void onVolumeChanged( int );

protected:
  void clearAudioLevels();
  void computeDurantionRange();
  void closeEvent( QCloseEvent* );
  void checkAndSetFilePath();

private:
  QAudioRecorder* mp_audioRecorder;
  QAudioProbe* mp_audioProbe;
  QList<GuiAudioLevel*> m_audioLevels;
  QString m_filePath;
  bool m_fileAccepted;
  int m_warningDuration;
  int m_criticalDuration;
  int m_maxDuration;
  qint64 m_duration;

};

// Inline Functions
inline const QString& GuiRecordVoiceMessage::filePath() const { return m_filePath; }
inline qint64 GuiRecordVoiceMessage::duration() const { return m_fileAccepted ? qMax( qint64( 1000 ), m_duration ) : -1; }

#endif // BEEBEEP_GUIRECORDVOICEMESSAGE_H
