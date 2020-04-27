//////////////////////////////////////////////////////////////////////
//
// BeeBEEP Copyright (C) 2010-2020 Marco Mastroddi
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
// along with BeeBEEP. If not, see <http://www.gnu.org/licenses/>.
//
// Author: Marco Mastroddi <marco.mastroddi(AT)gmail.com>
//
// $Id$
//
//////////////////////////////////////////////////////////////////////

#ifndef BEEBEEP_VCARD_H
#define BEEBEEP_VCARD_H

#include "Config.h"


class VCard
{
public:
  VCard();
  VCard( const VCard& );

  VCard& operator=( const VCard& );
  inline bool operator==( const VCard& ) const;

  inline bool isValid() const;
  inline bool hasOnlyNickName() const;

  inline const QString& nickName() const;
  inline void setNickName( const QString& );
  inline const QString& firstName() const;
  inline void setFirstName( const QString& );
  inline const QString& lastName() const;
  inline void setLastName( const QString& );
  inline QString fullName( bool first_name_first ) const;
  inline bool hasFullName() const;
  inline const QDate& birthday() const;
  inline void setBirthday( const QDate& );
  inline const QString& email() const;
  inline void setEmail( const QString& );
  inline const QPixmap& photo() const;
  void setPhoto( const QPixmap& );
  inline const QByteArray& photoHash() const;
  inline const QString& phoneNumber() const;
  inline void setPhoneNumber( const QString& );
  inline const QString& info() const;
  inline void setInfo( const QString& );

private:
  QString m_nickName;
  QString m_firstName;
  QString m_lastName;
  QDate m_birthday;
  QString m_email;
  QByteArray m_photoHash;
  QPixmap m_photo;
  QString m_phoneNumber;
  QString m_info;

};


// Inline Functions
inline bool VCard::isValid() const { return !m_nickName.isEmpty(); }
inline const QString& VCard::nickName() const { return m_nickName; }
inline void VCard::setNickName( const QString& new_value ) { m_nickName = new_value; }
inline const QString& VCard::firstName() const { return m_firstName; }
inline void VCard::setFirstName( const QString& new_value ) { m_firstName = new_value; }
inline const QString& VCard::lastName() const { return m_lastName; }
inline void VCard::setLastName( const QString& new_value ) { m_lastName = new_value; }
inline QString VCard::fullName( bool first_name_first ) const { return first_name_first ? QString( "%1 %2" ).arg( m_firstName, m_lastName ).simplified() : QString( "%1 %2" ).arg( m_lastName, m_firstName ).simplified(); }
inline const QDate& VCard::birthday() const { return m_birthday; }
inline void VCard::setBirthday( const QDate& new_value ) { m_birthday = new_value; }
inline const QString& VCard::email() const { return m_email; }
inline void VCard::setEmail( const QString& new_value ) { m_email = new_value; }
inline const QPixmap& VCard::photo() const { return m_photo; }
inline const QByteArray& VCard::photoHash() const { return m_photoHash; }
inline bool VCard::hasFullName() const { return !m_firstName.isEmpty() && !m_lastName.isEmpty(); }
inline bool VCard::hasOnlyNickName() const
{
  return m_firstName.isEmpty() && m_lastName.isEmpty() && !m_birthday.isValid() && m_email.isEmpty()
         && m_photoHash.isEmpty() && m_phoneNumber.isEmpty() && m_info.isEmpty();
}
inline bool VCard::operator==( const VCard& vc ) const
{
  return m_nickName == vc.m_nickName && m_firstName == vc.m_firstName
      && m_lastName == vc.m_lastName && m_birthday == vc.m_birthday
      && m_email == vc.m_email && m_photoHash == vc.m_photoHash
      && m_phoneNumber == vc.m_phoneNumber && m_info == vc.m_info;
}
inline const QString& VCard::phoneNumber() const { return m_phoneNumber; }
inline void VCard::setPhoneNumber( const QString& new_value ) { m_phoneNumber = new_value; }
inline const QString& VCard::info() const { return m_info; }
inline void VCard::setInfo( const QString& new_value ) { m_info = new_value; }

#endif // BEEBEEP_VCARD_H
