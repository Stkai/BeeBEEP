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
  : QWidget( parent ), m_filePath( "" ), m_chatId( ID_INVALID ), m_isPaused( true )
{
  setObjectName( "GuiVoicePlayer" );
  setupUi( this );

  mp_pbPlay->setEnabled( false );
  connect( mp_pbPlay, SIGNAL( clicked() ), this, SLOT( onPlayClicked() ) );
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
  m_isPaused = true;
  connect( beeCore->voicePlayer(), SIGNAL( playing( const QString&, VNumber ) ), this, SLOT( onPlaying( const QString&, VNumber ) ) );
  connect( beeCore->voicePlayer(), SIGNAL( finished( const QString&, VNumber ) ), this, SLOT( finished( const QString&, VNumber ) ) );
  connect( beeCore->voicePlayer(), SIGNAL( durationChanged( const QString&, VNumber, qint64 ) ), this, SLOT( setFileDuration( const QString&, VNumber, qint64 ) ) );
  connect( beeCore->voicePlayer(), SIGNAL( positionChanged( const QString&, VNumber, qint64 ) ), this, SLOT( setPositionChanged( const QString&, VNumber, qint64 ) ) );
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
  }
}

void GuiVoicePlayer::onFinished( const QString& voice_file_path, VNumber chat_id )
{
  if( m_chatId == chat_id && m_filePath == voice_file_path )
  {
    setPaused( true );
    if( isVisible() )
      hide();
  }
}

void GuiVoicePlayer::setFileDuration( const QString& voice_file_path, VNumber chat_id, qint64 file_duration )
{
  if( m_chatId == chat_id && m_filePath == voice_file_path )
  {
    int slider_max_value = file_duration < 1000 ? 1 : static_cast<int>( file_duration / 1000 );
    mp_sliderPosition->setMinimum( 0 );
    mp_sliderPosition->setMaximum( slider_max_value );
    mp_sliderPosition->setValue( 0 );
    mp_sliderPosition->setEnabled( true );
    mp_lDuration->setText( Bee::timeToString( file_duration ) );
  }
}

void GuiVoicePlayer::setFilePosition( const QString& voice_file_path, VNumber chat_id, qint64 file_position )
{
  if( m_chatId == chat_id && m_filePath == voice_file_path )
  {
    int slider_value = file_position < 1000 ? 1 : static_cast<int>( file_position / 1000 );
    if( slider_value > mp_sliderPosition->maximum() && mp_sliderPosition->value() < mp_sliderPosition->maximum() )
      mp_sliderPosition->setValue( mp_sliderPosition->maximum() );
    else
      mp_sliderPosition->setValue( slider_value );
  }
}

qint64 GuiVoicePlayer::filePosition() const
{
  int slider_value = mp_sliderPosition->value();
  return slider_value > 1 ? slider_value * 1000 : 0;
}

void GuiVoicePlayer::setPaused( bool yes )
{
  m_isPaused = yes;
  if( m_isPaused )
    mp_pbPlay->setIcon( IconManager::instance().icon( "play.png" ) );
  else
    mp_pbPlay->setIcon( IconManager::instance().icon( "pause.png" ) );
}

void GuiVoicePlayer::onPlayClicked()
{
  if( m_isPaused )
  {
    setPaused( false );
    qint64 file_position = filePosition();
    beeCore->voicePlayer()->playFile( m_filePath, m_chatId, file_position );
  }
  else
  {
    setPaused( true );
    beeCore->voicePlayer()->stop();
  }
}

void GuiVoicePlayer::onSliderPositionPressed()
{
  if( !m_isPaused )
    onPlayClicked();
}

void GuiVoicePlayer::onSliderPositionReleased()
{
  if( m_isPaused )
    onPlayClicked();
}
