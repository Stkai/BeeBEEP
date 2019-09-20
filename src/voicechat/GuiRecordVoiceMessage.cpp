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

#include "AudioInfo.h"
#include "AudioManager.h"
#include "GuiAudioLevel.h"
#include "GuiRecordVoiceMessage.h"
#include "IconManager.h"
#include "Settings.h"
#include "BeeUtils.h"


GuiRecordVoiceMessage::GuiRecordVoiceMessage( QWidget *parent )
  : QDialog( parent )
{
  setupUi( this );
  setObjectName( "GuiRecordVoiceMessage" );
  setWindowTitle( tr( "Record your voice message" ) + QString( " - %1" ).arg( Settings::instance().programName() ) );
  setWindowIcon( IconManager::instance().icon( "microphone.png" ) );
  Bee::removeContextHelpButton( this );

  mp_audioRecorder = new QAudioRecorder( this );
  mp_audioProbe = new QAudioProbe( this );
  connect( mp_audioProbe, SIGNAL( audioBufferProbed( const QAudioBuffer& ) ), this, SLOT( processAudioBuffer( const QAudioBuffer& ) ) );
  mp_audioProbe->setSource( mp_audioRecorder );

  QString file_path_tmp = Bee::convertToNativeFolderSeparator( QString( "%1/%2" ).arg( Settings::instance().cacheFolder() ).arg( AudioManager::createDefaultAudioContainerFilename() ) );
  m_filePath = Bee::uniqueFilePath( file_path_tmp, false );
  if( !mp_audioRecorder->setOutputLocation( QUrl::fromLocalFile( m_filePath ) ) )
    qWarning() << "QAudioRecorder is unable to set output location to" << qPrintable( m_filePath );

  mp_progressBar->setMinimum( 0 );
  mp_lStatus->setText( tr( "Click to start recording your message" ) );
  mp_pbSend->setEnabled( false );
  m_warningDuration = 80;

  connect( mp_audioRecorder, SIGNAL( durationChanged( qint64 ) ), this, SLOT( updateRecorderProgress( qint64 ) ) );
  connect( mp_audioRecorder, SIGNAL( statusChanged( QMediaRecorder::Status ) ), this, SLOT( onRecorderStatusChanged( QMediaRecorder::Status ) ) );
  connect( mp_audioRecorder, SIGNAL( stateChanged( QMediaRecorder::State ) ), this, SLOT( onRecorderStateChanged( QMediaRecorder::State ) ) );
  connect( mp_audioRecorder, SIGNAL( error( QMediaRecorder::Error ) ), this,  SLOT( showRecorderError( QMediaRecorder::Error ) ) );
  connect( mp_pbRecord, SIGNAL( clicked() ), this, SLOT( toggleRecord() ) );
  connect( mp_pbSend, SIGNAL( clicked() ), this, SLOT( sendVoiceMessage() ) );
  connect( mp_pbCancel, SIGNAL( clicked() ), this, SLOT( reject() ) );
}

void GuiRecordVoiceMessage::setRecipient( const QString& new_value )
{
  mp_leTo->setText( new_value );
}

void GuiRecordVoiceMessage::closeEvent( QCloseEvent* e )
{
  if( mp_audioRecorder->state() == QMediaRecorder::RecordingState || mp_audioRecorder->state() == QMediaRecorder::PausedState )
    mp_audioRecorder->stop();
  e->accept();
}

void GuiRecordVoiceMessage::sendVoiceMessage()
{
  if( mp_audioRecorder->state() == QMediaRecorder::RecordingState || mp_audioRecorder->state() == QMediaRecorder::PausedState )
    mp_audioRecorder->stop();
  QFileInfo file_info( m_filePath );
  if( file_info.exists())
  {
    if( file_info.size() > 0 )
    {
      qDebug() << "Recorded voice message in file" << qPrintable( m_filePath );
      accept();
    }
    else
      reject();
  }
  else
    reject();
}

void GuiRecordVoiceMessage::updateRecorderProgress( qint64 duration_ms )
{
  if( mp_audioRecorder->error() != QMediaRecorder::NoError )
    return;

  if( duration_ms < 1000 )
  {
    mp_progressBar->setValue( 1 );
    return;
  }

  int duration_s = static_cast<int>( duration_ms / 1000.0 );
  mp_progressBar->setValue( duration_s );
  if( duration_s >= Settings::instance().voiceMessageMaxDuration() )
  {
    QPalette p = mp_progressBar->palette();
    p.setColor( QPalette::Highlight, Qt::darkRed );
    mp_progressBar->setPalette( p );
    mp_audioRecorder->stop();
    mp_lStatus->setText( tr( "Stopped. Reached the maximum duration of the voice message" ) );
    mp_pbRecord->setEnabled( false );
  }
  else if( duration_s > m_warningDuration )
  {
    if( !m_defaultProgressBarColor.isValid() )
    {
      QPalette p = mp_progressBar->palette();
      m_defaultProgressBarColor = p.color( QPalette::Highlight );
      p.setColor( QPalette::Highlight, Qt::red );
      mp_progressBar->setPalette( p );
    }
  }
  else
  {
    if( m_defaultProgressBarColor.isValid() )
    {
      QPalette p = mp_progressBar->palette();
      p.setColor( QPalette::Highlight, m_defaultProgressBarColor );
      mp_progressBar->setPalette( p );
      m_defaultProgressBarColor = QColor();
    }
  }
}

