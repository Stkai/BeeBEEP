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

#ifndef BEEBEEP_VCARD_H
#define BEEBEEP_VCARD_H

#include "Config.h"


class VCard
{
public:
  enum Gender { Male, Female };

  VCard();
  VCard( const VCard& );

  VCard& operator=( const VCard& );
  inline bool operator==( const VCard& ) const;

  inline bool hasOnlyNickName() const;

  inline const QString& nickName() const;
  inline void setNickName( const QString& );
  inline const QString& firstName() const;
  inline void setFirstName( const QString& );
  inline const QString& lastName() const;
  inline void setLastName( const QString& );
  inline int gender() const;
  inline void setGender( Gender );
  inline bool isFemale() const;
  inline const QDate& birthday() const;
  inline void setBirthday( const QDate& );
  inline const QString& email() const;
  inline void setEmail( const QString& );
  inline const QPixmap& photo() const;
  inline void setPhoto( const QPixmap& );

private:
  QString m_nickName;
  QString m_firstName;
  QString m_lastName;
  int m_gender;
  QDate m_birthday;
  QString m_email;
  QPixmap m_photo;

};


// Inline Functions
inline const QString& VCard::nickName() const { return m_nickName; }
inline void VCard::setNickName( const QString& new_value ) { m_nickName = new_value; }
inline const QString& VCard::firstName() const { return m_firstName; }
inline void VCard::setFirstName( const QString& new_value ) { m_firstName = new_value; }
inline const QString& VCard::lastName() const { return m_lastName; }
inline void VCard::setLastName( const QString& new_value ) { m_lastName = new_value; }
inline int VCard::gender() const { return (int)m_gender; }
inline void VCard::setGender( Gender new_value ) { m_gender = new_value; }
inline bool VCard::isFemale() const { return m_gender == VCard::Female; }
inline const QDate& VCard::birthday() const { return m_birthday; }
inline void VCard::setBirthday( const QDate& new_value ) { m_birthday = new_value; }
inline const QString& VCard::email() const { return m_email; }
inline void VCard::setEmail( const QString& new_value ) { m_email = new_value; }
inline const QPixmap& VCard::photo() const { return m_photo; }
inline void VCard::setPhoto( const QPixmap& new_value ) { m_photo = new_value; }

inline bool VCard::hasOnlyNickName() const { return !m_firstName.isEmpty() || !m_lastName.isEmpty() || m_birthday.isValid() || !m_email.isEmpty() || !m_photo.isNull(); }
inline bool VCard::operator==( const VCard& vc ) const
{
  return m_nickName == vc.m_nickName && m_firstName == vc.m_firstName
      && m_lastName == vc.m_lastName && m_gender == vc.m_gender
      && m_birthday == vc.m_birthday && m_email == vc.m_email
      && m_photo.toImage() == vc.m_photo.toImage();
}


#endif // BEEBEEP_VCARD_H
