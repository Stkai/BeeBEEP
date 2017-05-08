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

#include "BeeUtils.h"
#include "ChatManager.h"
#include "GuiCreateGroup.h"
#include "Settings.h"
#include "UserManager.h"


GuiCreateGroup::GuiCreateGroup( QWidget *parent )
  : QDialog( parent ), m_selectedName( "" ), m_selectedUsersId()
{
  setupUi( this );
  setObjectName( "GuiCreateGroup" );
  Bee::removeContextHelpButton( this );

  QStringList labels;
  labels << tr( "Users" );
  mp_twUsers->setHeaderLabels( labels );

  mp_twUsers->sortItems( 0, Qt::AscendingOrder );
  mp_twUsers->setAlternatingRowColors( true );
  mp_twUsers->setRootIsDecorated( false );
  mp_twUsers->setSelectionMode( QAbstractItemView::MultiSelection );

  connect( mp_pbOk, SIGNAL( clicked() ), this, SLOT( checkAndClose() ) );
  connect( mp_pbCancel, SIGNAL( clicked() ), this, SLOT( reject() ) );
}

void GuiCreateGroup::init( const QString& group_name, const QList<VNumber>& group_members )
{
  m_selectedName = group_name;
  m_selectedUsersId = group_members;
}

void GuiCreateGroup::loadData( bool is_group )
{
  if( is_group )
  {
    mp_labelName->setText( tr( "Group name" ) );
    mp_labelText->setText( tr( "Please add member in the group:" ) );
  }
  else
  {
    mp_labelName->setText( tr( "Chat name" ) );
    mp_labelText->setText( tr( "Please add member in the chat:" ) );
  }

  if( m_selectedName.isEmpty() )
  {
    mp_leName->setText( "" );
    if( is_group )
      setWindowTitle( tr( "Create Group - %1" ).arg( Settings::instance().programName() ) );
    else
      setWindowTitle( tr( "Create Chat - %1" ).arg( Settings::instance().programName() ) );
  }
  else
  {
    mp_leName->setText( m_selectedName );
    if( is_group )
      setWindowTitle( tr( "Edit Group - %1" ).arg( Settings::instance().programName() ) );
    else
      setWindowTitle( tr( "Edit Chat - %1" ).arg( Settings::instance().programName() ) );
  }

  if( mp_twUsers->topLevelItemCount() > 0 )
    mp_twUsers->clear();

  QTreeWidgetItem* item;
  foreach( User u, UserManager::instance().userList().toList() )
  {
    item = new QTreeWidgetItem( mp_twUsers );
    item->setIcon( 0, Bee::userStatusIcon( u.status() ) );
    item->setText( 0, u.name() );
    item->setData( 0, Qt::UserRole+1, u.id() );
    item->setFlags( item->flags() | Qt::ItemIsUserCheckable );
    item->setCheckState( 0, Qt::Checked );

    if( m_selectedUsersId.contains( u.id() ) )
    {
      item->setCheckState( 0, Qt::Checked );
      item->setDisabled( true );
    }
    else
      item->setCheckState( 0, Qt::Unchecked );
  }
}

void GuiCreateGroup::checkAndClose()
{
  QList<QTreeWidgetItem*> item_list;
  QTreeWidgetItemIterator it( mp_twUsers, QTreeWidgetItemIterator::Checked );
  while( *it )
  {
    item_list.append( *it );
    ++it;
  }

  if( item_list.size() < 2 )
  {
    QMessageBox::information( this, Settings::instance().programName(), tr( "Please select two or more member for the group." ) );
    return;
  }

  QString group_name_tmp = mp_leName->text().simplified();

  if( group_name_tmp.isEmpty() )
  {
    QMessageBox::information( this, Settings::instance().programName(), tr( "Please insert a group name." ) );
    mp_leName->setFocus();
    return;
  }

  if( group_name_tmp != m_selectedName )
  {
    if( UserManager::instance().hasGroupName( group_name_tmp ) || ChatManager::instance().hasName( group_name_tmp ) )
    {
      QMessageBox::information( this, Settings::instance().programName(), tr( "%1 already exists as group name or chat name.\nPlease select a different name." ).arg( group_name_tmp ) );
      mp_leName->setFocus();
      return;
    }
  }

  m_selectedName = group_name_tmp;

  if( m_selectedUsersId.size() > 0 )
    m_selectedUsersId.clear();
  m_selectedUsersId.append( ID_LOCAL_USER );

  VNumber user_id = 0;
  foreach( QTreeWidgetItem* item, item_list )
  {
    user_id = Bee::qVariantToVNumber( item->data( 0, Qt::UserRole+1 ) );
    if( !m_selectedUsersId.contains( user_id ) )
      m_selectedUsersId.append( user_id );
  }

  Group g = UserManager::instance().findGroupByUsers( m_selectedUsersId );
  if( g.isValid() )
  {
    if( QMessageBox::question( this, Settings::instance().programName(),
                               QString( "%1\n%2" ).arg( tr( "There is a group with the same members: %1." ).arg( g.name() ) )
                                                  .arg( "How do you want to continue?" ),
                               tr( "Create new group" ), tr( "Cancel" ), QString::null, 1, 1 ) == 1 )
    {
      mp_leName->setFocus();
      return;
    }
  }

  accept();
}
