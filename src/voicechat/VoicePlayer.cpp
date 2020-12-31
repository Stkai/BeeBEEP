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

#include "AudioManager.h"
#include "VoicePlayer.h"


VoicePlayer::VoicePlayer( QObject* parent )
 : QObject( parent ), m_chatId( ID_INVALID ), m_currentFilePath(), mp_voicePlayer( Q_NULLPTR )
{
  if( AudioManager::instance().isAudioDeviceAvailable() )
  {
    mp_voicePlayer = new QMediaPlayer( this );
    mp_voicePlayer->setAudioRole( QAudio::VoiceCommunicationRole );
    connect( mp_voicePlayer, SIGNAL( error( QMediaPlayer::Error ) ), this, SLOT( onError( QMediaPlayer::Error ) ) );
    connect( mp_voicePlayer, SIGNAL( durationChanged( qint64 ) ), this, SLOT( onDurationChanged( qint64 ) ) );
    connect( mp_voicePlayer, SIGNAL( positionChanged( qint64 ) ), this, SLOT( onPositionChanged( qint64 ) ) );
  }
  else
    qWarning() << "VoicePlayer is disabled because system audio device is not available";
}

void VoicePlayer::onError( QMediaPlayer::Error error_code )
{
  qWarning() << "VoicePlayer is stopped due the error" << static_cast<int>(error_code) << "-" << qPrintable( mp_voicePlayer->errorString() );
  if( error_code == QMediaPlayer::ResourceError || error_code == QMediaPlayer::FormatError )
  {
    qDebug() << "VoicePlayer tries to use external player to read file" << qPrintable( m_currentFilePath );
    QUrl file_url = QUrl::fromLocalFile( m_currentFilePath );
    emit openWithExternalPlayer( file_url, m_chatId );
  }
  stop();
}

bool VoicePlayer::playFile( const QString& file_path, VNumber chat_id, qint64 file_starting_position )
{
  if( isPlaying() )
    stop();

  if( !canPlay() )
    return false;

  if( !QFile::exists( file_path ) )
  {
    qWarning() << "VoicePlayer cannot play" << qPrintable( file_path ) << "(file not found)";
    return false;
  }

  m_chatId = chat_id;
  m_currentFilePath = file_path;
  emit playing( m_currentFilePath, m_chatId );
  QMediaContent media_content( QUrl::fromLocalFile( m_currentFilePath ) );
  mp_voicePlayer->setMedia( media_content );
  if( file_starting_position > 0 )
    mp_voicePlayer->setPosition( file_starting_position );
  mp_voicePlayer->play();
  return true;
}

void VoicePlayer::stop()
{
  if( mp_voicePlayer && !isStopped() )
    mp_voicePlayer->stop();
  if( !m_currentFilePath.isEmpty() )
    emit finished( m_currentFilePath, m_chatId );
  m_chatId = ID_INVALID;
  m_currentFilePath = "";
}

void VoicePlayer::onDurationChanged( qint64 new_duration )
{
  emit durationChanged( m_currentFilePath, m_chatId, new_duration );
}

void VoicePlayer::onPositionChanged( qint64 new_position )
{
  emit positionChanged( m_currentFilePath, m_chatId, new_position );
}
