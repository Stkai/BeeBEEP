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

#ifndef BEEBEEP_GUIUTILS_H
#define BEEBEEP_GUIUTILS_H

#include "Config.h"
class Chat;
class ChatMessage;


namespace Bee
{
  QString chatMessagesToText( const Chat& );
  QString chatMessageToText( const ChatMessage& );
  QIcon userStatusIcon( int );
  QString userStatusIconFileName( int );
  QString userStatusToString( int );
  inline QString iconToHtml( const QString& icon_path, const QString& icon_alt );
  inline VNumber qVariantToVNumber( const QVariant& );
  QString bytesToString( FileSizeType, int precision = 1 );
  QString timerToString( int msec );


  namespace Private
  {
    QString formatHtmlText( QString );
    QString formatMessage( const ChatMessage& );
    QString formatSystemMessage( const ChatMessage& );
  }
}


// Inline Functions
inline QIcon Bee::userStatusIcon( int user_status ) { return QIcon( userStatusIconFileName( user_status ) ); }
inline QString Bee::iconToHtml( const QString& icon_path, const QString& icon_alt ) { return QString( "<img src='%1' width=16 height=16 alt=' %2 ' /> " ).arg( icon_path ).arg( icon_alt ); }
inline VNumber Bee::qVariantToVNumber( const QVariant& v ) { return v.toULongLong(); }

#endif // BEEBEEP_GUIUTILS_H
