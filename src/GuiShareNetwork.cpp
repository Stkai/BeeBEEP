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

  mp_comboFileType->insertItem( Bee::NumFileType, QIcon( ":/images/green-ball.png" ), tr( "All" ), Bee::NumFileType );
  mp_comboFileType->setCurrentIndex( Bee::NumFileType );

  connect( mp_pbSearch, SIGNAL( clicked() ), this, SLOT( search() ) );
  connect( mp_twShares, SIGNAL( itemDoubleClicked( QTreeWidgetItem*, int ) ), this, SLOT( checkItemDoubleClicked( QTreeWidgetItem*, int ) ) );
}

void GuiShareNetwork::loadShares( const User& u )
{
  foreach( FileInfo fi, FileShare::instance().network().values( u.id() ) )
  {
    if( !fi.isValid() )
      continue;

    if( !filterPassThrough( fi ) )
      continue;

    QTreeWidgetItem* item = new QTreeWidgetItem( mp_twShares );
    item->setIcon( ColumnFile, QIcon( Bee::fileTypeIconFileName( Bee::fileTypeFromSuffix( fi.suffix() ) ) ) );
    item->setText( ColumnFile, fi.name() );
    item->setData( ColumnFile, Qt::UserRole + 1, u.id() );
    item->setText( ColumnSize, Bee::bytesToString( fi.size() ) );
    item->setText( ColumnUser, u.name() );
  }
}

void GuiShareNetwork::checkItemDoubleClicked( QTreeWidgetItem*, int )
{

}

void GuiShareNetwork::search()
{
  mp_twShares->clear();

  foreach( User u, UserManager::instance().userList().toList() )
  {
    if( u.isConnected() && u.isOnLan() )
      loadShares( u );
  }

  emit fileShareListRequested();
}

bool GuiShareNetwork::filterPassThrough( const FileInfo& fi )
{
  QString filter_name = mp_leSearch->text().simplified().toLower();
  if( !filter_name.isEmpty() && !fi.name().contains( filter_name, Qt::CaseInsensitive ) )
    return false;

  if( mp_comboFileType->currentIndex() == (int)Bee::NumFileType )
    return true;
  else
    return (int)Bee::fileTypeFromSuffix( fi.suffix() ) == mp_comboFileType->currentIndex();
}