void GuiRecordVoiceMessage::onRecorderStatusChanged( QMediaRecorder::Status recorder_status )
{
  QString status_message = "";
  switch( recorder_status )
  {
  case QMediaRecorder::RecordingStatus:
    status_message = tr( "Recording" );
    break;
  case QMediaRecorder::PausedStatus:
    clearAudioLevels();
    status_message = tr( "Paused" );
    break;
  case QMediaRecorder::UnloadedStatus:
    clearAudioLevels();
    status_message = tr( "Ready" );
    break;
  case QMediaRecorder::LoadedStatus:
    clearAudioLevels();
    break;
  default:
    break;
  }

  if( mp_audioRecorder->error() == QMediaRecorder::NoError && !status_message.isEmpty() )
    mp_lStatus->setText( status_message );
}

void GuiRecordVoiceMessage::computeWarningDurantion()
{
  m_warningDuration = qMax( 1, qMin( 60, Settings::instance().voiceMessageMaxDuration() - qMax( 1, Settings::instance().voiceMessageMaxDuration() / 5 ) ) );
}

void GuiRecordVoiceMessage::onRecorderStateChanged( QMediaRecorder::State recorder_state )
{
  switch( recorder_state )
  {
  case QMediaRecorder::RecordingState:
    mp_pbRecord->setText( tr( "Pause" ) );
    mp_pbRecord->setIcon( IconManager::instance().icon( "pause.png" ) );
    mp_progressBar->setMaximum( Settings::instance().voiceMessageMaxDuration() );
    computeWarningDurantion();
    break;
  case QMediaRecorder::PausedState:
    mp_pbRecord->setText( tr( "Resume" ) );
    mp_pbRecord->setIcon( IconManager::instance().icon( "record.png" ) );
    mp_progressBar->setMaximum( Settings::instance().voiceMessageMaxDuration() );
    break;
  case QMediaRecorder::StoppedState:
    mp_pbRecord->setText( tr( "Record" ) );
    mp_pbRecord->setIcon( IconManager::instance().icon( "record.png" ) );
    break;
  }
}

void GuiRecordVoiceMessage::showRecorderError( QMediaRecorder::Error recorder_error )
{
  QString s_error = mp_audioRecorder->errorString();
  qWarning() << "VoiceRecorder exit with error code" << (int)recorder_error << "-" << qPrintable( s_error );
  mp_lStatus->setText( s_error );
  mp_pbSend->setEnabled( false );
}

void GuiRecordVoiceMessage::toggleRecord()
{
  if( mp_audioRecorder->error() != QMediaRecorder::NoError )
  {
    mp_lStatus->setText( tr( "The message could not be recorded because an error has occurred: %1" ).arg( mp_audioRecorder->errorString() ) );
    mp_pbSend->setEnabled( false );
    return;
  }

  if( mp_audioRecorder->state() == QMediaRecorder::StoppedState )
  {
    if( QFile::exists( m_filePath ) )
    {
      if( QMessageBox::question( this, Settings::instance().programName(), tr( "Do you want to start recording your voice message again?"),
                                 tr( "Yes, clean and restart" ), tr( "Cancel" ), QString::null, 1, 1 ) != 0 )
      {
        return;
      }
    }
    mp_lStatus->setText( tr( "Please wait" ) + QString( "..." ) );
    mp_pbSend->setEnabled( true );
    QApplication::setOverrideCursor( Qt::WaitCursor );
    QApplication::processEvents();
    mp_audioRecorder->setEncodingSettings( AudioManager::defaultAudioEncoderSettings(), QVideoEncoderSettings(), AudioManager::defaultAudioContainer() );
    mp_audioRecorder->record();
    QApplication::restoreOverrideCursor();
  }
  else if( mp_audioRecorder->state() != QMediaRecorder::PausedState )
  {
    mp_audioRecorder->pause();
  }
  else
  {
    mp_lStatus->setText( tr( "Please wait" ) + QString( "..." ) );
    mp_audioRecorder->record();
  }
}

static QVector<qreal> GetAudioBufferLevels( const QAudioBuffer& );

