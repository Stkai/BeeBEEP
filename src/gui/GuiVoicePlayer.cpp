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

#include "BeeUtils.h"
#include "Core.h"
#include "GuiVoicePlayer.h"
#include "IconManager.h"
#include "Settings.h"
#ifdef BEEBEEP_USE_VOICE_CHAT
#include "VoicePlayer.h"
#endif


GuiVoicePlayer::GuiVoicePlayer( QWidget* parent )
  : QWidget( parent ), m_filePath( "" ), m_chatId( ID_INVALID )
{
  setObjectName( "GuiVoicePlayer" );
  setupUi( this );

  mp_pbPlay->setEnabled( false );
  connect( mp_pbPlay, SIGNAL( clicked() ), this, SLOT( onPlayClicked() ) );
  connect( mp_sliderPosition, SIGNAL( sliderPressed() ), this, SLOT( onSliderPositionPressed() ) );
  connect( mp_sliderPosition, SIGNAL( sliderMoved( int ) ), this, SLOT( onSliderPositionMoved( int ) ) );
  connect( mp_sliderPosition, SIGNAL( sliderReleased() ), this, SLOT( onSliderPositionReleased() ) );
  connect( mp_pbClose, SIGNAL( clicked() ), this, SLOT( onCloseClicked() ) );
}

void GuiVoicePlayer::onCloseClicked()
{
  if( !beeCore->voicePlayer()->isStopped() )
    beeCore->voicePlayer()->stop();
}

void GuiVoicePlayer::setFilePath( const QString& voice_file_path, VNumber chat_id )
{
  m_filePath = voice_file_path;
  m_chatId = chat_id;
  if( mp_pbPlay->isEnabled() )
    mp_pbPlay->setEnabled( false );
#ifdef BEEBEEP_USE_VOICE_CHAT
  mp_sliderPosition->setMinimum( 0 );
  mp_sliderPosition->setMaximum( 1 );
  mp_sliderPosition->setValue( 0 );
  mp_sliderPosition->setEnabled( false );
  mp_lPosition->setText( Bee::timeToString( 0 ) );
  mp_lDuration->setText( Bee::timeToString( 0 ) );
  mp_lVoiceFile->setToolTip( m_filePath );
  connect( beeCore->voicePlayer(), SIGNAL( playing( const QString&, VNumber ) ), this, SLOT( onPlaying( const QString&, VNumber ) ) );
  connect( beeCore->voicePlayer(), SIGNAL( paused( const QString&, VNumber ) ), this, SLOT( onPaused( const QString&, VNumber ) ) );
  connect( beeCore->voicePlayer(), SIGNAL( finished( const QString&, VNumber ) ), this, SLOT( onFinished( const QString&, VNumber ) ) );
  connect( beeCore->voicePlayer(), SIGNAL( durationChanged( const QString&, VNumber, qint64 ) ), this, SLOT( setFileDuration( const QString&, VNumber, qint64 ) ) );
  connect( beeCore->voicePlayer(), SIGNAL( positionChanged( const QString&, VNumber, qint64 ) ), this, SLOT( setFilePosition( const QString&, VNumber, qint64 ) ) );
  if( !beeCore->voicePlayer()->playFile( m_filePath, m_chatId ) )
    QMessageBox::information( qApp->activeWindow(), Settings::instance().programName(), tr( "Unable to open voice message %1" ).arg( m_filePath ), tr( "Ok" ) );
#endif
}

void GuiVoicePlayer::onPlaying( const QString& voice_file_path, VNumber chat_id )
{
  if( m_chatId == chat_id && m_filePath == voice_file_path )
  {
    if( !mp_pbPlay->isEnabled() )
      mp_pbPlay->setEnabled( true );
    setPaused( false );
    if( !isVisible() )
      show();
#ifdef BEEBEEP_DEBUG
    qDebug() << "Voice file" << qPrintable( voice_file_path ) << "-> playing";
#endif
  }
}

void GuiVoicePlayer::onPaused( const QString& voice_file_path, VNumber chat_id )
{
  if( m_chatId == chat_id && m_filePath == voice_file_path )
  {
    setPaused( true );
#ifdef BEEBEEP_DEBUG
    qDebug() << "Voice file" << qPrintable( voice_file_path ) << "-> paused";
#endif
  }
}

void GuiVoicePlayer::onFinished( const QString& voice_file_path, VNumber chat_id )
{
  if( m_chatId == chat_id && m_filePath == voice_file_path )
  {
    setPaused( true );
    if( isVisible() )
      hide();
#ifdef BEEBEEP_DEBUG
    qDebug() << "Voice file" << qPrintable( voice_file_path ) << "-> play finished";
#endif
  }
}

void GuiVoicePlayer::setFileDuration( const QString& voice_file_path, VNumber chat_id, qint64 file_duration )
{
  if( m_chatId == chat_id && m_filePath == voice_file_path )
  {
#ifdef BEEBEEP_DEBUG
    qDebug() << "Voice file" << qPrintable( voice_file_path ) << "has duration" << file_duration << "ms";
#endif
    mp_sliderPosition->setMinimum( 0 );
    mp_sliderPosition->setMaximum( static_cast<int>( file_duration ) );
    mp_sliderPosition->setValue( 0 );
    mp_sliderPosition->setEnabled( true );
    mp_lDuration->setText( QString( "<b>%1</b>" ).arg( Bee::timeToString( file_duration ) ) );
  }
}

void GuiVoicePlayer::setFilePosition( const QString& voice_file_path, VNumber chat_id, qint64 file_position )
{
  if( m_chatId == chat_id && m_filePath == voice_file_path )
  {
#ifdef BEEBEEP_DEBUG
    qDebug() << "Voice file" << qPrintable( voice_file_path ) << "new position" << file_position << "ms";
#endif
    qint64 slider_new_position = file_position > mp_sliderPosition->maximum() ? mp_sliderPosition->maximum() : file_position;
    mp_sliderPosition->setValue( static_cast<int>( slider_new_position ) );
    mp_lPosition->setText( QString( "<i>%1</i>" ).arg( Bee::timeToString( slider_new_position ) ) );
  }
}

void GuiVoicePlayer::setPaused( bool yes )
{
  if( yes )
    mp_pbPlay->setIcon( IconManager::instance().icon( "play.png" ) );
  else
    mp_pbPlay->setIcon( IconManager::instance().icon( "pause.png" ) );
}

void GuiVoicePlayer::onPlayClicked()
{
  if( !beeCore->voicePlayer()->canPlay() )
  {
    mp_pbPlay->setDisabled( true );
    return;
  }

  if( beeCore->voicePlayer()->isPlaying() )
    beeCore->voicePlayer()->pause();
  else
    beeCore->voicePlayer()->playFile( m_filePath, m_chatId, mp_sliderPosition->value() );
}

void GuiVoicePlayer::onSliderPositionPressed()
{
  if( beeCore->voicePlayer()->isPlaying() )
    beeCore->voicePlayer()->pause();
}

void GuiVoicePlayer::onSliderPositionMoved( int slider_new_position )
{
  mp_lPosition->setText( Bee::timeToString( slider_new_position ) );
}

void GuiVoicePlayer::onSliderPositionReleased()
{
  if( beeCore->voicePlayer()->isPlaying() )
    onPlayClicked();
}
