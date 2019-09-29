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

#ifndef BEEBEEP_GUIRECORDVOICEMESSAGE_H
#define BEEBEEP_GUIRECORDVOICEMESSAGE_H

#include "ui_GuiRecordVoiceMessage.h"
#include "VoiceRecorder.h"
class GuiAudioLevel;


class GuiRecordVoiceMessage : public QDialog, private Ui::GuiRecordVoiceMessageDialog
{
  Q_OBJECT

public:
  explicit GuiRecordVoiceMessage( QWidget *parent = Q_NULLPTR );

  void setRecipient( const QString& );
  inline const QString& filePath() const;

protected slots:
  void showRecorderError( VoiceRecorder::Error );
  void onRecorderStateChanged( VoiceRecorder::State );
  void sendVoiceMessage();
  void toggleRecord();
  void processAudioBuffer( const QAudioBuffer& );
  void updateRecorderProgress( qint64 );

protected:
  void clearAudioLevels();
  void computeWarningDurantion();
  void closeEvent( QCloseEvent* );

private:
  VoiceRecorder* mp_voiceRecorder;
  QAudioProbe* mp_audioProbe;
  QList<GuiAudioLevel*> m_audioLevels;
  QString m_filePath;

  int m_warningDuration;
  QColor m_defaultProgressBarColor;

};

// Inline Functions
inline const QString& GuiRecordVoiceMessage::filePath() const { return m_filePath; }

#endif // BEEBEEP_GUIRECORDVOICEMESSAGE_H
