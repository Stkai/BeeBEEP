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

#ifndef BEEBEEP_USER_H
#define BEEBEEP_USER_H

#include "VCard.h"


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
  inline bool isLocal() const;
  inline bool isConnected() const;
  inline void setId( VNumber );
  inline VNumber id() const;
  inline void setName( const QString& );
  inline const QString& name() const;
  inline void setHostAddress( const QHostAddress& );
  inline const QHostAddress& hostAddress() const;
  inline void setHostPort( int );
  inline int hostPort() const;
  inline void setStatus( int );
  inline int status() const;
  inline void setStatusDescription( const QString& );
  inline const QString& statusDescription() const;
  inline void setColor( const QString& );
  inline const QString& color() const;
  inline void setVCard( const VCard& );
  inline const VCard& vCard() const;
  inline void setAccountName( const QString& );
  inline const QString& accountName() const;
  inline void setVersion( const QString& );
  inline const QString& version() const;

  inline QString path() const;
  inline QString accountPath() const;
  inline QString hostAddressAndPort() const;

  bool isBirthDay() const;

  static QString nameFromPath( const QString& );
  static QString hostAddressAndPortFromPath( const QString& );

  bool operator<( const User& ) const;

private:
  VNumber m_id;
  VCard m_vCard;
  QHostAddress m_hostAddress; // his listener address
  int m_hostPort;             // his listener port  (address+port = unique id)
  int m_status;
  QString m_statusDescription;
  QString m_color;
  QString m_accountName;
  QString m_version;

};


// Inline Functions
inline bool User::operator==( const User& u ) const { return m_id == u.m_id; }
inline bool User::isValid() const { return m_id != ID_INVALID; }
inline bool User::isLocal() const { return m_id == ID_LOCAL_USER; }
inline bool User::isConnected() const { return m_status != User::Offline; }
inline void User::setId( VNumber new_value ) { m_id = new_value; }
inline VNumber User::id() const { return m_id; }
inline void User::setName( const QString& new_value ) { m_vCard.setNickName( new_value ); }
inline const QString& User::name() const { return m_vCard.nickName(); }
inline void User::setHostAddress( const QHostAddress& new_value ) { m_hostAddress = new_value; }
inline const QHostAddress& User::hostAddress() const { return m_hostAddress; }
inline void User::setHostPort( int new_value ) { m_hostPort = new_value; }
inline int User::hostPort() const { return m_hostPort; }
inline void User::setStatus( int new_value ) { m_status = new_value; }
inline int User::status() const { return m_status; }
inline void User::setStatusDescription( const QString& new_value ) { m_statusDescription = new_value; }
inline const QString& User::statusDescription() const { return m_statusDescription; }
inline void User::setColor( const QString& new_value ) { m_color = new_value; }
inline const QString& User::color() const { return m_color; }
inline void  User::setVCard( const VCard& new_value ) { m_vCard = new_value; }
inline const VCard& User::vCard() const { return m_vCard; }
inline void User::setAccountName( const QString& new_value ) { m_accountName = new_value; }
inline const QString& User::accountName() const { return m_accountName; }
inline void User::setVersion( const QString& new_value ) { m_version = new_value; }
inline const QString& User::version() const { return m_version; }
inline QString User::path() const { return QString( "%1@%2:%3" ).arg( name().toLower(), m_hostAddress.toString(), QString::number( m_hostPort ) ); }
inline QString User::accountPath() const { return QString( "%1@%2:%3" ).arg( m_accountName.toLower(), m_hostAddress.toString(), QString::number( m_hostPort ) ); }
inline QString User::hostAddressAndPort() const { return QString( "%1:%2" ).arg( m_hostAddress.toString(), QString::number( m_hostPort ) ); }

#endif // BEEBEEP_USER_H
