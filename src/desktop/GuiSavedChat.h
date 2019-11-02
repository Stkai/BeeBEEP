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

#ifndef BEEBEEP_GUISAVEDCHAT_H
#define BEEBEEP_GUISAVEDCHAT_H

#include "Config.h"


class GuiSavedChat : public QMainWindow
{
  Q_OBJECT

public:
  explicit GuiSavedChat( QWidget *parent = Q_NULLPTR );
  inline const QString& savedChatName() const;

signals:
  void openUrl( const QUrl& );
  void deleteSavedChatRequest( const QString& );

public slots:
  void showSavedChat( const QString& );

protected slots:
  void printChat();
  void customContextMenu( const QPoint& );
  void openSelectedTextAsUrl();
  void deleteSavedChat();
  void saveHistoryAs();
  void findTextInHistory();
  void checkAnchorClicked( const QUrl& );

protected:
  void setupToolBar( QToolBar* );

private:
  QTextBrowser* mp_teSavedChat;
  QLineEdit* mp_leFilter;
  QCheckBox* mp_cbCaseSensitive;
  QCheckBox* mp_cbWholeWordOnly;
  QToolBar* mp_barHistory;
  QMenu* mp_menuContext;
  QString m_savedChatName;

};

// Inline Functions
inline const QString& GuiSavedChat::savedChatName() const { return m_savedChatName; }

#endif // BEEBEEP_GUISAVEDCHAT_H
