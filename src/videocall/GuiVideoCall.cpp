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

#include "GuiVideoCall.h"
#include "Settings.h"


GuiVideoCall::GuiVideoCall( QWidget* parent )
  : QMainWindow( parent ), m_isVideoCallActive( false )
{
}


void GuiVideoCall::startVideoCall()
{
  m_isVideoCallActive = true;
  emit videoCallStarted();
}

void GuiVideoCall::closeVideoCall()
{
  m_isVideoCallActive = false;
  emit videoCallClosed();
}


void GuiVideoCall::closeEvent( QCloseEvent* e )
{
  if( isVideoCallActive() )
  {
    if( QMessageBox::question( this, Settings::instance().programName(), tr("Do you want to end the video call?"),
                               tr( "Yes" ), tr( "No" ), QString::null, 1, 1 ) == 1 )
    {
      e->ignore();
      return;
    }
  }

  closeVideoCall();
  QMainWindow::closeEvent( e );
  e->accept();
}

#if 0


class GuiRecordVoiceMessage : public QDialog, private Ui::GuiRecordVoiceMessageDialog
{
  Q_OBJECT

public:
  explicit GuiRecordVoiceMessage( QWidget *parent = Q_NULLPTR );

  void setRecipient( const QString& );

protected slots:
  void sendVoiceMessage();
  void togglePause();
  void toggleRecord();
  void updateAudioLevel();
  void onVolumeChanged( int );
  void onStateChanged( QAudio::State );

protected:
  void initAudio();
  void createAudioInput();

private:
  QAudioDeviceInfo m_audioDevice;
  QAudioFormat m_audioFormat;
  AudioInfo* mp_audioInfo;
  QAudioInput* mp_audioInput;
  QIODevice *mp_input;

};

GuiRecordVoiceMessage::GuiRecordVoiceMessage( QWidget *parent )
  : QDialog( parent ), m_audioDevice(), m_audioFormat(), mp_audioInfo( Q_NULLPTR ),
    mp_audioInput( Q_NULLPTR ), mp_input( Q_NULLPTR )
{
  setupUi( this );
  setWindowTitle( tr( "Record your voice message" ) + QString( " - %1" ).arg( Settings::instance().programName() ) );
  setWindowIcon( IconManager::instance().icon( "microphone.png" ) );
  Bee::removeContextHelpButton( this );

  initAudio();

  connect( mp_pbRecord, SIGNAL( clicked() ), this, SLOT( toggleRecord() ) );
  connect( mp_pbPause, SIGNAL( clicked() ), this, SLOT( togglePause() ) );
  connect( mp_pbSend, SIGNAL( clicked() ), this, SLOT( sendVoiceMessage() ) );
  connect( mp_pbCancel, SIGNAL( clicked() ), this, SLOT( reject() ) );

}

void GuiRecordVoiceMessage::setRecipient( const QString& new_value )
{
  mp_leTo->setText( new_value );
}

void GuiRecordVoiceMessage::sendVoiceMessage()
{
  accept();
}


void GuiRecordVoiceMessage::initAudio()
{
  m_audioFormat = AudioManager::defaultAudioFormat();
  m_audioDevice = AudioManager::defaultInputDevice();

  if( mp_audioInfo )
  {
    mp_audioInfo->disconnect( this );
    delete mp_audioInfo;
  }

  mp_audioInfo = new AudioInfo( m_audioFormat, this );
  connect( mp_audioInfo, SIGNAL( update() ), this, SLOT( updateAudioLevel() ) );

  createAudioInput();
}

void GuiRecordVoiceMessage::createAudioInput()
{
  mp_audioInput = new QAudioInput( m_audioDevice, m_audioFormat, this);
  qreal initial_volume = QAudio::convertVolume( mp_audioInput->volume(),
                                                QAudio::LinearVolumeScale,
                                                QAudio::LogarithmicVolumeScale );
  mp_sliderVolume->setValue( qRound( initial_volume * 100 ) );
}

void AudioRecorder::toggleRecord()
{
    if (audioRecorder->state() == QMediaRecorder::StoppedState) {
        audioRecorder->setAudioInput(boxValue(ui->audioDeviceBox).toString());

        QAudioEncoderSettings settings;
        settings.setCodec(boxValue(ui->audioCodecBox).toString());
        settings.setSampleRate(boxValue(ui->sampleRateBox).toInt());
        settings.setBitRate(boxValue(ui->bitrateBox).toInt());
        settings.setChannelCount(boxValue(ui->channelsBox).toInt());
        settings.setQuality(QMultimedia::EncodingQuality(ui->qualitySlider->value()));
        settings.setEncodingMode(ui->constantQualityRadioButton->isChecked() ?
                                 QMultimedia::ConstantQualityEncoding :
                                 QMultimedia::ConstantBitRateEncoding);

        QString container = boxValue(ui->containerBox).toString();

        audioRecorder->setEncodingSettings(settings, QVideoEncoderSettings(), container);
        audioRecorder->record();
    }
    else {
        audioRecorder->stop();
    }
}

void AudioRecorder::togglePause()
{
    if (audioRecorder->state() != QMediaRecorder::PausedState)
        audioRecorder->pause();
    else
        audioRecorder->record();
}


void GuiRecordVoiceMessage::updateAudioLevel()
{
  if( mp_audioInfo )
    mp_wAudioLevel->setLevel( mp_audioInfo->level() );
  else
    mp_wAudioLevel->setLevel( 0.0 );
}

void GuiRecordVoiceMessage::onStateChanged( QAudio::State audio_state )
{
  switch( audio_state )
  {
  case QAudio::ActiveState:
    mp_lStatus->setText( "Recording" );
    mp_pbRecord->setEnabled( false );
    mp_pbPause->setEnabled( true );
    break;
  case QAudio::SuspendedState:
    mp_lStatus->setText( "Paused" );
    break;
  case QAudio::StoppedState:
    mp_lStatus->setText( "Stopped" );
    mp_pbRecord->setEnabled( true );
    mp_pbPause->setEnabled( false );
    break;
  case QAudio::IdleState:
    mp_lStatus->setText( "Waiting" );
    break;
#if QT_VERSION >= 0x051000
  case QAudio::InterruptedState:
    mp_lStatus->setText( "Interrupted" );
    break;
#endif
  default:
    mp_lStatus->setText( "Ready" );
    break;
  }
}

void GuiRecordVoiceMessage::togglePause()
{
  if( mp_audioInput->state() == QAudio::ActiveState )
  {
    mp_audioInput->suspend();
    mp_pbPause->setText( tr( "Resume" ) );
  }
  else if( mp_audioInput->state() == QAudio::SuspendedState || mp_audioInput->state() == QAudio::StoppedState )
  {
    mp_audioInput->resume();
    mp_pbPause->setText( tr( "Pause" ) );
  }
  else
  {
    // do nothing
  }
}

void GuiRecordVoiceMessage::toggleRecord()
{
  mp_audioInfo->start();
    mp_audioInput->start(m_audioInfo);
}

void GuiRecordVoiceMessage::onVolumeChanged( int new_value )
{
  if( mp_audioInput )
  {
    qreal linear_volume = QAudio::convertVolume( new_value / qreal( 100 ),
                                                 QAudio::LogarithmicVolumeScale,
                                                 QAudio::LinearVolumeScale);
    mp_audioInput->setVolume( linear_volume );
  }
}

#endif
