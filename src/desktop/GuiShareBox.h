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

#ifndef BEEBEEP_GUISHAREBOX_H
#define BEEBEEP_GUISHAREBOX_H

#include "ui_GuiShareBox.h"
#include "GuiShareBoxFileInfoList.h"
class User;


class GuiShareBox : public QWidget, private Ui::GuiShareBoxWidget
{
  Q_OBJECT

public:
  explicit GuiShareBox( QWidget *parent = Q_NULLPTR );

  void updateUser( const User& );

signals:
  void shareBoxRequest( VNumber, const QString&, bool );
  void openUrlRequest( const QUrl& );
  void shareBoxDownloadRequest( VNumber, const FileInfo&, const QString& );
  void shareBoxUploadRequest( VNumber, const FileInfo&, const QString& );

public slots:
  void updateShareBoxes();
  void updateBox( const User&, const QString&, const QList<FileInfo>& );
  void onShareFolderUnavailable(  const User&, const QString& );
  void onFileDownloadCompleted( VNumber, const FileInfo& );
  void onFileUploadCompleted( VNumber, const FileInfo& );

protected slots:
  void openMyBox();
  void updateMyBox();
  void updateOutBox();
  void backToParentFolder();
  void onMyItemDoubleClicked( QTreeWidgetItem*, int );
  void onOutItemDoubleClicked( QTreeWidgetItem*, int );
  void onEnableMyShareBoxClicked();
  void onShareBoxSelected( int );
  void selectMyShareBoxFolder();
  void createFolderInMyBox();
  void createFolderInOutBox();
  void createFolderInBox( VNumber );
  void dropInMyBox( const QString& );
  void dropInOutBox( const QString& );
  void updateFolder( const QString& );

protected:
  void updateMyBox( const QString&, const QList<FileInfo>& );
  void updateOutBox( const User&, const QString&, const QList<FileInfo>& );
  bool isValidNewFolderName( QTreeWidget*, const QString& );
  void disableBox( VNumber );
  void enableBox( VNumber );
  void makeShareBoxRequest( VNumber, const QString&, bool );
  void setCurrentFolder( VNumber, const QString& );
  void setUsers();
  inline const QString& currentFolder( VNumber ) const;
  inline GuiShareBoxFileInfoList* fileInfoList( VNumber ) const;
  inline QLabel* currentFolderLabel( VNumber ) const;
  VNumber selectedUserId() const;
  void updateCurrentFolderLabel( VNumber );

private:
  QAction* mp_actUpdate;
  QAction* mp_actDownload;
  QAction* mp_actUpload;
  VNumber m_userId;
  QString m_myCurrentFolder;
  QString m_outCurrentFolder;
  QSplitter* mp_splitter;

  QFileSystemWatcher* mp_fsWatcher;

};


// Inline Functions
inline const QString& GuiShareBox::currentFolder( VNumber user_id ) const { return user_id == ID_LOCAL_USER ? m_myCurrentFolder : m_outCurrentFolder; }
inline GuiShareBoxFileInfoList* GuiShareBox::fileInfoList( VNumber user_id ) const { return user_id == ID_LOCAL_USER ? mp_myBox : mp_outBox; }
inline QLabel* GuiShareBox::currentFolderLabel( VNumber user_id ) const { return user_id == ID_LOCAL_USER ? mp_lMyBox : mp_lOutBox; }

#endif // BEEBEEP_GUISHAREBOX_H
