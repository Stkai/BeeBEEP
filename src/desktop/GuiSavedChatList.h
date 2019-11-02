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

#ifndef BEEBEEP_GUISAVEDCHATLIST_H
#define BEEBEEP_GUISAVEDCHATLIST_H

#include "GuiSavedChatItem.h"
#include "ui_GuiSavedChatList.h"


class GuiSavedChatList : public QWidget, private Ui::GuiSavedChatListWidget
{
  Q_OBJECT

public:
  GuiSavedChatList( QWidget* parent  );

  inline void setMainToolTip( const QString& );

  void updateBackground();

signals:
  void savedChatSelected( const QString& );
  void savedChatRemoved( const QString& );
  void savedChatLinkRequest( const QString& );

public slots:
  void updateSavedChats();

protected slots:
  void savedChatClicked( QTreeWidgetItem*, int );
  void showSavedChatMenu( const QPoint& );
  void showSavedChatSelected();
  void removeSavedChatSelected();
  void linkSavedChatSelected();
  void filterText( const QString& );
  void clearFilter();
  void selectBackgroundColor();

private:
  QString m_savedChatSelected;
  QMenu* mp_menuContext;
  bool m_blockShowChatRequest;
  QString m_filter;

};


// Inline Functions
inline void GuiSavedChatList::setMainToolTip( const QString& new_value ) { mp_twSavedChatList->setToolTip( new_value ); }

#endif // BEEBEEP_GUISAVEDCHATLIST_H
