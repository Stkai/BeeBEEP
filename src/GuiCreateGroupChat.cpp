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

#include "BeeUtils.h"
#include "GuiCreateGroupChat.h"
#include "Settings.h"
#include "UserManager.h"


GuiCreateGroupChat::GuiCreateGroupChat( QWidget *parent )
  : QDialog( parent ), m_groupUsersId()
{
  setupUi( this );
  setObjectName( "GuiCreateGroupChat" );

  setWindowTitle( tr( "Create Group - %1" ).arg( Settings::instance().programName() ) );

  mp_labelName->setText( tr( "Group name" ) );

  mp_labelText->setText( tr( "Please add member in the group chat:" ) );

  QStringList labels;
  labels << tr( "Users" );
  mp_twUsers->setHeaderLabels( labels );

  mp_twUsers->sortItems( 0, Qt::AscendingOrder );
  mp_twUsers->setAlternatingRowColors( true );
  mp_twUsers->setRootIsDecorated( false );
  mp_twUsers->setSelectionMode( QAbstractItemView::MultiSelection );

  connect( mp_pbOk, SIGNAL( clicked() ), this, SLOT( updateGroupChat() ) );
  connect( mp_pbCancel, SIGNAL( clicked() ), this, SLOT( reject() ) );
}

void GuiCreateGroupChat::setGroupChat( const Chat& c )
{
  mp_leName->setText( c.name() );
  m_groupUsersId = c.usersId();
  setWindowTitle( tr( "Edit Group - %1" ).arg( Settings::instance().programName() ) );

  QTreeWidgetItem* item;
  foreach( User u, UserManager::instance().userList().toList() )
  {
    item = new QTreeWidgetItem( mp_twUsers );
    item->setIcon( 0, Bee::userStatusIcon( u.service(), u.status() ) );
    item->setText( 0, u.name() );
    item->setData( 0, Qt::UserRole+1, u.id() );

    if( !m_groupUsersId.isEmpty() && m_groupUsersId.contains( u.id() ) )
    {
      item->setSelected( true );
      item->setDisabled( true );
    }
  }
}

void GuiCreateGroupChat::updateGroupChat()
{
  QList<QTreeWidgetItem*> item_list = mp_twUsers->selectedItems();
  if( item_list.size() < 2 )
  {
    QMessageBox::information( this, Settings::instance().programName(), tr( "Please select two or more member of the group chat." ) );
    return;
  }

  m_groupName = mp_leName->text().simplified();
  VNumber user_id = 0;
  foreach( QTreeWidgetItem* item, item_list )
  {
    user_id = Bee::qVariantToVNumber( item->data( 0, Qt::UserRole+1 ) );
    if( !m_groupUsersId.contains( user_id ) )
      m_groupUsersId.append( user_id );
  }

  accept();
}


