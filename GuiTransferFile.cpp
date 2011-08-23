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
#include "GuiTransferFile.h"
#include "FileInfo.h"
#include "Settings.h"
#include "User.h"


GuiTransferFile::GuiTransferFile( QWidget *parent )
 : QTreeWidget( parent )
{
  setObjectName( "GuiTransferFile" );
  QStringList labels;
  labels << tr( "File" ) << tr( "User" ) << tr( "Status" );
  setHeaderLabels( labels );
  setRootIsDecorated( false );

  QHeaderView* hv = header();
  hv->setResizeMode( ColumnFile, QHeaderView::Stretch );
  hv->setResizeMode( ColumnUser, QHeaderView::Stretch );
  hv->setResizeMode( ColumnProgress, QHeaderView::Stretch );
}

void GuiTransferFile::setProgress( const User& u, const FileInfo& fi, FileSizeType bytes )
{
  QTreeWidgetItem* item = findItem( fi.id() );
  if( !item )
  {
    item = new QTreeWidgetItem( this );
    item->setFirstColumnSpanned( false );
    item->setIcon( ColumnFile, fi.isDownload() ? QIcon( ":/images/download.png" ) : QIcon( ":/images/upload.png" ) );
    item->setText( ColumnFile, fi.name() );
    item->setData( ColumnFile, FileId, fi.id() );
    item->setData( ColumnFile, FilePath, fi.path() );
    item->setText( ColumnUser, u.path() );
  }
  showProgress( item, fi, bytes );
}

void GuiTransferFile::cancelTransfer()
{
  VNumber file_info_id = 0;
  emit transferCancelled( file_info_id );
}

QTreeWidgetItem* GuiTransferFile::findItem( VNumber file_info_id )
{
  QTreeWidgetItemIterator it( this );
  while( *it )
  {
    if( (*it)->data( ColumnFile, FileId ).toULongLong() == file_info_id )
      return *it;
    ++it;
  }
  return 0;
}

void GuiTransferFile::showProgress( QTreeWidgetItem* item, const FileInfo& fi, FileSizeType bytes )
{
  if( fi.size() == 0 )
  {
    qWarning() << "GuiTransferFile::showProgress try to show progress divided by 0:" << fi.path();
    return;
  }
  QString file_transfer_progress = QString( "%1 %2 of %3 (%4%)" ).arg( fi.isDownload() ? tr( "Downloading") : tr( "Uploading"),
                                      Bee::bytesToString( bytes ), Bee::bytesToString( fi.size() ),
                                      QString::number( static_cast<FileSizeType>( (bytes * 100) / fi.size())) );
  item->setText( ColumnProgress, file_transfer_progress );

  file_transfer_progress.prepend( QString( "[%1] " ).arg( fi.name() ) );
  if( !isVisible() )
    emit stringToShow( file_transfer_progress, 1000 );
  qDebug() << file_transfer_progress;
}

void GuiTransferFile::setMessage( const User& u, const FileInfo& fi, const QString& msg )
{
  QTreeWidgetItem* item = findItem( fi.id() );
  if( !item )
    setProgress( u, fi, 0 );
  item = findItem( fi.id() );
  if( !item )
  {
    qWarning() << "Unable to find file transfer item with id" << fi.id();
    return;
  }
  item->setText( ColumnProgress, msg );
}

