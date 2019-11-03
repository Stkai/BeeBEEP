//////////////////////////////////////////////////////////////////////
//
// BeeBEEP Copyright (C) 2010-2019 Marco Mastroddi
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

#include "BeeUtils.h"
#include "BeeApplication.h"
#include "GuiIconProvider.h"
#include "GuiShareBox.h"
#include "GuiShareBoxFileInfoList.h"
#include "IconManager.h"
#include "Settings.h"
#include "FileDialog.h"
#include "UserManager.h"


GuiShareBox::GuiShareBox( QWidget *parent )
  : QWidget( parent )
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

  mp_lTitle->setText( QString( "<b>%1</b>" ).arg( QString( "BeeBOX" ) ) );

  mp_myBox->initTree();
  mp_outBox->initTree();

  m_myCurrentFolder = "";
  m_outCurrentFolder = "";
  m_userId = ID_INVALID;
  mp_fsWatcher = beeApp->fsWatcher();

  mp_pbMyUpdate->setToolTip( tr( "Update your BeeBOX" ) );
  mp_pbMyCreateFolder->setToolTip( tr( "Create folder in your BeeBOX" ) );
  mp_pbMyOpenFolder->setToolTip( tr( "Show current folder" ) );
  mp_pbOutUpdate->setToolTip( tr( "Update BeeBOX" ) );
  mp_pbOutParentFolder->setToolTip( tr( "Back to parent folder" ) );
  mp_pbOutCreateFolder->setToolTip( tr( "Create folder in BeeBOX" ) );

  mp_pbSelectMyBox->setIcon( IconManager::instance().icon( "sharebox.png" ) );
  mp_pbMyUpdate->setIcon( IconManager::instance().icon( "update.png" ) );
  mp_pbOutUpdate->setIcon( IconManager::instance().icon( "update.png" ) );
  mp_pbOutParentFolder->setIcon( IconManager::instance().icon( "back.png" ) );
  mp_pbMyCreateFolder->setIcon( IconManager::instance().icon( "folder-add.png" ) );
  mp_pbOutCreateFolder->setIcon( IconManager::instance().icon( "folder-add.png" ) );
  mp_pbMyOpenFolder->setIcon( IconManager::instance().icon( "folder.png" ) );

  mp_cbEnableMyBox->setChecked( Settings::instance().useShareBox() );

  connect( mp_comboUsers, SIGNAL( currentIndexChanged( int ) ), this, SLOT( onShareBoxSelected( int ) ) );
  connect( mp_pbSelectMyBox, SIGNAL( clicked() ), this, SLOT( selectMyShareBoxFolder() ) );
  connect( mp_cbEnableMyBox, SIGNAL( toggled( bool ) ), this, SLOT( onEnableMyShareBoxClicked() ) );
  connect( mp_pbMyUpdate, SIGNAL( clicked() ), this, SLOT( updateMyBox() ) );
  connect( mp_pbOutUpdate, SIGNAL( clicked() ), this, SLOT( updateOutBox() ) );
  connect( mp_pbOutParentFolder, SIGNAL( clicked() ), this, SLOT( backToParentFolder() ) );
  connect( mp_myBox, SIGNAL( itemDoubleClicked( QTreeWidgetItem*, int ) ), this, SLOT( onMyItemDoubleClicked( QTreeWidgetItem*, int ) ) );
  connect( mp_outBox, SIGNAL( itemDoubleClicked( QTreeWidgetItem*, int ) ), this, SLOT( onOutItemDoubleClicked( QTreeWidgetItem*, int ) ) );
  connect( mp_myBox, SIGNAL( dropEventRequest( const QString& ) ), this, SLOT( dropInMyBox( const QString& ) ) );
  connect( mp_outBox, SIGNAL( dropEventRequest( const QString& ) ), this, SLOT( dropInOutBox( const QString& ) ) );
  connect( mp_pbMyCreateFolder, SIGNAL( clicked() ), this, SLOT( createFolderInMyBox() ) );
  connect( mp_pbOutCreateFolder, SIGNAL( clicked() ), this, SLOT( createFolderInOutBox() ) );
  connect( mp_pbMyOpenFolder, SIGNAL( clicked() ), this, SLOT( openMyBox() ) );
  connect( mp_fsWatcher, SIGNAL( directoryChanged( const QString& ) ), this, SLOT( updateFolder( const QString& ) ) );
}

