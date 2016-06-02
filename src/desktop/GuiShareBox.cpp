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
#include "GuiIconProvider.h"
#include "GuiShareBox.h"
#include "GuiShareBoxFileInfoList.h"
#include "FileShare.h"
#include "Settings.h"
#include "UserManager.h"


GuiShareBox::GuiShareBox( QWidget *parent )
  : QWidget( parent ), m_fileInfoList()
{
  setupUi( this );

  setObjectName( "GuiShareBox" );
  mp_lTitle->setText( QString( "<b>%1</b>" ).arg( tr( "Shared box" ) ) );

  m_fileInfoList.initTree( mp_twBox );
  m_userId = ID_LOCAL_USER;
  m_currentFolder = "";
  connect( mp_twBox, SIGNAL( customContextMenuRequested( const QPoint& ) ), this, SLOT( openDownloadMenu( const QPoint& ) ) );
  connect( mp_twBox, SIGNAL( itemDoubleClicked( QTreeWidgetItem*, int ) ), this, SLOT( onItemDoubleClicked( QTreeWidgetItem*, int ) ) );

}

void GuiShareBox::setupToolBar( QToolBar* bar )
{

  /* Update button */
  mp_actUpdate = bar->addAction( QIcon( ":/images/update.png" ), tr( "Update" ), this, SLOT( updateBox() ) );

  /* Download button */
 // mp_actDownload = bar->addAction( QIcon( ":/images/download-box.png" ), tr( "Download" ), this, SLOT( downloadSelected() ) );

  /* Upload button */
 // mp_actUpload = bar->addAction( QIcon( ":/images/upload-box.png" ), tr( "Upload" ), this, SLOT( downloadSelected() ) );

}

void GuiShareBox::enableUpdateButton()
{
  if( !mp_actUpdate->isEnabled() )
    mp_actUpdate->setEnabled( true );
}

void GuiShareBox::updateBox()
{
  mp_actUpdate->setEnabled( false );
  emit shareBoxRequest( m_userId, m_currentFolder );
  QTimer::singleShot( 10000, this, SLOT( enableUpdateButton() ) );
}

void GuiShareBox::updateBox( const User& u, const QString& folder_path, const QList<FileInfo>&  file_info_list )
{
  m_userId = u.id();
  m_currentFolder = folder_path;
  m_fileInfoList.setFileInfoList( file_info_list );
  if( !folder_path.isEmpty() )
    m_fileInfoList.addDotDotFolder();

  QString s_title = QString( "%1@%2:<b>%3</b>" ).arg( "ShareBox" )
          .arg( u.isLocal() ? "local" : u.path() )
          .arg( folder_path.isEmpty() ? "/" : folder_path );
  mp_lTitle->setText( Bee::convertToNativeFolderSeparator( s_title ) );

  enableUpdateButton();
}

void GuiShareBox::openDownloadMenu( const QPoint& p )
{
  QTreeWidgetItem* item = mp_twBox->itemAt( p );
  if( item )
    item->setSelected( true );

  //int selected_items = m_fileInfoList.selectedFileInfoList().size();

  QMenu menu;
  menu.addAction( QIcon( ":/images/download.png" ), "Menu here" );
  menu.exec( QCursor::pos() );
}

void GuiShareBox::onItemDoubleClicked( QTreeWidgetItem* item, int )
{
  if( !item )
    return;

  GuiShareBoxFileInfoItem* file_info_item = (GuiShareBoxFileInfoItem*)item;

  if( file_info_item->isFolder() )
  {
    QString new_folder;
    if( file_info_item->fileInfo().id() == ID_DOTDOT_FOLDER )
      new_folder = Bee::folderCdUp( m_currentFolder );
    else
      new_folder = QString( "%1/%2" ).arg( m_currentFolder ).arg( file_info_item->fileInfo().name() );
    emit shareBoxRequest( m_userId, new_folder );
  }
  else
  {
    if( m_userId == ID_LOCAL_USER )
    {
      QUrl file_url = QUrl::fromLocalFile( file_info_item->fileInfo().path() );
      emit openUrlRequest( file_url );
    }
  }

}
