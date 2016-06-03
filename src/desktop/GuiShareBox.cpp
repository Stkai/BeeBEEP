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
#include "Settings.h"
#include "FileDialog.h"
#include "UserManager.h"


GuiShareBox::GuiShareBox( QWidget *parent )
  : QWidget( parent ), m_myBoxList(), m_outBoxList()
{
  setupUi( this );
  setObjectName( "GuiShareBox" );

  QGridLayout* grid_layout = new QGridLayout( this );
  grid_layout->setSpacing( 0 );
  grid_layout->setObjectName( QString::fromUtf8( "grid_layout" ) );
  grid_layout->setContentsMargins( 0, 0, 0, 0 );

  grid_layout->addWidget( mp_lTitle, 0, 0, 1, 1 );

  mp_splitter = new QSplitter( this );
  mp_splitter->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding );
  mp_splitter->setOrientation( Qt::Horizontal );
  mp_splitter->setChildrenCollapsible( false );
  mp_splitter->addWidget( mp_myFrame );
  mp_splitter->addWidget( mp_outFrame );
  grid_layout->addWidget( mp_splitter, 1, 0, 1, 1);

  mp_lTitle->setText( QString( "<b>%1</b>" ).arg( tr( "ShareBox" ) ) );

  m_myBoxList.initTree( mp_twMyBox );
  m_outBoxList.initTree( mp_twOutBox );

  m_myCurrentFolder = "";
  m_outCurrentFolder = "";
  m_userId = ID_LOCAL_USER;

  mp_pbMyUpdate->setToolTip( tr( "Update your ShareBox" ) );
  mp_pbOutUpdate->setToolTip( tr( "Update ShareBox" ) );

  connect( mp_comboUsers, SIGNAL( currentIndexChanged( int ) ), this, SLOT( onShareBoxSelected( int ) ) );
  connect( mp_pbSelectMyBox, SIGNAL( clicked() ), this, SLOT( selectMyShareBoxFolder() ) );
  connect( mp_cbEnableMyBox, SIGNAL( toggled( bool ) ), this, SLOT( onEnableMyShareBoxClicked() ) );
  connect( mp_pbMyUpdate, SIGNAL( clicked() ), this, SLOT( updateMyBox() ) );
  connect( mp_pbOutUpdate, SIGNAL( clicked() ), this, SLOT( updateOutBox() ) );
  connect( mp_twMyBox, SIGNAL( itemDoubleClicked( QTreeWidgetItem*, int ) ), this, SLOT( onMyItemDoubleClicked( QTreeWidgetItem*, int ) ) );
  connect( mp_twOutBox, SIGNAL( itemDoubleClicked( QTreeWidgetItem*, int ) ), this, SLOT( onOutItemDoubleClicked( QTreeWidgetItem*, int ) ) );
}

void GuiShareBox::updateShareBoxes()
{
  mp_cbEnableMyBox->setChecked( Settings::instance().useShareBox() );
  if( mp_comboUsers->count() > 0 )
    mp_comboUsers->clear();
  foreach( User u, UserManager::instance().userList().toList() )
  {
    if( u.isStatusConnected() )
      mp_comboUsers->addItem( u.name(), u.id() );
  }

  updateMyBox();
  updateOutBox();
}

void GuiShareBox::onEnableMyShareBoxClicked()
{
  Settings::instance().setUseShareBox( mp_cbEnableMyBox->isChecked() );
  Settings::instance().save();
  updateMyBox();
}

void GuiShareBox::enableMyUpdateButton()
{
  if( !mp_pbMyUpdate->isEnabled() )
    mp_pbMyUpdate->setEnabled( true );
}

void GuiShareBox::enableOutUpdateButton()
{
  if( !mp_pbOutUpdate->isEnabled() )
    mp_pbOutUpdate->setEnabled( true );
}

void GuiShareBox::updateMyBox()
{
  mp_pbMyUpdate->setEnabled( false );
  if( mp_cbEnableMyBox->isChecked() )
  {
    QString my_path = QString( "%1/%2" ).arg( Settings::instance().shareBoxPath() ).arg( m_myCurrentFolder );
    mp_lMyBox->setToolTip( my_path );
    emit shareBoxRequest( ID_LOCAL_USER, m_myCurrentFolder );
    QTimer::singleShot( 10000, this, SLOT( enableMyUpdateButton() ) );
  }
  else
  {
    m_myBoxList.clearTree();
    m_myCurrentFolder = "";
    mp_lMyBox->setToolTip( "" );
    mp_lMyBox->setText( tr( "Your ShareBox is disabled" ) );
  }
}

