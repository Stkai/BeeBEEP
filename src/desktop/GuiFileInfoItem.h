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

#ifndef BEEBEEP_GUIFILEINFOITEM_H
#define BEEBEEP_GUIFILEINFOITEM_H

#include "Config.h"
class FileInfo;


class GuiFileInfoItem : public QTreeWidgetItem
{
public:
  enum ObjectType { ObjectInvalid, ObjectUser, ObjectFolder, ObjectFile, ObjectNumTypes };
  enum ColumnType { ColumnFile, ColumnSize, ColumnStatus };

  GuiFileInfoItem( QTreeWidget* );
  GuiFileInfoItem( QTreeWidgetItem* );

  bool operator<( const QTreeWidgetItem& ) const;

  void initUser( VNumber user_id, const QString& );
  void initFolder( VNumber user_id, const QString&, const QString& );
  void initFile( VNumber user_id, const FileInfo& );

  inline bool isValid() const;
  inline bool isObjectUser() const;
  inline bool isObjectFolder() const;
  inline bool isObjectFile() const;

  inline VNumber userId() const;
  inline VNumber fileInfoId() const;
  inline FileSizeType fileSize() const;
  inline const QString& folder() const;
  inline const QString& filePath() const;
  inline void setFilePath( const QString& );

  int removeChildren();

private:
  ObjectType m_type;
  VNumber m_userId;
  VNumber m_fileInfoId;
  FileSizeType m_fileSize;
  QString m_folder;
  QString m_filePath;

};

// Inline functions
inline bool GuiFileInfoItem::isValid() const { return m_type == ObjectInvalid; }
inline bool GuiFileInfoItem::isObjectUser() const { return m_type == ObjectUser; }
inline bool GuiFileInfoItem::isObjectFolder() const { return m_type == ObjectFolder; }
inline bool GuiFileInfoItem::isObjectFile() const { return m_type == ObjectFile; }
inline VNumber GuiFileInfoItem::userId() const { return m_userId; }
inline VNumber GuiFileInfoItem::fileInfoId() const { return m_fileInfoId; }
inline FileSizeType GuiFileInfoItem::fileSize() const { return m_fileSize; }
inline const QString& GuiFileInfoItem::folder() const { return m_folder; }
inline const QString& GuiFileInfoItem::filePath() const { return m_filePath; }
inline void GuiFileInfoItem::setFilePath( const QString& new_value ) { m_filePath = new_value; }

#endif // BEEBEEP_GUIFILEINFOITEM_H
