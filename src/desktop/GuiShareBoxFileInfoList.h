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

#ifndef BEEBEEP_GUISHAREBOXFILEINFOLIST_H
#define BEEBEEP_GUISHAREBOXFILEINFOLIST_H

#include "Config.h"
#include "GuiShareBoxFileInfoItem.h"
#include "FileInfo.h"


class GuiShareBoxFileInfoList : public QTreeWidget
{
  Q_OBJECT

public:
  GuiShareBoxFileInfoList( QWidget* parent );

  void setFileInfoList( const QList<FileInfo>& );
  QList<FileInfo> selectedFileInfoList() const;

  void addDotDotFolder();

  int countFileItems() const;
  inline bool isEmpty() const;

  void initTree();

signals:
  void dropEventRequest( const QString& );

public slots:
  void clearTree();

protected:
  void mousePressEvent( QMouseEvent* );
  void mouseMoveEvent( QMouseEvent* );
  void dragEnterEvent( QDragEnterEvent* );
  void dragMoveEvent( QDragMoveEvent* );
  void dropEvent( QDropEvent* );
  bool performDrag();

private:
  QPoint m_dragStartPoint;

};

// Inline Functions
inline bool GuiShareBoxFileInfoList::isEmpty() const { return topLevelItemCount() == 0; }

#endif // BEEBEEP_GUISHAREBOXFILEINFOLIST_H
