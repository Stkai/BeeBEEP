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
// Author: Marco Mastroddi (marco.mastroddi(AT)gmail.com)
//
// $Id$
//
//////////////////////////////////////////////////////////////////////

#include "GuiSavedChatList.h"
#include "ChatManager.h"
#include "Settings.h"


GuiSavedChatList::GuiSavedChatList( QWidget* parent )
  : QTreeWidget( parent )
{
  setObjectName( "GuiSavedChatList" );

  setColumnCount( 1 );
  header()->hide();
  setRootIsDecorated( false );
  setSortingEnabled( true );

  connect( this, SIGNAL( itemDoubleClicked( QTreeWidgetItem*, int ) ), this, SLOT( savedChatDoubleClicked( QTreeWidgetItem*, int ) ) );
}

QSize GuiSavedChatList::sizeHint() const
{
  return QSize( 140, 300 );
}

void GuiSavedChatList::savedChatDoubleClicked( QTreeWidgetItem* item, int )
{
  if( !item )
    return;

  GuiSavedChatItem* saved_chat_item = (GuiSavedChatItem*)item;
  emit savedChatSelected( saved_chat_item->chatName() );
}

void GuiSavedChatList::updateSavedChats()
{
  if( ChatManager::instance().constHistoryMap().isEmpty() )
    return;

  GuiSavedChatItem *item;
  QMap<QString, QString>::const_iterator it = ChatManager::instance().constHistoryMap().constBegin();
  while( it !=  ChatManager::instance().constHistoryMap().constEnd() )
  {
    if( it.key() != Settings::instance().defaultChatName() )
    {
      item = new GuiSavedChatItem( this );
      item->setChatName( it.key() );
      item->setIcon( 0, QIcon( ":/images/saved-chat.png" ) );
      item->setText( 0, it.key() );
    }
    ++it;
  }
}
