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
  UserRecord( const QString& user_name, const QString& user_account, const QString& user_hash );

  UserRecord& operator=( const UserRecord& );
  bool operator<( const UserRecord& ) const;

  inline bool networkAddressIsValid() const;

  inline void setName( const QString& );
  inline const QString& name() const;
  inline void setAccount( const QString& );
  inline const QString& account() const;
  inline void setNetworkAddress( const NetworkAddress& );
  inline const NetworkAddress& networkAddress() const;
  inline void setFavorite( bool );
  inline bool isFavorite() const;
  inline void setColor( const QString& );
  inline const QString& color() const;
  inline void setHash( const QString& );
  inline const QString& hash() const;

private:
  QString m_name;
  QString m_account;
  NetworkAddress m_networkAddress;
  bool m_isFavorite;
  QString m_color;
  QString m_hash;

};


// Inline Functions
inline bool UserRecord::networkAddressIsValid() const { return m_networkAddress.isHostAddressValid() && m_networkAddress.isHostPortValid(); }
inline void UserRecord::setName( const QString& new_value ) { m_name = new_value; }
inline const QString& UserRecord::name() const { return m_name; }
inline void UserRecord::setAccount( const QString& new_value ) { m_account = new_value; }
inline const QString& UserRecord::account() const { return m_account; }
inline void UserRecord::setNetworkAddress( const NetworkAddress& new_value ) { m_networkAddress = new_value; }
inline const NetworkAddress& UserRecord::networkAddress() const { return m_networkAddress; }
inline void UserRecord::setFavorite( bool new_value ) { m_isFavorite = new_value; }
inline bool UserRecord::isFavorite() const { return m_isFavorite; }
inline void UserRecord::setColor( const QString& new_value ) { m_color = new_value; }
inline const QString& UserRecord::color() const { return m_color; }
inline void UserRecord::setHash( const QString& new_value ) { m_hash = new_value; }
inline const QString& UserRecord::hash() const { return m_hash; }

#endif // BEEBEEP_USERRECORD_H
