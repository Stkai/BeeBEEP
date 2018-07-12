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
#include "GuiChatItem.h"
#include "GuiChatMessage.h"
#include "GuiSelectItems.h"
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
  mp_teMessage->setForceNoWritingAlert( true );
  mp_teMessage->setFocus();

  mp_teTo->setObjectName( "GuiCreateMessageRecipients" );
  mp_teTo->setFocusPolicy( Qt::NoFocus );
  mp_teTo->setStyleSheet( QString( "#GuiCreateMessageRecipients { background-color: #999; color: #fff }" ) );

  if( Settings::instance().maxChatsToOpenAfterSendingMessage() > 0 )
    mp_cbOpenChat->setChecked( true );
  else
    mp_cbOpenChat->setEnabled( false );

  updateRecipients();

  connect( mp_pbSend, SIGNAL( clicked() ), this, SLOT( sendMessage() ) );
  connect( mp_pbEditUsers, SIGNAL( clicked() ), this, SLOT( editRecipients() ) );

}

void GuiCreateMessage::updateRecipients()
{
  QStringList s_recipients;
  if( !m_toChatIdList.isEmpty() )
  {
    foreach( VNumber chat_id, m_toChatIdList )
      s_recipients <<  QString( "< %1 >" ).arg( ChatManager::instance().chatName( chat_id ) );
  }
  else
    s_recipients << QString( "< %1 >" ).arg( tr( "Nobody" ) );
  mp_teTo->setPlainText( s_recipients.join( ", " ) );
}

void GuiCreateMessage::editRecipients()
{
  GuiSelectItems gsi( this );
  gsi.setWindowTitle( tr( "Add recipients" ) + QString( " - %1" ).arg( Settings::instance().programName() ) );
  gsi.setWindowIcon( IconManager::instance().icon( "group-add.png" ) );

  QTreeWidget* tw = gsi.treeWidget();
  tw->setIconSize( Settings::instance().avatarIconSize() );
  GuiChatItem* item;

  foreach( Chat c, ChatManager::instance().constChatList() )
  {
    if( !c.isValid() )
      continue;

    if( c.isDefault() )
      continue;

    item = new GuiChatItem( tw );
    item->setChatId( c.id() );
    item->updateItem( c );
  }

  gsi.show();
  if( gsi.exec() == QDialog::Accepted )
  {
    m_toChatIdList.clear();
    QList<QTreeWidgetItem*> selected_items = tw->selectedItems();
    if( !selected_items.isEmpty() )
    {
      foreach( QTreeWidgetItem* item , selected_items )
      {
        GuiChatItem* c_item = (GuiChatItem*)item;
        if( c_item )
          m_toChatIdList.append( c_item->chatId() );
      }
    }
    updateRecipients();
  }
}

void GuiCreateMessage::sendMessage()
{
  if( m_toChatIdList.isEmpty() )
  {
    QMessageBox::information( this, Settings::instance().programName(), tr( "Please select at least one recipient for your message." ) );
    editRecipients();
    return;
  }

  if( mp_teMessage->message().isEmpty() )
  {
    QMessageBox::information( this, Settings::instance().programName(), tr( "You cannot send an empty message." ) );
    mp_teMessage->setFocus();
    return;
  }

  mp_teMessage->addMessageToHistory();

  QDialog::accept();
}
