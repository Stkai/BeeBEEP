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
// Author: Marco Mastroddi (marco.mastroddi(AT)gmail.com)
//
// $Id$
//
//////////////////////////////////////////////////////////////////////

#ifndef BEEBEEP_GUISAVEDCHATLIST_H
#define BEEBEEP_GUISAVEDCHATLIST_H

#include "GuiSavedChatItem.h"


class GuiSavedChatList : public QTreeWidget
{
  Q_OBJECT

public:
  GuiSavedChatList( QWidget* parent = 0 );
  virtual QSize sizeHint() const;

  void updateSavedChats();

signals:
  void savedChatSelected( const QString& );
  void savedChatRemoved( const QString& );
  void savedChatLinkRequest( const QString& );

protected slots:
  void savedChatDoubleClicked( QTreeWidgetItem*, int );
  void showSavedChatMenu( const QPoint& );
  void showSavedChatSelected();
  void removeSavedChatSelected();
  void linkSavedChatSelected();

private:
  QString m_savedChatSelected;
  QMenu* mp_menu;
  QAction* mp_actLink;

};


#endif // BEEBEEP_GUISAVEDCHATLIST_H