void GuiShareBox::onEnableMyShareBoxClicked()
{
  Settings::instance().setUseShareBox( mp_cbEnableMyBox->isChecked() );
  setCurrentFolder( ID_LOCAL_USER, "" );
  bool select_box = false;
  mp_comboUsers->setEnabled( Settings::instance().useShareBox() );
  mp_lUsers->setEnabled( mp_comboUsers->isEnabled() );

  if( Settings::instance().useShareBox() )
  {
    select_box = Settings::instance().shareBoxPath().isEmpty();
    if( !select_box )
    {
      QDir box_folder( Settings::instance().shareBoxPath() );
      if( box_folder.exists() )
        select_box = !Bee::folderIsWriteable( Settings::instance().shareBoxPath() );
    }
  }

  if( select_box )
  {
    QMessageBox::information( this, Settings::instance().programName(), tr( "BeeBOX path does not exist. Please select a valid folder.") );
    selectMyShareBoxFolder();
  }
  else
  {
    Settings::instance().save();
    QTimer::singleShot( 100, this, SLOT( updateShareBoxes() ) );
  }
}

void GuiShareBox::selectMyShareBoxFolder()
{
  QString folder_path = FileDialog::getExistingDirectory( this,
                                                          tr( "%1 - Select the BeeBOX folder" )
                                                          .arg( Settings::instance().programName() ),
                                                                Settings::instance().shareBoxPath() );
  if( folder_path.isEmpty() )
    return;

  Settings::instance().setShareBoxPath( Bee::convertToNativeFolderSeparator( folder_path ) );
  Settings::instance().save();
  makeShareBoxRequest( ID_LOCAL_USER, "", false );
}

void GuiShareBox::setUsers()
{
  mp_comboUsers->clear();
  mp_comboUsers->addItem( QString( "..." ), static_cast<VNumber>(ID_INVALID) );
  foreach( User u, UserManager::instance().userList().toList() )
  {
    if( u.isStatusConnected() )
      mp_comboUsers->addItem( Bee::userNameToShow( u, false ), u.id() );
  }
  mp_comboUsers->setEnabled( Settings::instance().useShareBox() );
  mp_lUsers->setEnabled( mp_comboUsers->isEnabled() );
}

VNumber GuiShareBox::selectedUserId() const
{
  VNumber user_id;
#if QT_VERSION >= 0x050000
  user_id = Bee::qVariantToVNumber( mp_comboUsers->currentData() );
#else
  user_id = Bee::qVariantToVNumber( mp_comboUsers->itemData( mp_comboUsers->currentIndex() ) );
#endif
  return user_id;
}

void GuiShareBox::updateShareBoxes()
{
  setUsers();
  makeShareBoxRequest( ID_LOCAL_USER, "", false );
  makeShareBoxRequest( selectedUserId(), "", false );
}

void GuiShareBox::updateCurrentFolderLabel( VNumber user_id )
{
  QLabel* folder_label = currentFolderLabel( user_id );

  if( user_id == ID_LOCAL_USER )
  {
    if( Settings::instance().useShareBox() )
    {
      folder_label->setText( Bee::convertToNativeFolderSeparator( QString( "<b>./%1</b>" ).arg( m_myCurrentFolder ) ) );
      folder_label->setToolTip( QString( "%1%2%3" ).arg( Settings::instance().shareBoxPath() ).arg( QDir::separator() ).arg( m_myCurrentFolder ) );
    }
    else
    {
      folder_label->setText( "" );
      folder_label->setToolTip( "" );
    }
  }
  else
  {
    if( Settings::instance().useShareBox() )
      folder_label->setText( Bee::convertToNativeFolderSeparator( QString( "<b>./%1</b>" ).arg( m_outCurrentFolder ) ) );
    else
      folder_label->setText( "" );
  }
}

