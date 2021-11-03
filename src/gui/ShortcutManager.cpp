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

#include "ShortcutManager.h"

ShortcutManager* ShortcutManager::mp_instance = Q_NULLPTR;


ShortcutManager::ShortcutManager()
  : m_shortcuts( NumShortcut, QKeySequence() ), m_shortcutNames( NumShortcut )
{
  setDefaultShortcuts();
}

QStringList ShortcutManager::saveToStringList() const
{
  QStringList sl;
  QKeySequence ks;
  for( int i = 0; i < NumShortcut; i++ )
  {
    ks = shortcut( i );
    sl.append( ks.toString() );
    qDebug() << "Save shortcut type" << i << shortcutName( i ) << "with key sequence:" << qPrintable( ks.isEmpty() ? "none" : ks.toString() );
  }
  return sl;
}

void ShortcutManager::loadFromStringList( const QStringList& sl )
{
#ifdef BEEBEEP_DEBUG
  qDebug() << "Loading" << sl.size() << "shortcuts:" << qPrintable( sl.join( ", " ) );
#endif
  int shortcut_type = Empty;

  foreach( QString s, sl )
  {
    if( shortcut_type != Empty )
    {
      if( setShortcut( shortcut_type, s ) )
        qDebug() << "Load shortcut type" << shortcut_type << shortcutName( shortcut_type ) << "with key sequence:" << qPrintable( shortcut( shortcut_type ).isEmpty() ? "none" : shortcut( shortcut_type ).toString() );
      else
        qWarning() << "Unable to load shortcut string:" << s;
    }

    shortcut_type++;
    if( shortcut_type >= NumShortcut )
      break;
  }
}

QString ShortcutManager::shortcutKey( const QKeySequence& ks ) const
{
#ifdef Q_OS_MAC
  return ks.toString().replace( QString( "Ctrl" ), QString( "Cmd" ) );
#else
  return ks.toString().replace( QString( "Cmd" ), QString( "Ctrl" ) );
#endif
}

bool ShortcutManager::setShortcut( int st, const QString& s )
{
  if( st > 0 && st < NumShortcut )
  {
    QString s_key = s;
#ifdef Q_OS_MAC
    s_key.replace( QString( "Cmd" ), QString( "Ctrl" ) );
#endif
    QKeySequence ks = QKeySequence::fromString( s_key );
    m_shortcuts[ st ] = ks;
    return true;
  }
  else
    return false;
}

void ShortcutManager::setDefaultShortcuts()
{
  qDebug() << "Loading default shortcuts";

  m_shortcutNames[ Empty ] = QObject::tr( "Empty" );
  m_shortcuts[ Empty ] = QKeySequence();
  m_shortcutNames[ SendFile ] = QObject::tr( "Send file" );
  m_shortcuts[ SendFile ] = QKeySequence( Qt::CTRL | Qt::Key_S );
  m_shortcutNames[ ShowFileTransfers ] = QObject::tr( "Show file transfers" );
  m_shortcuts[ ShowFileTransfers ] = QKeySequence( Qt::CTRL | Qt::Key_J );
  m_shortcutNames[ SetFocusInMessageBox ] = QObject::tr( "Set focus in message box" );
  m_shortcuts[ SetFocusInMessageBox ] = QKeySequence( Qt::CTRL | Qt::Key_L );
  m_shortcutNames[ MinimizeAllChats ] = QObject::tr( "Minimize all chats" );
  m_shortcuts[ MinimizeAllChats ] = QKeySequence( Qt::CTRL | Qt::Key_M );
  m_shortcutNames[ ShowNextUnreadMessage ] = QObject::tr( "Show the next unread message" );
  m_shortcuts[ ShowNextUnreadMessage ] = QKeySequence( Qt::CTRL | Qt::Key_Tab );
  m_shortcutNames[ SendChatMessage ] = QObject::tr( "Send chat message" );
  m_shortcuts[ SendChatMessage ] = QKeySequence( Qt::CTRL | Qt::Key_Return );
  m_shortcutNames[ Print ] = QObject::tr( "Print" );
  m_shortcuts[ Print ] = QKeySequence( QKeySequence::Print );
  m_shortcutNames[ Broadcast ] = QObject::tr( "Broadcast" );
  m_shortcuts[ Broadcast ] = QKeySequence( Qt::CTRL | Qt::Key_B );
  m_shortcutNames[ FindTextInChat ] = QObject::tr( "Find text in chat" );
  m_shortcuts[ FindTextInChat ] = QKeySequence( Qt::CTRL | Qt::Key_F );
  m_shortcutNames[ FindNextTextInChat ] = QObject::tr( "Find next text in chat" );
  m_shortcuts[ FindNextTextInChat ] = QKeySequence( Qt::CTRL | Qt::Key_G );
  m_shortcutNames[ SendFolder ] = QObject::tr( "Send folder" );
  m_shortcuts[ SendFolder ] = QKeySequence( Qt::CTRL | Qt::SHIFT | Qt::Key_S );
  m_shortcutNames[ ShowEmoticons ] = QObject::tr( "Show emoticons panel" );
  m_shortcuts[ ShowEmoticons ] = QKeySequence( Qt::CTRL | Qt::Key_E );
  m_shortcutNames[ ShowAllChats ] = QObject::tr( "Show all chats" );
  m_shortcuts[ ShowAllChats ] = QKeySequence( Qt::CTRL | Qt::ALT | Qt::Key_B );
  m_shortcutNames[ SendHelpMessage ] = QObject::tr( "Send a help request message" );
  m_shortcuts[ SendHelpMessage ] = QKeySequence( Qt::CTRL | Qt::SHIFT | Qt::Key_1 );
  m_shortcutNames[ SelectFirstChat ] = QObject::tr( "Select the first available chat" );
  m_shortcuts[ SelectFirstChat ] = QKeySequence( Qt::CTRL | Qt::ALT | Qt::Key_C );


#ifdef BEEBEEP_DEBUG
  for( int i = Empty; i < NumShortcut; i++ )
    qDebug() << "Default shortcut" << i << shortcutName( (ShortcutType)i ) << "with key sequence:" << qPrintable( m_shortcuts.at( i ).toString() );
#endif
}
