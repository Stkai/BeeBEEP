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

#ifndef BEEBEEP_GUISHAREBOXFILEINFOLIST_H
#define BEEBEEP_GUISHAREBOXFILEINFOLIST_H

#include "Config.h"
#include "GuiShareBoxFileInfoItem.h"
#include "FileInfo.h"
class User;


class GuiShareBoxFileInfoList : public QObject
{
  Q_OBJECT

public:
  GuiShareBoxFileInfoList();

  void initTree( QTreeWidget* );

  void setFileInfoList( const QList<FileInfo>& );
  QList<FileInfo> selectedFileInfoList() const;

  void addDotDotFolder();

  void clearTree();
  int countFileItems() const;
  inline bool isEmpty() const;

public slots:
  void clearTreeSelection();

private:
  QTreeWidget* mp_tree;

};

// Inline Functions
inline bool GuiShareBoxFileInfoList::isEmpty() const { return mp_tree->topLevelItemCount() == 0; }

#endif // BEEBEEP_GUISHAREBOXFILEINFOLIST_H
