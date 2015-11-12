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

#include "ShortcutManager.h"

ShortcutManager* ShortcutManager::mp_instance = NULL;


ShortcutManager::ShortcutManager()
  : m_shortcuts( NumShortcut, QKeySequence() ), m_shortcutNames( NumShortcut )
{
  m_shortcutNames.fill( QObject::tr( "Empty" ) );
  m_shortcutNames[ SendFile ] = QObject::tr( "Send file" );
  m_shortcutNames[ ShowFileTransfers ] = QObject::tr( "Show file transfers" );
}

QStringList ShortcutManager::saveToStringList() const
{
  QStringList sl;
  QKeySequence ks;
  for( int i = 0; i < NumShortcut; i++ )
  {
    ks = shortcut( (ShortcutType)i );
    sl.append( ks.toString() );
    qDebug() << "Save shortcut type" << i << shortcutName( (ShortcutType)i ) << "with key sequence:" << qPrintable( ks.toString() );
  }
  return sl;
}

void ShortcutManager::loadFromStringList( const QStringList& sl )
{
#ifdef BEEBEEP_DEBUG
  qDebug() << "Loading" << sl.size() << "shortcuts:" << qPrintable( sl.join( ", " ) );
#endif
  QKeySequence empty_ks;
  m_shortcuts.fill( empty_ks, NumShortcut );

  int shortcut_type = Empty;

  foreach( QString s, sl )
  {
    QKeySequence ks = QKeySequence::fromString( s );
    m_shortcuts[ shortcut_type ] = ks;
    qDebug() << "Load shortcut type" << shortcut_type << shortcutName( (ShortcutType)shortcut_type ) << "with key sequence:" << qPrintable( ks.toString() );

    shortcut_type++;
    if( shortcut_type >= NumShortcut )
      break;
  }
}

void ShortcutManager::setDefaultShortcuts()
{
  qDebug() << "Load default shortcuts";
  QKeySequence ks_sendFile( Qt::CTRL | Qt::SHIFT | Qt::Key_F );
  QKeySequence ks_showFileTransfer( Qt::CTRL | Qt::SHIFT | Qt::Key_T );

  m_shortcuts[ SendFile ] = ks_sendFile;
  m_shortcuts[ ShowFileTransfers ] = ks_showFileTransfer;

  for( int i = 0; i < NumShortcut; i++ )
    qDebug() << "Default shortcut" << i << shortcutName( (ShortcutType)i ) << "with key sequence:" << qPrintable( m_shortcuts.at( i ).toString() );
}
