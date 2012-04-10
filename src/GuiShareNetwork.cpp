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

  QHeaderView* hv = mp_twShares->header();
  hv->setResizeMode( ColumnFile, QHeaderView::ResizeToContents );
  hv->setResizeMode( ColumnSize, QHeaderView::ResizeToContents );

  connect( mp_twShares, SIGNAL( itemClicked( QTreeWidgetItem*, int ) ), this, SLOT( checkItemClicked( QTreeWidgetItem*, int ) ) );
}

void GuiShareNetwork::loadShares( const User& u )
{
  foreach( FileInfo fi, FileShare::instance().network().values( u.id() ) )
  {
    QTreeWidgetItem* item = new QTreeWidgetItem( mp_twShares );
    item->setText( ColumnFile, fi.name() );
    item->setData( ColumnFile, Qt::UserRole + 1, u.id() );
    item->setText( ColumnSize, Bee::bytesToString( fi.size() ) );
    item->setText( ColumnUser, u.name() );
  }
}

void GuiShareNetwork::checkItemClicked( QTreeWidgetItem*, int )
{

}