void GuiShareBox::setCurrentFolder( VNumber user_id, const QString& new_path )
{
  if( user_id != ID_LOCAL_USER )
  {
    m_userId = user_id;
    if( m_userId != ID_INVALID )
      m_outCurrentFolder = new_path;
    else
      m_outCurrentFolder = "";
  }
  else
  {
    if( Settings::instance().useShareBox() )
    {
      QString old_dir_to_watch = Bee::convertToNativeFolderSeparator( m_myCurrentFolder.isEmpty() ? Settings::instance().shareBoxPath() : QString( "%1/%2" ).arg( Settings::instance().shareBoxPath(), m_myCurrentFolder ) );
      QString new_dir_to_watch = Bee::convertToNativeFolderSeparator( new_path.isEmpty() ? Settings::instance().shareBoxPath() : QString( "%1/%2" ).arg( Settings::instance().shareBoxPath(), new_path ) );
      mp_fsWatcher->removePath( old_dir_to_watch );
      mp_fsWatcher->addPath( new_dir_to_watch );
#ifdef BEEBEEP_DEBUG
      qDebug() << "FileSystemWatcher is now checking path" << qPrintable( mp_fsWatcher->directories().join( ", " ) );
#endif
    }
    else
    {
      QStringList fsw_dirs = mp_fsWatcher->directories();
      if( !fsw_dirs.isEmpty() )
      {
#ifdef BEEBEEP_DEBUG
        qDebug() << "FileSystemWatcher does not check path" << qPrintable( fsw_dirs.join( ", " ) ) << "anymore";
#endif
        mp_fsWatcher->removePaths( fsw_dirs );
      }
    }
    m_myCurrentFolder = new_path;
  }
}

void GuiShareBox::disableBox( VNumber user_id )
{
  GuiShareBoxFileInfoList* pfil = fileInfoList( user_id );
  pfil->clearTree();
  pfil->setToolTip( "" );
  pfil->setEnabled( false );
  if( user_id == ID_LOCAL_USER )
  {
    mp_lMyBox->setEnabled( false );
    mp_lMyBox->setToolTip( "" );
    mp_pbMyCreateFolder->setEnabled( false );
    mp_pbMyUpdate->setEnabled( false );
    mp_pbMyOpenFolder->setEnabled( false );

  }
  else
  {
    mp_lOutBox->setEnabled( false );
    mp_pbOutParentFolder->setEnabled( false );
    mp_pbOutUpdate->setEnabled( false );
    mp_pbOutCreateFolder->setEnabled( false );
  }
}

void GuiShareBox::enableBox( VNumber user_id )
{
  GuiShareBoxFileInfoList* pfil = fileInfoList( user_id );
  pfil->setToolTip( "" );
  pfil->setEnabled( true );

  if( user_id == ID_LOCAL_USER )
  {
    mp_pbMyCreateFolder->setEnabled( true );
    mp_pbMyOpenFolder->setEnabled( true );
    mp_pbMyUpdate->setEnabled( false );
    mp_lMyBox->setEnabled( true );
  }
  else
  {
    mp_pbOutParentFolder->setEnabled( !m_outCurrentFolder.isEmpty() );
    mp_pbOutUpdate->setEnabled( true );
    mp_pbOutCreateFolder->setEnabled( true );
    mp_lOutBox->setEnabled( true );
  }
}

void GuiShareBox::updateMyBox()
{
  if( Settings::instance().useShareBox() )
    makeShareBoxRequest( ID_LOCAL_USER, m_myCurrentFolder, false );
  else
    disableBox( ID_LOCAL_USER );
}

void GuiShareBox::updateOutBox()
{
  VNumber user_id = selectedUserId();
  if( Settings::instance().useShareBox() )
  {
    if( user_id != m_userId )
      makeShareBoxRequest( user_id, "", false );
    else
      makeShareBoxRequest( user_id, m_outCurrentFolder, false );
  }
  else
    disableBox( user_id );
}

void GuiShareBox::updateBox( const User& u, const QString& folder_path, const QList<FileInfo>& file_info_list )
{
  if( u.isLocal() )
  {
    if( mp_cbEnableMyBox->isChecked() )
      updateMyBox( folder_path, file_info_list );
    else
      disableBox( ID_LOCAL_USER );
  }
  else
  {
    if( mp_cbEnableMyBox->isChecked() )
      updateOutBox( u, folder_path, file_info_list );
    else
      disableBox( u.id() );
  }
}

