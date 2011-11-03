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

#ifndef BEEBEEP_NETWORKACCOUNT_H
#define BEEBEEP_NETWORKACCOUNT_H

#include "Config.h"


class NetworkAccount
{
public:
  NetworkAccount();
  NetworkAccount( const NetworkAccount& );

  NetworkAccount& operator=( const NetworkAccount& );

  inline bool isValid() const;
  inline void setService( const QString& );
  inline const QString& service() const;
  inline void setUser( const QString& );
  inline const QString& user() const;
  inline void setPassword( const QString& );
  inline const QString& password() const;
  inline void setAutoConnect( bool );
  inline bool autoConnect() const;
  inline void setSaveUser( bool );
  inline bool saveUser() const;
  inline void setSavePassword( bool );
  inline bool savePassword() const;

  QString toString() const;
  bool fromString( const QString& );

private:
  QString m_service;
  QString m_user;
  QString m_password;
  bool m_autoConnect;
  bool m_saveUser;
  bool m_savePassword;

};


// Inline Functions
inline bool NetworkAccount::isValid() const { return m_service.size() > 0; }
inline void NetworkAccount::setService( const QString& new_value ) { m_service = new_value; }
inline const QString& NetworkAccount::service() const { return m_service; }
inline void NetworkAccount::setUser( const QString& new_value ) { m_user = new_value; }
inline const QString& NetworkAccount::user() const { return m_user; }
inline void NetworkAccount::setPassword( const QString& new_value ) { m_password = new_value; }
inline const QString& NetworkAccount::password() const { return m_password; }
inline void NetworkAccount::setAutoConnect( bool new_value ) { m_autoConnect = new_value; }
inline bool NetworkAccount::autoConnect() const { return m_autoConnect; }
inline void NetworkAccount::setSaveUser( bool new_value ) { m_saveUser = new_value; }
inline bool NetworkAccount::saveUser() const { return m_saveUser; }
inline void NetworkAccount::setSavePassword( bool new_value ) { m_savePassword = new_value; }
inline bool NetworkAccount::savePassword() const { return m_savePassword; }

#endif // BEEBEEP_NETWORKACCOUNT_H
