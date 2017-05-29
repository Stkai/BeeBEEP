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

#include "UserRecord.h"
#include "VCard.h"


class User
{
public:
  enum Status { Offline, Online, Busy, Away, NumStatus };
  User();
  User( const User& );
  explicit User( VNumber );
  explicit User( VNumber, const UserRecord& );

  User& operator=( const User& );
  inline bool operator==( const User& ) const;
  bool operator<( const User& ) const;

  inline bool isValid() const;
  inline bool isLocal() const;
  inline bool isStatusConnected() const;
  inline void setId( VNumber );
  inline VNumber id() const;
  inline void setName( const QString& );
  inline const QString& name() const;
  inline void setNetworkAddress( const NetworkAddress& );
  inline const NetworkAddress& networkAddress() const;
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
  inline void setDomainName( const QString& );
  inline const QString& domainName() const;
  inline void setVersion( const QString& );
  inline const QString& version() const;
  inline void setHash( const QString& );
  inline const QString& hash() const;
  inline void setIsFavorite( bool );
  inline bool isFavorite() const;
  inline void setQtVersion( const QString& );
  inline const QString& qtVersion() const;
  inline void setProtocolVersion( int );
  inline int protocolVersion() const;
  inline void setStatusChangedIn( const QDateTime& );
  inline const QDateTime& statusChangedIn() const;

  inline QString path() const;
  inline QString accountPath() const;

  bool isBirthDay() const;

  static QString nameFromPath( const QString& );
  static QString hostAddressAndPortFromPath( const QString& );

private:
  VNumber m_id;
  VCard m_vCard;
  NetworkAddress m_networkAddress; // his listener address and port (address+port = unique id)
  int m_status;
  QString m_statusDescription;
  QString m_color;
  QString m_accountName;
  QString m_domainName;
  QString m_version;
  QString m_hash;
  bool m_isFavorite;
  QString m_qtVersion;
  int m_protocolVersion;
  QDateTime m_statusChangedIn;

};


// Inline Functions
inline bool User::operator==( const User& u ) const { return m_id == u.m_id; }
inline bool User::isValid() const { return m_id != ID_INVALID; }
inline bool User::isLocal() const { return m_id == ID_LOCAL_USER; }
inline bool User::isStatusConnected() const { return m_status > User::Offline && m_status < User::NumStatus; }
inline void User::setId( VNumber new_value ) { m_id = new_value; }
inline VNumber User::id() const { return m_id; }
inline void User::setName( const QString& new_value ) { m_vCard.setNickName( new_value ); }
inline const QString& User::name() const { return m_vCard.nickName(); }
inline void User::setNetworkAddress( const NetworkAddress& new_value ) { m_networkAddress = new_value; }
inline const NetworkAddress& User::networkAddress() const { return m_networkAddress; }
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
inline void User::setDomainName( const QString& new_value ) { m_domainName = new_value; }
inline const QString& User::domainName() const { return m_domainName; }
inline void User::setVersion( const QString& new_value ) { m_version = new_value; }
inline const QString& User::version() const { return m_version; }
inline void User::setHash( const QString& new_value ) { m_hash = new_value; }
inline const QString& User::hash() const { return m_hash; }
inline void User::setIsFavorite( bool new_value ) { m_isFavorite = new_value; }
inline bool User::isFavorite() const { return m_isFavorite; }
inline void User::setQtVersion( const QString& new_value ) { m_qtVersion = new_value; }
inline const QString& User::qtVersion() const { return m_qtVersion; }
inline void User::setProtocolVersion( int new_value ) { m_protocolVersion = new_value; }
inline int User::protocolVersion() const { return m_protocolVersion; }
inline void User::setStatusChangedIn( const QDateTime& new_value ) { m_statusChangedIn = new_value; }
inline const QDateTime& User::statusChangedIn() const { return m_statusChangedIn; }
inline QString User::path() const { return QString( "%1@%2" ).arg( name().toLower(), m_networkAddress.toString() ); }
inline QString User::accountPath() const { return m_domainName.isEmpty() ? m_accountName : QString( "%1@%2" ).arg( m_accountName, m_domainName ); }

#endif // BEEBEEP_USER_H