void GuiShareBox::updateOutBox()
{
  mp_pbOutUpdate->setEnabled( false );
  VNumber user_id = mp_comboUsers->count() > 0 ? Bee::qVariantToVNumber( mp_comboUsers->currentData() ) : ID_INVALID;

  if( user_id > ID_INVALID )
  {
    if( user_id != m_userId )
      emit shareBoxRequest( user_id, "" );
    else
      emit shareBoxRequest( user_id, m_outCurrentFolder );
    QTimer::singleShot( 10000, this, SLOT( enableOutUpdateButton() ) );
  }
  else
  {
    m_outBoxList.clearTree();
    m_outCurrentFolder = "";
    mp_lOutBox->setText( tr( "ShareBox is not available" ) );
  }
}

void GuiShareBox::updateBox( const User& u, const QString& folder_path, const QList<FileInfo>&  file_info_list )
{
  if( u.isLocal() )
  {
    updateMyBox( folder_path, file_info_list );
    enableMyUpdateButton();
  }
  else
  {
    updateOutBox( u, folder_path, file_info_list );
    enableOutUpdateButton();
  }
}

void GuiShareBox::updateMyBox( const QString& folder_path, const QList<FileInfo>&  file_info_list )
{
  m_myCurrentFolder = folder_path;
  m_myBoxList.setFileInfoList( file_info_list );
  if( !folder_path.isEmpty() )
    m_myBoxList.addDotDotFolder();

  QString s_title = QString( "<b>.%1</b>" ).arg( m_myCurrentFolder.isEmpty() ? "/" : m_myCurrentFolder );
  mp_lMyBox->setText( Bee::convertToNativeFolderSeparator( s_title ) );
}

void GuiShareBox::updateOutBox( const User& u, const QString& folder_path, const QList<FileInfo>&  file_info_list )
{
  m_userId = u.id();
  m_outCurrentFolder = folder_path;
  m_outBoxList.setFileInfoList( file_info_list );
  if( !folder_path.isEmpty() )
    m_outBoxList.addDotDotFolder();

  QString s_title = QString( "<b>.%1</b>" ).arg( m_outCurrentFolder.isEmpty() ? "/" : m_outCurrentFolder );
  mp_lOutBox->setText( Bee::convertToNativeFolderSeparator( s_title ) );

  mp_comboUsers->blockSignals( true );
  if( mp_comboUsers->findData( u.id() ) == -1 )
    mp_comboUsers->addItem( u.name(), u.id() );

  mp_comboUsers->setEnabled( mp_comboUsers->count() > 1 );
  mp_comboUsers->blockSignals( false );
}

void GuiShareBox::onMyItemDoubleClicked( QTreeWidgetItem* item, int )
{
  if( !item )
    return;

  GuiShareBoxFileInfoItem* file_info_item = (GuiShareBoxFileInfoItem*)item;

  if( file_info_item->isFolder() )
  {
    QString new_folder;
    if( file_info_item->fileInfo().id() == ID_DOTDOT_FOLDER )
      new_folder = Bee::folderCdUp( m_myCurrentFolder );
    else
      new_folder = QString( "%1/%2" ).arg( m_myCurrentFolder ).arg( file_info_item->fileInfo().name() );
    emit shareBoxRequest( ID_LOCAL_USER, new_folder );
  }
  else
  {
    QUrl file_url = QUrl::fromLocalFile( file_info_item->fileInfo().path() );
    emit openUrlRequest( file_url );
  }
}

void GuiShareBox::onOutItemDoubleClicked( QTreeWidgetItem* item, int )
{
  if( !item )
    return;

  GuiShareBoxFileInfoItem* file_info_item = (GuiShareBoxFileInfoItem*)item;

  if( file_info_item->isFolder() )
  {
    QString new_folder;
    if( file_info_item->fileInfo().id() == ID_DOTDOT_FOLDER )
      new_folder = Bee::folderCdUp( m_outCurrentFolder );
    else
      new_folder = QString( "%1/%2" ).arg( m_outCurrentFolder ).arg( file_info_item->fileInfo().name() );
    emit shareBoxRequest( m_userId, new_folder );
  }
}

void GuiShareBox::selectMyShareBoxFolder()
{
  QString sharebox_folder_path = FileDialog::getExistingDirectory( this,
                                                                   tr( "%1 - Select the ShareBox folder" )
                                                                   .arg( Settings::instance().programName() ),
                                                                         Settings::instance().shareBoxPath() );
  if( sharebox_folder_path.isEmpty() )
    return;

  Settings::instance().setShareBoxPath( sharebox_folder_path );
  Settings::instance().save();
  m_myCurrentFolder = "";
  updateMyBox();
}

void GuiShareBox::onShareBoxSelected( int )
{
  updateOutBox();
}

void GuiShareBox::onShareFolderUnavailable( const User& u, const QString& folder_path )
{
  qDebug() << u.path() << "has not shared box folder" << folder_path;
}
