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

  User createUser( const Message& );
  inline User createLocalUser() const;
  bool isUserAuthorized( const QString& nick_name, const QString& md5 );

  QString encrypt( const QString& ) const;
  QString decrypt( const QString& ) const;

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

private:
  int m_id;

};


// Inline Functions

inline int Protocol::messageMinimumSize() const { return 10; }
inline User Protocol::createLocalUser() const { return User( 1 ); }

#endif // BEEBEEP_PROTOCOL_H
