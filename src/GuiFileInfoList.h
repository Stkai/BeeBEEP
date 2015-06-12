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
// $Id: GuiFileInfoItem.h 346 2015-04-05 16:12:37Z mastroddi $
//
//////////////////////////////////////////////////////////////////////

#ifndef BEEBEEP_GUIFILEINFOLIST_H
#define BEEBEEP_GUIFILEINFOLIST_H

#include "Config.h"
#include "GuiFileInfoItem.h"
class User;


class GuiFileInfoList
{
public:
  GuiFileInfoList();

  void initTree( QTreeWidget* );

  GuiFileInfoItem* userItem( VNumber );
  GuiFileInfoItem* createUserItem( const User& );
  GuiFileInfoItem* folderItem( VNumber, const QString& );
  GuiFileInfoItem* createFolderItem( const User&, const QString& );
  GuiFileInfoItem* fileItem( VNumber, VNumber );
  GuiFileInfoItem* createFileItem( const User&, const FileInfo& );

private:
  QTreeWidget* mp_tree;

};



#endif // BEEBEEP_GUIFILEINFOLIST_H
