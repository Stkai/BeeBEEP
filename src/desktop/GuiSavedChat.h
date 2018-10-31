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

#ifndef BEEBEEP_GUISAVEDCHAT_H
#define BEEBEEP_GUISAVEDCHAT_H

#include "Config.h"
#include "ui_GuiSavedChat.h"


class GuiSavedChat : public QDialog, private Ui::GuiSavedChatDialog
{
  Q_OBJECT

public:
  explicit GuiSavedChat( QWidget *parent = Q_NULLPTR );
  inline const QString& savedChatName() const;
  void updateShortcuts();

signals:
  void openUrl( const QUrl& );
  void deleteSavedChatRequest( const QString& );

public slots:
  void showSavedChat( const QString& );

protected slots:
  void printChat();
  void showFindTextInChatDialog();
  void findNextTextInChat();
  void customContextMenu( const QPoint& );
  void openSelectedTextAsUrl();
  void deleteSavedChat();

private:
  void findTextInChat( const QString& );

  QString m_savedChatName;
  QString m_lastTextFound;
  QShortcut* mp_scFindTextInChat;
  QShortcut* mp_scFindNextTextInChat;
  QShortcut* mp_scPrint;

  QMenu* mp_menuContext;

};

// Inline Functions
inline const QString& GuiSavedChat::savedChatName() const { return m_savedChatName; }

#endif // BEEBEEP_GUISAVEDCHAT_H
