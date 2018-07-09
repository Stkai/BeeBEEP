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

#include "IconManager.h"
#include "ChatManager.h"
#include "GuiCreateMessage.h"
#include "GuiChatMessage.h"
#include "Settings.h"
#include "UserManager.h"


GuiCreateMessage::GuiCreateMessage( QWidget *parent )
 : QDialog( parent )
{
  setupUi( this );
  setWindowIcon( IconManager::instance().icon( "message-to-many.png" ) );
  setObjectName( "GuiCreateMessage" );
  setWindowTitle( tr( "Create message" ) + QString( " - %1" ).arg( Settings::instance().programName() ) );

  mp_teMessage->setFocusPolicy( Qt::StrongFocus );
  mp_teMessage->setAcceptRichText( false );
  mp_teMessage->setObjectName( "GuiCreateMessageEdit" );
  mp_teMessage->setStyleSheet( QString( "#GuiCreateMessageEdit { background-color: #fff; color: #000 }" ) );
  mp_teMessage->setForceCRonEnterClicked( true );
  mp_teMessage->setFocus();

  mp_teTo->setObjectName( "GuiCreateMessageRecipients" );
  mp_teTo->setFocusPolicy( Qt::NoFocus );
  mp_teTo->setStyleSheet( QString( "#GuiCreateMessageRecipients { background-color: #999; color: #fff }" ) );

  updateRecipients();

  connect( mp_pbSend, SIGNAL( clicked() ), this, SLOT( sendMessage() ) );

}

void GuiCreateMessage::updateRecipients()
{
  if( !m_toChatIdList.isEmpty() )
  {
    foreach( VNumber chat_id, m_toChatIdList )
    {

    }
  }
  else
    mp_teTo->setPlainText( QString( "< %1 >" ).arg( tr( "Nobody" ) ) );
}

void GuiCreateMessage::sendMessage()
{
  if( m_toChatIdList.isEmpty() )
  {
    QMessageBox::information( this, Settings::instance().programName(), tr( "Please select at least one recipient for your message." ) );
    return;
  }

  if( mp_teMessage->message().isEmpty() )
  {
    QMessageBox::information( this, Settings::instance().programName(), tr( "You cannot send an empty message." ) );
    mp_teMessage->setFocus();
    return;
  }

  QDialog::accept();
}