void GuiShareBox::updateMyBox( const QString& folder_path, const QList<FileInfo>& file_info_list )
{
  enableBox( ID_LOCAL_USER );
  mp_myBox->setFileInfoList( file_info_list );
  if( !folder_path.isEmpty() )
    mp_myBox->addDotDotFolder();
  updateCurrentFolderLabel( ID_LOCAL_USER );
}

void GuiShareBox::updateOutBox( const User& u, const QString& folder_path, const QList<FileInfo>& file_info_list )
{
  if( m_userId != u.id() )
  {
#ifdef BEEBEEP_DEBUG
    qDebug() << "GuiSharebox skips updates for not current user" << qPrintable( u.name() );
#endif
    return;
  }

  if( !u.isStatusConnected() )
  {
    m_outCurrentFolder = "";
    updateCurrentFolderLabel( m_userId );
    return;
  }

  if( m_outCurrentFolder != folder_path )
  {
#ifdef BEEBEEP_DEBUG
    qDebug() << "GuiSharebox skips updates from" << qPrintable( u.name() ) << "for not current folder" << qPrintable( folder_path );
#endif
    return;
  }

  mp_outBox->setFileInfoList( file_info_list );
  if( !folder_path.isEmpty() )
    mp_outBox->addDotDotFolder();

  enableBox( m_userId );
  updateCurrentFolderLabel( m_userId );
}

