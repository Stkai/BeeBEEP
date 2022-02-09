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
// $Id: GuiWorkgroups.cpp 1455 2020-12-23 10:17:53Z mastroddi $
//
//////////////////////////////////////////////////////////////////////

#include "BeeUtils.h"
#include "Config.h"
#include "Core.h"
#include "GuiCreateTextFile.h"
#include "IconManager.h"
#include "Settings.h"


GuiCreateTextFile::GuiCreateTextFile( QWidget *parent )
  : QDialog( parent )
{
  setupUi( this );
  setWindowTitle( tr( "Create text" ) + QString( " - %1" ).arg( Settings::instance().programName() ) );
  setWindowIcon( IconManager::instance().icon( "send-code.png" ) );
  Bee::removeContextHelpButton( this );

  mp_leFileSuffix->setToolTip( tr( "For example:" ) + QString( " c, cpp, h, txt, html, php, js, css, java, ..." ) );

  connect( mp_pbSendAsMessage, SIGNAL( clicked() ), this, SLOT( sendTextAsMessage() ) );
  connect( mp_pbSendAsFile, SIGNAL( clicked() ), this, SLOT( sendTextAsFile() ) );

  loadSettings();
}

QString GuiCreateTextFile::text() const
{
  return mp_text->toPlainText();
}

void GuiCreateTextFile::sendTextAsMessage()
{
  m_sendAsFile = false;
  sendText();
}

void GuiCreateTextFile::sendTextAsFile()
{
  m_sendAsFile = true;
  sendText();
}

QString GuiCreateTextFile::fileSuffix() const
{
  QString file_suffix = Bee::removeInvalidCharactersForFilePath( mp_leFileSuffix->text() );
  while( file_suffix.startsWith( '.' ) )
    file_suffix.remove( 0, 1 );
  return file_suffix.isEmpty() ? QLatin1String( "txt" ) : file_suffix;
}

void GuiCreateTextFile::loadSettings()
{
  m_sendAsFile = false;
  if( beeCore->isFileTransferActive() )
    mp_pbSendAsFile->setEnabled( true );
  else
    mp_pbSendAsFile->setEnabled( false );

  mp_leFileSuffix->setText( Settings::instance().createTextCodeFileSuffix() );
  mp_cbUseFileTmp->setChecked( Settings::instance().createTextCodeAsTemporaryFile() );
}

void GuiCreateTextFile::saveSettings()
{
  Settings::instance().setCreateTextCodeFileSuffix( fileSuffix() );
  Settings::instance().setCreateTextCodeAsTemporaryFile( mp_cbUseFileTmp->isChecked() );
}

void GuiCreateTextFile::sendText()
{
  saveSettings();
  accept();
}

