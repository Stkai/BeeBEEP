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

#ifndef BEEBEEP_USER_H
#define BEEBEEP_USER_H

#include "Config.h"


class User
{
public:
  enum Status { Offline, Online, Busy, Away, NumStatus };
  User();
  User( const User& );
  explicit User( VNumber );

  User& operator=( const User& );
  inline bool operator==( const User& ) const;

  inline bool isValid() const;
  inline void setId( VNumber );
  inline VNumber id() const;
  inline void setName( const QString& );
  inline const QString& name() const;
  inline void setNickname( const QString& );
  inline const QString& nickname() const;
  inline void setPeerAddress( const QHostAddress& );
  inline const QHostAddress& peerAddress() const;
  inline void setPeerPort( int );
  inline int peerPort() const;
  inline void setListenerPort( int );
  inline int listenerPort() const;
  inline void setStatus( int );
  inline int status() const;
  inline void setStatusDescription( const QString& );
  inline const QString& statusDescription() const;
  inline void setColor( const QString& );
  inline const QString& color() const;

  inline QString path() const;

private:
  VNumber m_id;
  QString m_name;
  QString m_nickname;
  QHostAddress m_peerAddress;
  int m_peerPort;
  int m_listenerPort;
  int m_status;
  QString m_statusDescription;
  QString m_color;

};


// Inline Functions
inline bool User::operator==( const User& u ) const { return m_id == u.m_id; }
inline bool User::isValid() const { return m_id > 0; }
inline void User::setId( VNumber new_value ) { m_id = new_value; }
inline VNumber User::id() const { return m_id; }
inline void User::setName( const QString& new_value ) { m_name = new_value; }
inline const QString& User::name() const { return m_name; }
inline void User::setNickname( const QString& new_value ) { m_nickname = new_value; }
inline const QString& User::nickname() const { return m_nickname; }
inline void User::setPeerAddress( const QHostAddress& new_value ) { m_peerAddress = new_value; }
inline const QHostAddress& User::peerAddress() const { return m_peerAddress; }
inline void User::setPeerPort( int new_value ) { m_peerPort = new_value; }
inline int User::peerPort() const { return m_peerPort; }
inline void User::setListenerPort( int new_value ) { m_listenerPort = new_value; }
inline int User::listenerPort() const { return m_listenerPort; }
inline void User::setStatus( int new_value ) { m_status = new_value; }
inline int User::status() const { return m_status; }
inline void User::setStatusDescription( const QString& new_value ) { m_statusDescription = new_value; }
inline const QString& User::statusDescription() const { return m_statusDescription; }
inline void User::setColor( const QString& new_value ) { m_color = new_value; }
inline const QString& User::color() const { return m_color; }
inline QString User::path() const { return QString( "%1 (%2@%3:%4)" ).arg( m_nickname, m_name, m_peerAddress.toString(), QString::number( m_listenerPort ) ); }

#endif // BEEBEEP_USER_H
