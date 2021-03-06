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

#include "GuiFileTransferItem.h"
#include "BeeUtils.h"
#include "IconManager.h"
#include "User.h"


GuiFileTransferItem::GuiFileTransferItem( QTreeWidget* parent )
 : QTreeWidgetItem( parent ), m_peerId( ID_INVALID), m_userId( ID_INVALID ), m_fileInfo(), m_transferState( FileTransferPeer::Unknown )
{
}

GuiFileTransferItem::GuiFileTransferItem( QTreeWidgetItem* parent )
 : QTreeWidgetItem( parent ),  m_peerId( ID_INVALID), m_userId( ID_INVALID ), m_fileInfo(), m_transferState( FileTransferPeer::Unknown )
{
}

QIcon GuiFileTransferItem::defaultIcon() const
{
  return m_fileInfo.isDownload() ? IconManager::instance().icon( "download.png" ) : IconManager::instance().icon( "upload.png" );
}

void GuiFileTransferItem::init( VNumber peer_id, const User& u, const FileInfo& fi )
{
  m_peerId = peer_id;
  m_fileInfo = fi;
  m_userId = u.id();
  setIcon( ColumnFile, defaultIcon() );
  setText( ColumnFile, fi.name() );
  setText( ColumnProgress, "" );
  setText( ColumnReport, "" );
  setText( ColumnTimeLeft, "" );
  updateUser( u );
}

bool GuiFileTransferItem::updateFileInfo( const FileInfo& fi, FileSizeType bytes_transferred, int elapsed_time )
{
  if( m_fileInfo.id() != fi.id() )
    return false;
  m_fileInfo = fi;
  update();
  if( bytes_transferred > 0 && m_transferState == FileTransferPeer::Transferring )
  {
    FileSizeType fi_size = m_fileInfo.size() > 0 ? m_fileInfo.size() : 1;
    if( bytes_transferred != fi.size() )
    {
      QString file_transfer_progress = QString( "%1 / %3 (%4)" ).arg( Bee::bytesToString( bytes_transferred ), Bee::bytesToString( fi_size ),
                                                                      m_fileInfo.isDownload() ? QObject::tr( "Downloading" ) : QObject::tr( "Uploading" ) );
      setText( ColumnProgress, file_transfer_progress );
      setText( ColumnReport, QString::number( static_cast<FileSizeType>( (bytes_transferred * 100) / fi_size ) ) );
      setText( ColumnTimeLeft, Bee::transferTimeLeft( bytes_transferred, fi_size, m_fileInfo.startingPosition(), elapsed_time ) );
    }
    else
    {
      setText( ColumnProgress, "" );
      setText( ColumnReport, QString::number( 100 ) );
      setText( ColumnTimeLeft, "" );
    }
  }
  return true;
}

bool GuiFileTransferItem::updateUser( const User& u )
{
  setText( ColumnUser, Bee::userNameToShow( u, false ) );
  return true;
}

void GuiFileTransferItem::setTransferState( FileTransferPeer::TransferState new_value )
{
  if( m_transferState != new_value )
  {
    m_transferState = new_value;
    update();
  }
}

void GuiFileTransferItem::setMessage( const QString& msg, FileTransferPeer::TransferState ft_state  )
{
  setTransferState( ft_state );
  setText( ColumnProgress, msg );
}

void GuiFileTransferItem::update()
{
  QIcon icon;
  QString action_tip = "";
  QString sort_string = text( ColumnSort );
  if( sort_string.size() > 0 && sort_string.at( 0 ).isLetter() )
    sort_string.remove( 0, 1 );

  if( m_transferState == FileTransferPeer::Queue )
  {
    icon = IconManager::instance().icon( "timer.png" );
    action_tip = QObject::tr( "In queue" );
    sort_string.prepend( 'E' );
  }
  else if( m_transferState == FileTransferPeer::Starting )
  {
    icon = IconManager::instance().icon( "play.png" );
    action_tip = QObject::tr( "Starting" );
    sort_string.prepend( 'B' );
  }
  else if( m_transferState == FileTransferPeer::Transferring )
  {
    icon = IconManager::instance().icon( "delete.png" );
    action_tip = QObject::tr( "Cancel transfer" );
    sort_string.prepend( 'A' );
  }
  else if( m_transferState == FileTransferPeer::Completed )
  {
    icon = IconManager::instance().icon( "green-ball.png" );
    action_tip = QObject::tr( "Completed" );
    sort_string.prepend( 'F' );
  }
  else if( m_transferState == FileTransferPeer::Error )
  {
    icon = IconManager::instance().icon( "red-ball.png" );
    action_tip = QObject::tr( "Not completed" );
    sort_string.prepend( 'H' );
  }
  else if( m_transferState == FileTransferPeer::Canceled )
  {
    icon = IconManager::instance().icon( "red-ball.png" );
    action_tip = QObject::tr( "Canceled" );
    sort_string.prepend( 'G' );
  }
  else if( m_transferState == FileTransferPeer::Pausing )
  {
    icon = IconManager::instance().icon( "timer.png" );
    action_tip = QObject::tr( "Please wait" );
    sort_string.prepend( 'C' );
  }
  else if( m_transferState == FileTransferPeer::Paused )
  {
    icon = IconManager::instance().icon( "pause.png" );
    action_tip = QObject::tr( "Resume transfer" );
    sort_string.prepend( 'D' );
  }
  else
  {
    action_tip = "";
    sort_string.prepend( 'Z' );
  }

  setToolTip( ColumnCancel, action_tip );
  setIcon( ColumnCancel, icon );
  setText( ColumnSort, sort_string );
}

bool GuiFileTransferItem::operator<( const GuiFileTransferItem& item ) const
{
  return text( ColumnSort ) < item.text( ColumnSort );
}

void GuiFileTransferItem::onTickEvent( int )
{
}
