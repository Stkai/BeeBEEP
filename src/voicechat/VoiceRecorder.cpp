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
#include "BeeUtils.h"
#include "Settings.h"
#include "VoiceRecorder.h"

const int VOICE_RECORDER_ELAPSED_TIME_INTERVAL = 1000;

VoiceRecorder::VoiceRecorder( const QString& file_path, QObject* parent )
 : QObject( parent ), mp_audioInput( Q_NULLPTR ), m_filePath( file_path ), mp_file( Q_NULLPTR ),
   m_error( NoError ), m_errorString( "" ), m_state( StoppedState ),  m_duration( 0 ),
   m_audioFormat()
{
}

VoiceRecorder::~VoiceRecorder()
{
  closeAudioInput();
  closeOutputFile();
}

bool VoiceRecorder::initAudioInput()
{
  if( mp_audioInput )
    return false;
  m_audioFormat = AudioManager::instance().defaultAudioFormat();
  mp_audioInput = new QAudioInput( AudioManager::instance().defaultInputDevice(), m_audioFormat, this );
#ifdef BEEBEEP_DEBUG
  qDebug() << "VoiceRecorder use audio input device:" <<  AudioManager::instance().defaultInputDevice().deviceName();
#endif
  mp_audioInput->setNotifyInterval( VOICE_RECORDER_ELAPSED_TIME_INTERVAL );
  connect( mp_audioInput, SIGNAL( stateChanged( QAudio::State ) ), this, SLOT( onAudioInputStateChanged( QAudio::State ) ) );
  connect( mp_audioInput, SIGNAL( notify() ), this, SLOT( updateTimeElapsed() ) );
  return true;
}

void VoiceRecorder::closeAudioInput()
{
  if( mp_audioInput )
  {
    disconnect( mp_audioInput, SIGNAL( stateChanged( QAudio::State ) ), this, SLOT( onAudioInputStateChanged( QAudio::State ) ) );
    disconnect( mp_audioInput, SIGNAL( notify() ), this, SLOT( updateTimeElapsed() ) );
    if( mp_audioInput->state() != QAudio::StoppedState )
      mp_audioInput->stop();
    mp_audioInput->deleteLater();
    mp_audioInput = Q_NULLPTR;
  }
}

bool VoiceRecorder::initOutputFile()
{
  if( mp_file )
    return false;
  mp_file = new VoiceFile( m_filePath, this );
  //mp_file->setAudioFormat( AudioManager::instance().defaultAudioFormat() ); // it is the default one
  if( !mp_file->open( QFile::WriteOnly ) )
  {
    qWarning() << "VoiceRecorder cannot save audio to file" << qPrintable( m_filePath );
    mp_file->deleteLater();
    mp_file = Q_NULLPTR;
    setError( ResourceError, tr( "Unable to save audio to file %1" ).arg( m_filePath ) );
    return false;
  }
  connect( mp_file, SIGNAL( levelChanged( qreal ) ), this, SIGNAL( levelChanged( qreal ) ) );
  return true;
}

void VoiceRecorder::closeOutputFile()
{
  if( mp_file )
  {
    disconnect( mp_file, SIGNAL( levelChanged( qreal ) ), this, SIGNAL( levelChanged( qreal ) ) );
    if( mp_file->isOpen() )
    {
      mp_file->flush();
      mp_file->close();
    }

    if( m_error != NoError )
    {
      if( !mp_file->remove() )
        qWarning() << "VoiceRecorder cannot remove invalid file:" << qPrintable( m_filePath );
#ifdef BEEBEEP_DEBUG
      else
        qDebug() << "VoiceRecorder removed invalid file:" << qPrintable( m_filePath );
#endif
    }
    mp_file->deleteLater();
    mp_file = Q_NULLPTR;
  }
}

void VoiceRecorder::record()
{
  if( !mp_audioInput )
  {
#ifdef BEEBEEP_DEBUG
     qDebug() << "VoiceRecorder: starting record";
#endif
    m_duration = 0;
    if( initAudioInput() && initOutputFile() )
    {
#ifdef BEEBEEP_DEBUG
      qDebug() << "VoiceRecorder: starting audio input";
#endif
      mp_audioInput->start( mp_file );
    }
  }
  else
  {
#ifdef BEEBEEP_DEBUG
    qDebug() << "VoiceRecorder: resuming record...";
#endif
    mp_audioInput->resume();
  }
}

void VoiceRecorder::pause()
{
  if( !mp_audioInput )
    return;
#ifdef BEEBEEP_DEBUG
    qDebug() << "VoiceRecorder: suspend record...";
#endif
  mp_audioInput->suspend();
}

void VoiceRecorder::stop()
{
#ifdef BEEBEEP_DEBUG
    qDebug() << "VoiceRecorder: stop record...";
#endif
  closeAudioInput();
  closeOutputFile();
}

int VoiceRecorder::volume() const
{
  if( mp_audioInput )
  {
    qreal current_volume = QAudio::convertVolume( mp_audioInput->volume(), QAudio::LinearVolumeScale, QAudio::LogarithmicVolumeScale );
    return qRound( current_volume * 100 );
  }
  else
    return 50;
}

bool VoiceRecorder::setVolume( int new_value )
{
  if( mp_audioInput )
  {
    qreal linear_volume = QAudio::convertVolume( new_value / qreal( 100.0 ), QAudio::LogarithmicVolumeScale, QAudio::LinearVolumeScale );
    mp_audioInput->setVolume( linear_volume );
    return true;
  }
  else
    return false;
}

void VoiceRecorder::setError( VoiceRecorder::Error err_code, const QString& err_string )
{
  m_error = err_code;
  if( m_error != NoError )
  {
    m_errorString = err_string;
    emit error( m_error );
  }
  else
    m_errorString = "";
}

void VoiceRecorder::onAudioInputStateChanged( QAudio::State new_state )
{
  VoiceRecorder::State current_state = m_state;
  switch( new_state )
  {
  case QAudio::ActiveState:
    m_state = RecordingState;
    break;
  case QAudio::SuspendedState:
    m_state = PausedState;
    break;
  case QAudio::StoppedState:
    m_state = StoppedState;
    break;
  case QAudio::IdleState:
    m_state = IdleState;
    break;
  default:
    break;
  }
  if( current_state != m_state )
    emit stateChanged( m_state );
}

void VoiceRecorder::onAudioInputError( QAudio::Error new_error )
{
  switch( new_error )
  {
  case QAudio::NoError:
    m_error = NoError;
    m_errorString = "";
    break;
  case QAudio::OpenError:
    setError( ResourceError, tr( "Cannot open audio input device" ) );
    break;
  case QAudio::IOError:
   setError( ResourceError, tr( "Cannot read from audio input device" ) );
    break;
  case QAudio::UnderrunError:
    setError( ResourceError, tr( "Audio data is not being fed to the audio device at a fast enough rate" ) );
    break;
  case QAudio::FatalError:
    setError( ResourceError, tr( "Internal error" ) );
    break;
  default:
    setError( ResourceError, tr( "Unknown error" ) );
    break;
  }
}

void VoiceRecorder::updateTimeElapsed()
{
  m_duration += VOICE_RECORDER_ELAPSED_TIME_INTERVAL;
  emit durationChanged( m_duration );
}
