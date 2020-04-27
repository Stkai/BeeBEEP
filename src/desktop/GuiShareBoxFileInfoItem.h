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

#ifndef BEEBEEP_GUISHAREBOXFILEINFOITEM_H
#define BEEBEEP_GUISHAREBOXFILEINFOITEM_H

#include "Config.h"
#include "FileInfo.h"


class GuiShareBoxFileInfoItem : public QTreeWidgetItem
{
public:
  enum ColumnType { ColumnFile, ColumnSize, ColumnLastModified };

  GuiShareBoxFileInfoItem( QTreeWidget* );

  bool operator<( const QTreeWidgetItem& ) const;

  inline bool isValid() const;
  inline bool isFolder() const;
  inline bool isFile() const;

  inline const FileInfo& fileInfo() const;
  void setFileInfo( const FileInfo& );

private:
  FileInfo m_fileInfo;

};


// Inline functions
inline bool GuiShareBoxFileInfoItem::isValid() const { return m_fileInfo.isValid(); }
inline bool GuiShareBoxFileInfoItem::isFolder() const { return m_fileInfo.isFolder(); }
inline bool GuiShareBoxFileInfoItem::isFile() const { return !m_fileInfo.isFolder(); }
inline const FileInfo& GuiShareBoxFileInfoItem::fileInfo() const { return m_fileInfo; }

#endif // BEEBEEP_GUISHAREBOXFILEINFOITEM_H
