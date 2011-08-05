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

#ifndef BEEBEEP_PROTOCOL_H
#define BEEBEEP_PROTOCOL_H

#include "Message.h"
#include "User.h"
class FileInfo;


class Protocol
{
  // Singleton Object
  static Protocol* mp_instance;

public:
  inline int messageMinimumSize() const;
  QString fromMessage( const Message& ) const;
  Message toMessage( const QString& ) const;

  QString pingMessage() const;
  QString pongMessage() const;
  QString broadcastMessage() const;
  QString helloMessage() const;
  inline const Message& writingMessage() const;
  inline Message systemMessage( const QString& ) const;
  inline Message chatMessage( const QString& );
  Message fileInfoToMessage( const FileInfo& );
  FileInfo fileInfoFromMessage( const Message& );

  Message userStatusToMessage( const User& ) const;
  User userStatusFromMessage( User, const Message& ) const;

  User createUser( const Message& );
  inline User createLocalUser() const;

  QString encrypt( const QString&, int* num_chars_used_to_fill = NULL ) const;
  QString decrypt( const QString& ) const;

  QByteArray encryptByteArray( const QByteArray& ) const;
  QByteArray decryptByteArray( const QByteArray& ) const;

  static Protocol& instance()
  {
    if( !mp_instance )
      mp_instance = new Protocol();
    return *mp_instance;
  }

  static void close()
  {
    if( mp_instance )
    {
      delete mp_instance;
      mp_instance = NULL;
    }
  }

protected:
  Protocol();
  QString messageHeader( Message::Type ) const;
  Message::Type messageType( const QString& ) const;
  int newId();

private:
  int m_id;
  Message m_writingMessage;

};


// Inline Functions

inline int Protocol::messageMinimumSize() const { return 10; }
inline User Protocol::createLocalUser() const { return User( ID_LOCAL_USER ); }
inline const Message& Protocol::writingMessage() const { return m_writingMessage; }
inline Message Protocol::systemMessage( const QString& msg_txt ) const { return Message( Message::System, ID_SYSTEM_MESSAGE, msg_txt ); }
inline Message Protocol::chatMessage( const QString& msg_txt ) { return Message( Message::Chat, newId(), msg_txt ); }

#endif // BEEBEEP_PROTOCOL_H
