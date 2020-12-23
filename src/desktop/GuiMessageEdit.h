//////////////////////////////////////////////////////////////////////
//
// BeeBEEP Copyright (C) 2010-2021 Marco Mastroddi
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

#ifndef BEEBEEP_GUIMESSAGEEDIT_H
#define BEEBEEP_GUIMESSAGEEDIT_H

#include "Config.h"
#include "Emoticon.h"
#ifdef BEEBEEP_USE_HUNSPELL
  class SpellCheckerHighlighter;
#endif

class GuiMessageEdit : public QTextEdit
{
  Q_OBJECT

public:
  GuiMessageEdit( QWidget* parent );

  void addEmoticon( const Emoticon& );
  void addPasted( const QMimeData* );
  void addText( const QString& );

  QString message();
  void clearMessage();
  void addMessageToHistory();

  void rehighlightMessage();
  void setCompleter( QCompleter* );

  inline void addActionToContextMenu( QAction* );
  inline void setForceCRonEnterClicked( bool );
  inline void setForceNoWritingAlert( bool );

signals:
  void returnPressed();
  void writing();
  void urlsToCheck( const QMimeData* );
  void imageToCheck( const QMimeData* );

protected:
  void keyPressEvent( QKeyEvent* );
  void dropEvent( QDropEvent* );
  void contextMenuEvent( QContextMenuEvent* );
  bool nextMessageFromHistory();
  bool prevMessageFromHistory();
  inline int historySize() const;
  void setMessageFromHistory();
  void createMessageToSend();
  bool canInsertFromMimeData( const QMimeData* ) const;
  void insertFromMimeData( const QMimeData* );
  QString textUnderCursor() const;
  void focusInEvent( QFocusEvent* );
  virtual void completerKeyPressEvent( QKeyEvent* );

private slots:
  void checkWriting();
  void setUndoAvailable( bool );
  void setRedoAvailable( bool );
  void insertCompletion( const QString& );

private:
  QTimer* mp_timer;
  bool m_undoAvailable;
  bool m_redoAvailable;
  QList<Emoticon> m_emoticonsAdded;
  QString m_currentMessage;
  bool m_messageChanged;

#ifdef BEEBEEP_USE_HUNSPELL
  SpellCheckerHighlighter* mp_scHighlighter;
#endif

  QCompleter* mp_completer;

  QList<QAction*> m_actionsToContextMenu;
  QMenu* mp_menuContext;

  bool m_forceCRonEnterClicked;
  bool m_forceNoWritingAlert;

};

// Inline Functions
inline void GuiMessageEdit::addActionToContextMenu( QAction* act ) { m_actionsToContextMenu.append( act ); }
inline void GuiMessageEdit::setForceCRonEnterClicked( bool new_value ) { m_forceCRonEnterClicked = new_value; }
inline void GuiMessageEdit::setForceNoWritingAlert( bool new_value ) { m_forceNoWritingAlert = new_value; }

#endif // BEEBEEP_GUIMESSAGEEDIT_H
