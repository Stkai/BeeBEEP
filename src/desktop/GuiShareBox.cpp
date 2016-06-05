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

  mp_lTitle->setText( QString( "<b>%1</b>" ).arg( tr( "ShareBox" ) ) );

  mp_myBox->initTree();
  mp_outBox->initTree();

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
  connect( mp_myBox, SIGNAL( itemDoubleClicked( QTreeWidgetItem*, int ) ), this, SLOT( onMyItemDoubleClicked( QTreeWidgetItem*, int ) ) );
  connect( mp_outBox, SIGNAL( itemDoubleClicked( QTreeWidgetItem*, int ) ), this, SLOT( onOutItemDoubleClicked( QTreeWidgetItem*, int ) ) );
  connect( mp_myBox, SIGNAL( dropEventRequest( const QString& ) ), this, SLOT( dropInMyBox( const QString& ) ) );
  connect( mp_outBox, SIGNAL( dropEventRequest( const QString& ) ), this, SLOT( dropInOutBox( const QString& ) ) );
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
    mp_myBox->clearTree();
    m_myCurrentFolder = "";
    mp_lMyBox->setToolTip( "" );
    mp_lMyBox->setText( tr( "Your ShareBox is disabled" ) );
  }
}

void GuiShareBox::updateOutBox()
{
  mp_pbOutUpdate->setEnabled( false );
#if QT_VERSION >= 0x050000
  VNumber user_id = mp_comboUsers->count() > 0 ? Bee::qVariantToVNumber( mp_comboUsers->currentData() ) : ID_INVALID;
#else
  VNumber user_id = mp_comboUsers->count() > 0 ? Bee::qVariantToVNumber( mp_comboUsers->itemData( mp_comboUsers->currentIndex() ) ) : ID_INVALID;
#endif
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
    mp_outBox->clearTree();
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
  mp_myBox->setFileInfoList( file_info_list );
  if( !folder_path.isEmpty() )
    mp_myBox->addDotDotFolder();

  QString s_title = QString( "<b>.%1</b>" ).arg( m_myCurrentFolder.isEmpty() ? "/" : m_myCurrentFolder );
  mp_lMyBox->setText( Bee::convertToNativeFolderSeparator( s_title ) );
}

void GuiShareBox::updateOutBox( const User& u, const QString& folder_path, const QList<FileInfo>&  file_info_list )
{
  m_userId = u.id();
  m_outCurrentFolder = folder_path;
  mp_outBox->setFileInfoList( file_info_list );
  if( !folder_path.isEmpty() )
    mp_outBox->addDotDotFolder();

  QString s_title = QString( "<b>.%1</b>" ).arg( m_outCurrentFolder.isEmpty() ? "/" : m_outCurrentFolder );
  mp_lOutBox->setText( Bee::convertToNativeFolderSeparator( s_title ) );

  mp_comboUsers->blockSignals( true );
  if( mp_comboUsers->findData( u.id() ) == -1 )
    mp_comboUsers->addItem( u.name(), u.id() );

  mp_comboUsers->setEnabled( mp_comboUsers->count() > 0 );
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
  QString folder_path = FileDialog::getExistingDirectory( this,
                                                          tr( "%1 - Select the ShareBox folder" )
                                                          .arg( Settings::instance().programName() ),
                                                                Settings::instance().shareBoxPath() );
  if( folder_path.isEmpty() )
    return;

  Settings::instance().setShareBoxPath( folder_path );
  Settings::instance().save();
  m_myCurrentFolder = "";
  updateMyBox();
}

void GuiShareBox::onShareBoxSelected( int )
{
#if QT_VERSION >= 0x050000
  VNumber current_user_id = Bee::qVariantToVNumber( mp_comboUsers->currentData() );
#else
  VNumber current_user_id = Bee::qVariantToVNumber( mp_comboUsers->itemData( mp_comboUsers->currentIndex() ) );
#endif
  if( current_user_id != m_userId )
  {
    m_outCurrentFolder = "";
    m_userId = current_user_id;
    mp_outBox->clearTree();
#ifdef BEEBEEP_DEBUG
    qDebug() << "ShareBox requests list for user" << current_user_id;
#endif
    emit shareBoxRequest( current_user_id, m_outCurrentFolder );
  }
}

