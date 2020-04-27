//////////////////////////////////////////////////////////////////////
//
// BeeBEEP Copyright (C) 2010-2020 Marco Mastroddi
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

#ifndef BEEBEEP_GUIFILEINFOLIST_H
#define BEEBEEP_GUIFILEINFOLIST_H

#include "Config.h"
#include "GuiFileInfoItem.h"
#include "FileInfo.h"
class User;


class GuiFileInfoList : public QObject
{
  Q_OBJECT

public:
  GuiFileInfoList();

  void initTree( QTreeWidget*, bool is_local );
  inline void setUpdatesEnabled( bool );

  GuiFileInfoItem* userItem( VNumber );
  GuiFileInfoItem* createUserItem( const User& );
  GuiFileInfoItem* folderItem( VNumber, const QString& );
  GuiFileInfoItem* createFolderItem( const User&, const QString& );
  GuiFileInfoItem* fileItem( VNumber, VNumber );
  GuiFileInfoItem* createFileItem( const User&, const FileInfo& );

  int parseSelectedItems();
  inline const QList<SharedFileInfo>& selectedFileInfoList() const;

  void clearTree();
  int countFileItems() const;
  inline bool isEmpty() const;

public slots:
  void clearTreeSelection();

protected:
  void addFileInfoToList( VNumber, const FileInfo& );
  void addItemToFileInfoList( GuiFileInfoItem* );
  void parseItem( QTreeWidgetItem* );
  FileInfo fileInfoFromNetwork( GuiFileInfoItem* );
  GuiFileInfoItem* createSubFolderItem( GuiFileInfoItem*, VNumber, const QString&, const QString& );

private:
  QTreeWidget* mp_tree;
  QList<SharedFileInfo> m_selectedFileInfoList;
  bool m_isLocal;

};

// Inline Functions
inline void GuiFileInfoList::setUpdatesEnabled( bool new_value ) { mp_tree->setUpdatesEnabled( new_value ); }
inline const QList<SharedFileInfo>& GuiFileInfoList::selectedFileInfoList() const { return m_selectedFileInfoList; }
inline bool GuiFileInfoList::isEmpty() const { return mp_tree->topLevelItemCount() == 0; }

#endif // BEEBEEP_GUIFILEINFOLIST_H
