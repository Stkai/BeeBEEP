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

#include "IconManager.h"
#include "ChatManager.h"
#include "EmoticonManager.h"
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
  setWindowIcon( IconManager::instance().icon( "message-create.png" ) );
  setObjectName( "GuiCreateMessage" );
  setWindowTitle( tr( "Create message" ) + QString( " - %1" ).arg( Settings::instance().programName() ) );

  mp_teMessage->setFocusPolicy( Qt::StrongFocus );
  mp_teMessage->setAcceptRichText( false );
  mp_teMessage->setObjectName( "GuiCreateMessageEdit" );
  mp_teMessage->setStyleSheet( QString( "#GuiCreateMessageEdit { background-color: %1; color: %2 }" ).arg( Settings::instance().chatBackgroundColor() ).arg( Settings::instance().chatDefaultTextColor() ) );
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

  mp_pbEmoticons->setDisabled( Settings::instance().useOnlyTextEmoticons() );

  updateRecipients();
  if( mp_pbEmoticons->isEnabled() )
  {
    mp_pbEmoticons->setChecked( Settings::instance().showEmoticonMenu() );
    updateEmoticons();
  }

  mp_cbSendPrivate->setChecked( Settings::instance().sendNewMessageIndividually() );
  mp_cbOpenChat->setChecked( Settings::instance().openChatWhenSendNewMessage() );

  if( !Settings::instance().createMessageGeometry().isEmpty() )
    restoreGeometry( Settings::instance().createMessageGeometry() );

  connect( mp_pbSend, SIGNAL( clicked() ), this, SLOT( sendMessage() ) );
  connect( mp_pbEditUsers, SIGNAL( clicked() ), this, SLOT( editRecipients() ) );
  connect( mp_twEmoticons, SIGNAL( emoticonSelected( const Emoticon& ) ), this, SLOT( addEmoticon( const Emoticon& ) ) );
  connect( mp_pbEmoticons, SIGNAL( clicked() ), this, SLOT( toggleEmoticons() ) );
}

void GuiCreateMessage::closeEvent( QCloseEvent* ev )
{
  Settings::instance().setSendNewMessageIndividually( mp_cbSendPrivate->isChecked() );
  Settings::instance().setOpenChatWhenSendNewMessage( mp_cbOpenChat->isChecked() );
  if( isVisible() )
  {
    Settings::instance().setShowEmoticonMenu( mp_twEmoticons->isVisible() );
    Settings::instance().setCreateMessageGeometry( saveGeometry() );
    Settings::instance().save();
  }
  QDialog::closeEvent( ev );
}

void GuiCreateMessage::updateEmoticons()
{
  QMetaObject::invokeMethod( mp_twEmoticons, "updateEmoticons", Qt::QueuedConnection );
  QMetaObject::invokeMethod( this, "toggleEmoticons", Qt::QueuedConnection );
}

void GuiCreateMessage::addEmoticon( const Emoticon& e )
{
  mp_teMessage->addEmoticon( e );
  mp_teMessage->setFocus();
}

void GuiCreateMessage::toggleEmoticons()
{
  if( mp_pbEmoticons->isEnabled() && mp_pbEmoticons->isChecked() )
    mp_twEmoticons->show();
  else
    mp_twEmoticons->hide();
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
    if( m_toChatIdList.contains( c.id() ) )
      item->setSelected( true );
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
        GuiChatItem* c_item = reinterpret_cast<GuiChatItem*>(item);
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
  QList<VNumber> to_chat_id_list;
  if( sendAsPrivate() )
  {
    foreach( VNumber chat_id, m_toChatIdList )
    {
      Chat c = ChatManager::instance().chat( chat_id );
      if( c.isValid() )
      {
        if( c.isGroup() )
        {
          QList<VNumber> chat_members = c.usersId();
          foreach( VNumber member_id, chat_members )
          {
            if( member_id != ID_LOCAL_USER )
            {
              Chat to_chat = ChatManager::instance().privateChatForUser( member_id );
              if( to_chat.isValid() && !to_chat_id_list.contains( to_chat.id() ) )
                to_chat_id_list.append( to_chat.id() );
            }
          }
        }
        else
        {
          if( !to_chat_id_list.contains( chat_id ) )
            to_chat_id_list.append( chat_id );
        }
      }
    }
    m_toChatIdList = to_chat_id_list;
  }

  QDialog::accept();
}
