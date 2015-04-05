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

#ifndef BEEBEEP_GUIMESSAGEEDIT_H
#define BEEBEEP_GUIMESSAGEEDIT_H

#include "Config.h"


class GuiMessageEdit : public QTextEdit
{
  Q_OBJECT

public:
  GuiMessageEdit( QWidget* parent );

  QString message() const;
  void clearMessage();

signals:
  void returnPressed();
  void writing();
  void tabPressed();

protected:
  void keyPressEvent( QKeyEvent* );
  void contextMenuEvent( QContextMenuEvent* );
  void addMessageToHistory();
  bool nextMessageFromHistory();
  bool prevMessageFromHistory();
  inline int historySize() const;
  void setMessageFromHistory();

private slots:
  void checkWriting();
  void setUndoAvailable( bool );
  void setRedoAvailable( bool );

private:
  QTimer* mp_timer;
  bool m_undoAvailable;
  bool m_redoAvailable;

  QStringList m_history;
  int m_historyIndex;
  QString m_lastMessage;

};


//Inline Functions
inline int GuiMessageEdit::historySize() const { return m_history.size()-1;}

#endif // BEEBEEP_GUIMESSAGEEDIT_H
