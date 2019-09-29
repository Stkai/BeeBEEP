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

  mp_progressBar->setMinimum( 0 );
  mp_lStatus->setText( tr( "Click to start recording your message" ) );
  mp_pbSend->setEnabled( false );
  m_warningDuration = 80;

  QString file_path_tmp = Bee::convertToNativeFolderSeparator( QString( "%1/%2" ).arg( Settings::instance().cacheFolder() ).arg( AudioManager::instance().createDefaultAudioContainerFilename() ) );
  m_filePath = Bee::uniqueFilePath( file_path_tmp, false );
  mp_voiceRecorder = new VoiceRecorder( m_filePath, this );

  connect( mp_voiceRecorder, SIGNAL( durationChanged( qint64 ) ), this, SLOT( updateRecorderProgress( qint64 ) ) );
  connect( mp_voiceRecorder, SIGNAL( stateChanged( VoiceRecorder::State ) ), this, SLOT( onRecorderStateChanged( VoiceRecorder::State ) ) );
  connect( mp_voiceRecorder, SIGNAL( error( VoiceRecorder::Error ) ), this,  SLOT( showRecorderError( VoiceRecorder::Error ) ) );
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
  if( mp_voiceRecorder->state() == VoiceRecorder::RecordingState || mp_voiceRecorder->state() == VoiceRecorder::PausedState )
    mp_voiceRecorder->stop();
  e->accept();
}

void GuiRecordVoiceMessage::sendVoiceMessage()
{
  if( mp_voiceRecorder->state() == VoiceRecorder::RecordingState || mp_voiceRecorder->state() == VoiceRecorder::PausedState )
    mp_voiceRecorder->stop();
  QFileInfo file_info( m_filePath );
  if( file_info.exists() && file_info.size() > 0 )
  {
    qDebug() << "Recorded voice message in file" << qPrintable( m_filePath );
    accept();
  }
  else
  {
    qWarning() << "File with voice message is empty or not exists:" << qPrintable( m_filePath );
    reject();
  }
}

void GuiRecordVoiceMessage::updateRecorderProgress( qint64 duration_ms )
{
  if( mp_voiceRecorder->error() != VoiceRecorder::NoError )
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
    mp_voiceRecorder->stop();
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

void GuiRecordVoiceMessage::computeWarningDurantion()
{
  m_warningDuration = qMax( 1, qMin( 60, Settings::instance().voiceMessageMaxDuration() - qMax( 1, Settings::instance().voiceMessageMaxDuration() / 5 ) ) );
}

void GuiRecordVoiceMessage::onRecorderStateChanged( VoiceRecorder::State recorder_state )
{
  switch( recorder_state )
  {
  case VoiceRecorder::RecordingState:
    mp_pbRecord->setText( tr( "Pause" ) );
    mp_pbRecord->setIcon( IconManager::instance().icon( "pause.png" ) );
    mp_progressBar->setMaximum( Settings::instance().voiceMessageMaxDuration() );
    computeWarningDurantion();
    break;
  case VoiceRecorder::PausedState:
    mp_pbRecord->setText( tr( "Resume" ) );
    mp_pbRecord->setIcon( IconManager::instance().icon( "record.png" ) );
    mp_progressBar->setMaximum( Settings::instance().voiceMessageMaxDuration() );
    clearAudioLevels();
    mp_lStatus->setText( tr( "Paused" ) );
    break;
  case VoiceRecorder::StoppedState:
    mp_pbRecord->setText( tr( "Record" ) );
    mp_pbRecord->setIcon( IconManager::instance().icon( "record.png" ) );
    clearAudioLevels();
    break;
  case VoiceRecorder::IdleState:
    mp_lStatus->setText( tr( "Start talking" ) + QString( "..." ) );
    break;
  default:
    break;
  }
}

void GuiRecordVoiceMessage::showRecorderError( VoiceRecorder::Error recorder_error )
{
  QString s_error = mp_voiceRecorder->errorString();
  qWarning() << "VoiceRecorder exit with error code" << static_cast<int>(recorder_error) << "-" << qPrintable( s_error );
  mp_lStatus->setText( s_error );
  mp_pbSend->setEnabled( false );
}

void GuiRecordVoiceMessage::toggleRecord()
{
  if( mp_voiceRecorder->error() != VoiceRecorder::NoError )
  {
    mp_lStatus->setText( tr( "The message could not be recorded because an error has occurred: %1" ).arg( mp_voiceRecorder->errorString() ) );
    mp_pbSend->setEnabled( false );
    return;
  }

  if( mp_voiceRecorder->state() == VoiceRecorder::StoppedState )
  {
    if( QFile::exists( m_filePath ) )
    {
      if( QMessageBox::question( this, Settings::instance().programName(), tr( "Do you want to start recording your voice message again?"),
                                 tr( "Yes, clean and restart" ), tr( "Cancel" ), QString(), 1, 1 ) != 0 )
      {
        return;
      }
    }
    mp_lStatus->setText( tr( "Please wait" ) + QString( "..." ) );
    mp_pbSend->setEnabled( true );
    QApplication::setOverrideCursor( Qt::WaitCursor );
    QApplication::processEvents();
    mp_voiceRecorder->record();
    QApplication::restoreOverrideCursor();
  }
  else if( mp_voiceRecorder->state() == VoiceRecorder::IdleState )
  {
    // do nothing
  }
  else if( mp_voiceRecorder->state() != VoiceRecorder::PausedState )
  {
    mp_voiceRecorder->pause();
  }
  else
  {
    mp_voiceRecorder->record();
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
    if( format.sampleSize() == 32 ) // other sample formats are not supported
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
    {
      if( format.sampleSize() == 32)
        return qreal(UINT_MAX);
      if( format.sampleSize() == 16)
        return qreal(USHRT_MAX);
      if( format.sampleSize() == 8)
        return qreal(UCHAR_MAX);
    }
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
