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
#include "GuiShareNetwork.h"
#include "FileShare.h"
#include "Settings.h"
#include "UserManager.h"


GuiShareNetwork::GuiShareNetwork( QWidget *parent )
  : QWidget(parent)
{
  setupUi( this );

  setObjectName( "GuiShareNetwork" );
  QStringList labels;
  labels << tr( "File" ) << tr( "Size" ) << tr( "User" );
  mp_twShares->setHeaderLabels( labels );

  mp_twShares->sortItems( ColumnFile, Qt::AscendingOrder );

  mp_twShares->setAlternatingRowColors( true );
  mp_twShares->setRootIsDecorated( false );

  QHeaderView* hv = mp_twShares->header();
  hv->setResizeMode( ColumnFile, QHeaderView::ResizeToContents );
  hv->setResizeMode( ColumnSize, QHeaderView::ResizeToContents );

  for( int i = Bee::FileAudio; i < Bee::NumFileType; i++ )
    mp_comboFileType->insertItem( i, QIcon( Bee::fileTypeIconFileName( (Bee::FileType)i ) ), Bee::fileTypeToString( (Bee::FileType)i ), i );

  mp_comboFileType->insertItem( Bee::NumFileType, QIcon( ":/images/green-ball.png" ), tr( "All Files" ), Bee::NumFileType );
  mp_comboFileType->setCurrentIndex( Bee::NumFileType );

  mp_comboUsers->insertItem( 0, tr( "All Users" ), 0 );
  mp_comboUsers->setCurrentIndex( 0 );

  connect( mp_pbSearch, SIGNAL( clicked() ), this, SLOT( search() ) );
  connect( mp_twShares, SIGNAL( itemDoubleClicked( QTreeWidgetItem*, int ) ), this, SLOT( checkItemDoubleClicked( QTreeWidgetItem*, int ) ) );
  connect( mp_comboFileType, SIGNAL( currentIndexChanged( int ) ), this, SLOT( search() ), Qt::QueuedConnection );
  connect( mp_comboUsers, SIGNAL( currentIndexChanged( int ) ), this, SLOT( search() ), Qt::QueuedConnection );
  connect( mp_leSearch, SIGNAL( textChanged( QString ) ), this, SLOT( enableSearchButton() ) );
}

void GuiShareNetwork::loadShares( const User& u )
{
  foreach( FileInfo fi, FileShare::instance().network().values( u.id() ) )
  {
    if( !fi.isValid() )
      continue;

    if( mp_comboUsers->findData( u.id() ) == -1 )
      mp_comboUsers->addItem( u.name(), u.id() );

    if( !filterPassThrough( u, fi ) )
      continue;

    QTreeWidgetItem* item = new QTreeWidgetItem( mp_twShares );
    item->setIcon( ColumnFile, QIcon( Bee::fileTypeIconFileName( Bee::fileTypeFromSuffix( fi.suffix() ) ) ) );
    item->setText( ColumnFile, fi.name() );
    item->setData( ColumnFile, UserId, u.id() );
    item->setData( ColumnFile, FileId, fi.id() );
    item->setText( ColumnSize, Bee::bytesToString( fi.size() ) );
    item->setText( ColumnUser, u.name() );
  }
}

void GuiShareNetwork::checkItemDoubleClicked( QTreeWidgetItem* item, int )
{
  if( !item )
    return;

  VNumber user_id = Bee::qVariantToVNumber( item->data( ColumnFile, UserId ) );
  VNumber file_id = Bee::qVariantToVNumber( item->data( ColumnFile, FileId ) );

  emit downloadSharedFile( user_id, file_id );
}

void GuiShareNetwork::search()
{
  mp_pbSearch->setEnabled( false );
  mp_twShares->clear();
  m_filterUserId = mp_comboUsers->currentIndex() <= 0 ? 0 : Bee::qVariantToVNumber( mp_comboUsers->itemData( mp_comboUsers->currentIndex() ) );

  foreach( User u, UserManager::instance().userList().toList() )
  {
    if( u.isConnected() && u.isOnLan() )
      loadShares( u );
    else
    {
      int user_id_index_to_remove = mp_comboUsers->findData( u.id() );
      if( user_id_index_to_remove > 0 && user_id_index_to_remove != mp_comboUsers->currentIndex() )
        mp_comboUsers->removeItem( user_id_index_to_remove );
    }
  }

  emit fileShareListRequested();
}

bool GuiShareNetwork::filterPassThrough( const User& u, const FileInfo& fi )
{
  QString filter_name = mp_leSearch->text().simplified().toLower();
  if( !filter_name.isEmpty() && !fi.name().contains( filter_name, Qt::CaseInsensitive ) )
    return false;

  if( m_filterUserId > 0 && u.id() != m_filterUserId )
    return false;

  if( mp_comboFileType->currentIndex() == (int)Bee::NumFileType )
    return true;
  else
    return (int)Bee::fileTypeFromSuffix( fi.suffix() ) == mp_comboFileType->currentIndex();
}

void GuiShareNetwork::enableSearchButton()
{
  mp_pbSearch->setEnabled( true );
}
