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

#ifndef BEEBEEP_USERRECORD_H
#define BEEBEEP_USERRECORD_H

#include "NetworkAddress.h"


class UserRecord
{
public:
  UserRecord();
  UserRecord( const UserRecord& );

  UserRecord& operator=( const UserRecord& );
  bool operator<( const UserRecord& ) const;
  inline bool operator==( const UserRecord& ) const;

  inline bool isValid() const;
  inline QString path() const;

  inline void setName( const QString& );
  inline const QString& name() const;
  inline void setAccount( const QString& );
  inline const QString& account() const;
  inline void setNetworkAddress( const NetworkAddress& );
  inline const NetworkAddress& networkAddress() const;
  inline void setComment( const QString& );
  inline const QString& comment() const;
  inline void setFavorite( bool );
  inline bool isFavorite() const;

private:
  QString m_name;
  QString m_account;
  NetworkAddress m_networkAddress;
  QString m_comment;
  bool m_isFavorite;

};


// Inline Functions
inline bool UserRecord::operator==( const UserRecord& ur ) const { return m_networkAddress == ur.m_networkAddress; }
inline bool UserRecord::isValid() const { return m_networkAddress.isHostAddressValid() && m_networkAddress.isHostPortValid(); }
inline void UserRecord::setName( const QString& new_value ) { m_name = new_value; }
inline const QString& UserRecord::name() const { return m_name; }
inline void UserRecord::setAccount( const QString& new_value ) { m_account = new_value; }
inline const QString& UserRecord::account() const { return m_account; }
inline QString UserRecord::path() const { return QString( "%1@%2" ).arg( m_name ).arg( m_networkAddress.toString() ); }
inline void UserRecord::setNetworkAddress( const NetworkAddress& new_value ) { m_networkAddress = new_value; }
inline const NetworkAddress& UserRecord::networkAddress() const { return m_networkAddress; }
inline void UserRecord::setComment( const QString& new_value ) { m_comment = new_value; }
inline const QString& UserRecord::comment() const { return m_comment; }
inline void UserRecord::setFavorite( bool new_value ) { m_isFavorite = new_value; }
inline bool UserRecord::isFavorite() const { return m_isFavorite; }

#endif // BEEBEEP_USERRECORD_H
