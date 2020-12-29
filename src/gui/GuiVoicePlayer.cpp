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
#include "GuiVoicePlayer.h"
#include "IconManager.h"


GuiVoicePlayer::GuiVoicePlayer( QWidget* parent )
  : QWidget( parent ), m_filePath( "" ), m_isPaused( true )
{
  setObjectName( "GuiVoicePlayer" );
  setupUi( this );

  connect( mp_pbPlay, SIGNAL( clicked() ), this, SLOT( onPlayClicked() ) );

}

void GuiVoicePlayer::setFilePath( const QString& file_path )
{
  m_filePath = file_path;
  mp_sliderPosition->setMinimum( 0 );
  mp_sliderPosition->setMaximum( 1 );
  mp_sliderPosition->setValue( 0 );
  mp_sliderPosition->setEnabled( false );
  mp_lDuration->setText( Bee::timeToString( 0 ) );
}

void GuiVoicePlayer::setFileDuration( const QString& file_path, qint64 file_duration )
{
  if( m_filePath == file_path )
  {
    int slider_max_value = file_duration < 1000 ? 1 : static_cast<int>( file_duration / 1000 );
    mp_sliderPosition->setMinimum( 0 );
    mp_sliderPosition->setMaximum( slider_max_value );
    mp_sliderPosition->setValue( 0 );
    mp_sliderPosition->setEnabled( true );
    mp_lDuration->setText( Bee::timeToString( file_duration ) );
  }
}

void GuiVoicePlayer::setFilePosition( const QString& file_path, qint64 file_position )
{
  if( m_filePath == file_path )
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

void GuiVoicePlayer::onPlayClicked()
{
  if( m_isPaused )
  {
    m_isPaused = false;
    mp_pbPlay->setIcon( IconManager::instance().icon( "pause.png" ) );
    qint64 file_position = filePosition();
    emit playFile( m_filePath, file_position );
  }
  else
  {
    m_isPaused = true;
    mp_pbPlay->setIcon( IconManager::instance().icon( "play.png" ) );
    emit pauseFile( m_filePath );
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