void GuiShareBox::onMyItemDoubleClicked( QTreeWidgetItem* item, int )
{
  if( !item )
    return;

  GuiShareBoxFileInfoItem* file_info_item = dynamic_cast<GuiShareBoxFileInfoItem*>(item);

  if( file_info_item->isFolder() )
  {
    QString new_folder;
    if( file_info_item->fileInfo().id() == ID_DOTDOT_FOLDER )
      new_folder = Bee::folderCdUp( m_myCurrentFolder );
    else if( m_myCurrentFolder.isEmpty() )
      new_folder = QString( "%1" ).arg( file_info_item->fileInfo().name() );
    else
      new_folder = QString( "%1%2%3" ).arg( m_myCurrentFolder ).arg( QDir::separator() ).arg( file_info_item->fileInfo().name() );
    makeShareBoxRequest( ID_LOCAL_USER, new_folder, false );
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

  GuiShareBoxFileInfoItem* file_info_item = dynamic_cast<GuiShareBoxFileInfoItem*>(item);

  if( file_info_item->isFolder() )
  {
    QString new_folder;
    if( file_info_item->fileInfo().id() == ID_DOTDOT_FOLDER )
      new_folder = Bee::folderCdUp( m_outCurrentFolder );
    else if( m_outCurrentFolder.isEmpty() )
      new_folder = QString( "%1" ).arg( file_info_item->fileInfo().name() );
    else
      new_folder = QString( "%1%2%3" ).arg( m_outCurrentFolder ).arg( QDir::separator() ).arg( file_info_item->fileInfo().name() );
    makeShareBoxRequest( m_userId, new_folder, false );
  }
}

void GuiShareBox::onShareBoxSelected( int )
{
  VNumber current_user_id = selectedUserId();
  if( current_user_id > ID_LOCAL_USER )
  {
#ifdef BEEBEEP_DEBUG
    qDebug() << "BeeBOX requests list for user" << current_user_id;
#endif
    makeShareBoxRequest( current_user_id, current_user_id == m_userId ? m_outCurrentFolder : "", false );
  }
  else
  {
    setCurrentFolder( current_user_id, "" );
    disableBox( current_user_id );
    updateCurrentFolderLabel( current_user_id );
  }
}

void GuiShareBox::onShareFolderUnavailable( const User& u, const QString& folder_path )
{
#ifdef BEEBEEP_DEBUG
  qDebug() << qPrintable( u.path() ) << "has not shared box folder" << qPrintable( folder_path );
#endif
  if( u.isLocal() )
  {
    if( mp_cbEnableMyBox->isChecked() )
      QMessageBox::information( this, Settings::instance().programName(), tr( "%1: access denied." ).arg( folder_path ) );
  }
  else
  {
    if( u.id() != m_userId )
      return;

    if( folder_path != m_outCurrentFolder )
      return;

    mp_lOutBox->setText( tr( "%1 <b>%2</b>" ).arg( folder_path.isEmpty() ? QString( "BeeBOX" ) : folder_path ).arg( tr( "is unavailable" ) ) );
    mp_outBox->setToolTip( Bee::removeHtmlTags( mp_lOutBox->text() ) );
  }

  GuiShareBoxFileInfoList* fil = fileInfoList( u.id() );
  if( fil->isEnabled() )
    disableBox( u.id() );
}

void GuiShareBox::makeShareBoxRequest( VNumber user_id, const QString& folder_path, bool create_folder )
{
  disableBox( user_id );
  if( user_id == ID_INVALID )
    return;
  setCurrentFolder( user_id, folder_path );
  QLabel* current_folder_label = currentFolderLabel( user_id );

  if( Settings::instance().useShareBox() )
  {
#ifdef BEEBEEP_DEBUG
    qDebug() << "BeeBOX ask for updating folder" << folder_path << "of user" << user_id;
#endif
    GuiShareBoxFileInfoList* pfil = fileInfoList( user_id );
    if( folder_path.isEmpty() )
      pfil->setToolTip( tr( "Please wait" ) );
    else
      pfil->setToolTip( tr( "Please wait for path %1" ).arg( folder_path ) );

    current_folder_label->setText( QString( "<b>%1...</b>" ).arg( tr( "Please wait" ) ) );
    current_folder_label->setToolTip( pfil->toolTip() );

    emit shareBoxRequest( user_id, folder_path, create_folder );
  }
  else
    current_folder_label->setText( QString( "<b>%1</b>" ).arg( tr( "Disabled" ) ) );
}

void GuiShareBox::updateUser( const User& u )
{
  if( u.isLocal() )
    return;

  int user_index = mp_comboUsers->findData( u.id() );
  if( user_index >= 0 )
  {
    if( !u.isStatusConnected() )
    {
      if( mp_comboUsers->currentIndex() == user_index )
      {
#ifdef BEEBEEP_DEBUG
        qDebug() << qPrintable( u.path() ) << "is offline and ShareBox will be disabled";
#endif
        m_outCurrentFolder = "";
        onShareFolderUnavailable( u, m_outCurrentFolder );
      }
      mp_comboUsers->removeItem( user_index );
    }
    else
      mp_comboUsers->setItemText( user_index, Bee::userNameToShow( u, false ) );
  }
  else
  {
    if( u.isStatusConnected() )
      mp_comboUsers->addItem( Bee::userNameToShow( u, false ), u.id() );
  }
}

void GuiShareBox::dropInMyBox( const QString& share_path )
{
  QStringList sl_paths = share_path.split( "\n" );
  QList<FileInfo> selected_list = mp_outBox->selectedFileInfoList();
  if( sl_paths.size() != selected_list.size() )
    qWarning() << "BeeBOX (mybox) has found drop list size" << sl_paths.size() << "not equal to selected list size" << selected_list.size();

  foreach( FileInfo file_info, selected_list )
  {
#ifdef BEEBEEP_DEBUG
    QString from_path = file_info.shareFolder().isEmpty() ? file_info.name() : QString( "%1%2%3" ).arg( file_info.shareFolder() ).arg( QDir::separator() ).arg( file_info.name() );
    qDebug() << "Drop in MY BeeBOX the file" << file_info.name() << "->" << from_path;
#endif
    QString to_path;
    if( m_myCurrentFolder.isEmpty() )
      to_path = Bee::convertToNativeFolderSeparator( QString( "%1/%2" ).arg( Settings::instance().shareBoxPath(), file_info.name() ) );
    else
      to_path = Bee::convertToNativeFolderSeparator( QString( "%1/%2/%3" ).arg( Settings::instance().shareBoxPath(), m_myCurrentFolder, file_info.name() ) );

    emit shareBoxDownloadRequest( m_userId, file_info, to_path );
  }
}

void GuiShareBox::dropInOutBox( const QString& share_path )
{
  QStringList sl_paths = share_path.split( "\n" );
  QList<FileInfo> selected_list = mp_myBox->selectedFileInfoList();
  if( sl_paths.size() != selected_list.size() )
    qWarning() << "BeeBOX (outbox) has found drop list size" << sl_paths.size() << "not equal to selected list size" << selected_list.size();

  foreach( FileInfo file_info, selected_list )
  {
    QString to_path = Bee::convertToNativeFolderSeparator( m_outCurrentFolder );
#ifdef BEEBEEP_DEBUG
    qDebug() << "Drop in OUT BeeBOX the file" << file_info.name() << "->" << to_path;
#endif
    emit shareBoxUploadRequest( m_userId, file_info, to_path );
  }
}

void GuiShareBox::onFileUploadCompleted( VNumber user_id, const FileInfo& fi )
{
#ifdef BEEBEEP_DEBUG
  qDebug() << "BeeBOX (upload completed) update list of the folder" << fi.shareFolder() << "and user" << user_id << "and current folder is" << m_outCurrentFolder << "of user" << m_userId;
#endif
  if( m_userId == user_id && fi.shareFolder() == m_outCurrentFolder )
    updateOutBox();
}

void GuiShareBox::onFileDownloadCompleted( VNumber user_id, const FileInfo& fi )
{
#ifdef BEEBEEP_DEBUG
  qDebug() << "BeeBOX (download completed) update list of the folder" << fi.shareFolder() << "and user" << user_id << "and current folder is" << m_outCurrentFolder << "of user" << m_userId;
#else
  Q_UNUSED( user_id );
  Q_UNUSED( fi );
#endif
  updateMyBox();
}

bool GuiShareBox::isValidNewFolderName( QTreeWidget* tw, const QString& name_to_check )
{
  QTreeWidgetItemIterator it( tw );
  while( *it )
  {
    if( (*it)->text( GuiShareBoxFileInfoItem::ColumnFile ).toLower() == name_to_check.toLower() )
      return false;
    ++it;
  }
  return true;
}

void GuiShareBox::createFolderInBox( VNumber user_id )
{
  if( user_id == ID_INVALID )
    return;

  GuiShareBoxFileInfoList* tw = fileInfoList( user_id );
  if( !tw->isEnabled() )
    return;

  QString current_folder = currentFolder( user_id );

  QString folder_to_create = QInputDialog::getText( tw, Settings::instance().programName(), tr( "Please insert the new folder name" ) );
  if( folder_to_create.isEmpty() )
    return;

  if( !isValidNewFolderName( tw, folder_to_create ) )
  {
    QMessageBox::information( tw, Settings::instance().programName(), tr( "%1 already exists." ).arg( folder_to_create ) );
    return;
  }

  QString new_folder_path = current_folder.isEmpty() ? folder_to_create : QString( "%1%2%3" ).arg( current_folder ).arg( QDir::separator() ).arg( folder_to_create );
  makeShareBoxRequest( user_id, new_folder_path, true );
}

void GuiShareBox::createFolderInMyBox()
{
  createFolderInBox( ID_LOCAL_USER );
}

void GuiShareBox::createFolderInOutBox()
{
  createFolderInBox( m_userId );
}

void GuiShareBox::openMyBox()
{
  QString folder_path = m_myCurrentFolder.isEmpty() ? Settings::instance().shareBoxPath() : QString( "%1%2%3" ).arg( Settings::instance().shareBoxPath() ).arg( QDir::separator() ).arg( m_myCurrentFolder );
  if( folder_path.isEmpty() )
  {
    mp_pbMyOpenFolder->setEnabled( false );
    return;
  }
  emit openUrlRequest( QUrl::fromLocalFile( folder_path ) );
}

void GuiShareBox::backToParentFolder()
{
  if( !m_outCurrentFolder.isEmpty() )
  {
    QString new_folder = Bee::folderCdUp( m_outCurrentFolder );
    makeShareBoxRequest( m_userId, new_folder, false );
  }
  else
    mp_pbOutParentFolder->setEnabled( false );
}

void GuiShareBox::updateFolder( const QString& f )
{
  if( Settings::instance().useShareBox() )
  {
#ifdef BEEBEEP_DEBUG
    qDebug() << "Folder" << qPrintable( f ) << "changed -> updating ShareBox";
#else
    Q_UNUSED( f );
#endif
    mp_pbMyUpdate->setEnabled( true );
  }
}