void GuiShareBox::onShareFolderUnavailable( const User& u, const QString& folder_path )
{
#ifdef BEEBEEP_DEBUG
  qDebug() << u.path() << "has not shared box folder" << folder_path;
#endif
  if( u.isLocal() )
    return;

#if QT_VERSION >= 0x050000
  if( u.id() == Bee::qVariantToVNumber( mp_comboUsers->currentData() ) )
#else
  if( u.id() == Bee::qVariantToVNumber( mp_comboUsers->itemData( mp_comboUsers->currentIndex() ) ) )
#endif
  {
    mp_lOutBox->setText( tr( "%1 <b>%2</b>" ).arg( folder_path.isEmpty() ? tr( "ShareBox" ) : folder_path ).arg( tr( "is unavailable" ) ) );
    if( folder_path == m_outCurrentFolder )
      mp_outBox->clearTree();
  }
}

void GuiShareBox::dropInMyBox( const QString& share_path )
{
  QStringList sl_paths = share_path.split( "\n" );
  QList<FileInfo> selected_list = mp_outBox->selectedFileInfoList();
  if( sl_paths.size() != selected_list.size() )
    qWarning() << "ShareBox (mybox) has found drop list size" << sl_paths.size() << "not equal to selected list size" << selected_list.size();

  foreach( FileInfo file_info, selected_list )
  {
#ifdef BEEBEEP_DEBUG
    QString from_path = Bee::convertToNativeFolderSeparator( QString( "%1/%2" ).arg( file_info.shareFolder(), file_info.name() ) );
    qDebug() << "Drop in MY sharebox the file" << file_info.name() << "->" << from_path;
#endif
    QString to_path = Bee::convertToNativeFolderSeparator( QString( "%1/%2" ).arg( m_myCurrentFolder, file_info.name() ) );
    to_path.prepend( Settings::instance().shareBoxPath() );
    to_path = Bee::convertToNativeFolderSeparator( to_path );
    emit shareBoxDownloadRequest( m_userId, file_info, to_path );
  }
}

void GuiShareBox::dropInOutBox( const QString& share_path )
{
  QStringList sl_paths = share_path.split( "\n" );
  QList<FileInfo> selected_list = mp_myBox->selectedFileInfoList();
  if( sl_paths.size() != selected_list.size() )
    qWarning() << "ShareBox (outbox) has found drop list size" << sl_paths.size() << "not equal to selected list size" << selected_list.size();

  foreach( FileInfo file_info, selected_list )
  {
    QString to_path = Bee::convertToNativeFolderSeparator( m_outCurrentFolder );
#ifdef BEEBEEP_DEBUG
    qDebug() << "Drop in OUT sharebox the file" << file_info.name() << "->" << to_path;
#endif
    emit shareBoxUploadRequest( m_userId, file_info, to_path );
  }
}

void GuiShareBox::updateUser( const User& u )
{
  int user_index = mp_comboUsers->findData( u.id() );
  if( user_index >= 0 )
  {
    if( !u.isStatusConnected() )
    {
      if( mp_comboUsers->currentIndex() == user_index )
      {
        mp_outBox->clearTree();
        m_outCurrentFolder = "";
      }
      mp_comboUsers->removeItem( user_index );
    }
    else
      mp_comboUsers->setItemText( user_index, u.name() );
  }
  else
  {
    if( u.isStatusConnected() )
      mp_comboUsers->addItem( u.name(), u.id() );
  }

  mp_comboUsers->setEnabled( mp_comboUsers->count() > 0 );
}

void GuiShareBox::onFileUploadCompleted( VNumber user_id, const QString& folder_path )
{
#ifdef BEEBEEP_DEBUG
  qDebug() << "ShareBox update list of the folder" << folder_path << "and user" << user_id;
#endif
  if( m_userId == user_id && folder_path == m_outCurrentFolder )
    updateOutBox();
}

void GuiShareBox::onFileDownloadCompleted( const QString& folder_path )
{
#ifdef BEEBEEP_DEBUG
  qDebug() << "ShareBox update list of the folder" << folder_path;
#endif
  if( folder_path == m_myCurrentFolder )
    updateMyBox();
}