void GuiRecordVoiceMessage::processAudioBuffer( const QAudioBuffer& buffer )
{
  if( m_audioLevels.count() != buffer.format().channelCount() )
  {
    qDeleteAll( m_audioLevels );
    m_audioLevels.clear();
    for( int i = 0; i < buffer.format().channelCount(); i++ )
    {
      GuiAudioLevel* gui_level = new GuiAudioLevel( this );
      m_audioLevels.append( gui_level );
      mp_layoutLevels->addWidget( gui_level );
    }
  }

  QVector<qreal> audio_levels = GetAudioBufferLevels( buffer );
  for( int i = 0; i < audio_levels.count(); i++ )
    m_audioLevels.at( i )->setLevel( audio_levels.at( i ) );
}

void GuiRecordVoiceMessage::clearAudioLevels()
{
  for( int i = 0; i < m_audioLevels.size(); i++ )
    m_audioLevels.at( i )->setLevel( 0 );
}


/* Static Functions from Qt AudioRecorder example */
// This function returns the maximum possible sample value for a given audio format
qreal GetAudioPeakValue( const QAudioFormat& format )
{
  // Note: Only the most common sample formats are supported
  if( !format.isValid() )
    return qreal(0);

  if( format.codec() != "audio/pcm" )
    return qreal(0);

  switch( format.sampleType() )
  {
  case QAudioFormat::Unknown:
    break;
  case QAudioFormat::Float:
    {
      if( format.sampleSize() != 32 ) // other sample formats are not supported
        return qreal( 0 );
      return qreal( 1.00003 );
    }
    break;
    case QAudioFormat::SignedInt:
      {
        if( format.sampleSize() == 32 )
            return qreal(INT_MAX);
        if (format.sampleSize() == 16)
            return qreal(SHRT_MAX);
        if (format.sampleSize() == 8)
            return qreal(CHAR_MAX);
      }
      break;
    case QAudioFormat::UnSignedInt:
        if (format.sampleSize() == 32)
            return qreal(UINT_MAX);
        if (format.sampleSize() == 16)
            return qreal(USHRT_MAX);
        if (format.sampleSize() == 8)
            return qreal(UCHAR_MAX);
        break;
    }

    return qreal(0);
}

template <class T>
QVector<qreal> GetAudioBufferLevels( const T* buffer, int frames, int channels )
{
  QVector<qreal> max_values;
  max_values.fill( 0, channels );

  for( int i = 0; i < frames; i++ )
  {
    for( int j = 0; j < channels; j++ )
    {
      qreal value = qAbs( qreal( buffer[ i * channels + j ] ) );
      if( value > max_values.at( j ) )
        max_values.replace( j, value );
    }
  }
  return max_values;
}

// returns the audio level for each channel
QVector<qreal> GetAudioBufferLevels(const QAudioBuffer& buffer)
{
    QVector<qreal> values;

    if (!buffer.format().isValid() || buffer.format().byteOrder() != QAudioFormat::LittleEndian)
        return values;

    if (buffer.format().codec() != "audio/pcm" )
        return values;

    int channelCount = buffer.format().channelCount();
    values.fill(0, channelCount);
    qreal peak_value = GetAudioPeakValue(buffer.format());
    if (qFuzzyCompare(peak_value, qreal(0)))
        return values;

    switch (buffer.format().sampleType()) {
    case QAudioFormat::Unknown:
    case QAudioFormat::UnSignedInt:
        if (buffer.format().sampleSize() == 32)
            values = GetAudioBufferLevels(buffer.constData<quint32>(), buffer.frameCount(), channelCount);
        if (buffer.format().sampleSize() == 16)
            values = GetAudioBufferLevels(buffer.constData<quint16>(), buffer.frameCount(), channelCount);
        if (buffer.format().sampleSize() == 8)
            values = GetAudioBufferLevels(buffer.constData<quint8>(), buffer.frameCount(), channelCount);
        for (int i = 0; i < values.size(); ++i)
            values[i] = qAbs(values.at(i) - peak_value / 2) / (peak_value / 2);
        break;
    case QAudioFormat::Float:
        if (buffer.format().sampleSize() == 32) {
            values = GetAudioBufferLevels(buffer.constData<float>(), buffer.frameCount(), channelCount);
            for (int i = 0; i < values.size(); ++i)
                values[i] /= peak_value;
        }
        break;
    case QAudioFormat::SignedInt:
        if (buffer.format().sampleSize() == 32)
            values = GetAudioBufferLevels(buffer.constData<qint32>(), buffer.frameCount(), channelCount);
        if (buffer.format().sampleSize() == 16)
            values = GetAudioBufferLevels(buffer.constData<qint16>(), buffer.frameCount(), channelCount);
        if (buffer.format().sampleSize() == 8)
            values = GetAudioBufferLevels(buffer.constData<qint8>(), buffer.frameCount(), channelCount);
        for (int i = 0; i < values.size(); ++i)
            values[i] /= peak_value;
        break;
    }

    return values;
}

