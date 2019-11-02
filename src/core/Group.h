//////////////////////////////////////////////////////////////////////
//
// BeeBEEP Copyright (C) 2010-2019 Marco Mastroddi
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

#ifndef BEEBEEP_GROUP_H
#define BEEBEEP_GROUP_H

#include "Config.h"


class Group
{
public:
  Group();
  Group( const Group& );

  Group& operator=( const Group& );
  inline bool operator==( const Group& ) const;
  inline bool isValid() const;

  inline VNumber id() const;
  inline void setId( VNumber );

  inline const QString& name() const;
  inline void setName( const QString& );

  inline const QList<VNumber>& usersId() const;
  bool addUser( VNumber );
  inline bool hasUser( VNumber ) const;
  bool hasUsers( const QList<VNumber>& ) const;
  inline bool removeUser( VNumber );
  void setUsers( const QList<VNumber>& );

  inline const QString& privateId() const;
  inline void setPrivateId( const QString& );

  inline const QDateTime& lastModified() const;
  inline void setLastModified( const QDateTime& );

  enum ChatType { DefaultChat, PrivateChat, GroupChat, NumChatTypes };
  inline void setChatType( Group::ChatType );
  inline Group::ChatType chatType() const;

private:
  VNumber m_id;
  QString m_name;
  QList<VNumber> m_usersId;
  QString m_privateId;
  QDateTime m_lastModified;
  ChatType m_chatType;

};


// Inline Functions
inline bool Group::operator==( const Group& c ) const { return m_id == c.m_id; }
inline bool Group::isValid() const { return m_id != ID_INVALID; }
inline VNumber Group::id() const { return m_id; }
inline void Group::setId( VNumber new_value ) { m_id = new_value; }
inline const QString& Group::name() const { return m_name; }
inline void Group::setName( const QString& new_value ) { m_name = new_value; }
inline const QList<VNumber>& Group::usersId() const { return m_usersId; }
inline bool Group::hasUser( VNumber user_id ) const { return m_usersId.contains( user_id ); }
inline bool Group::removeUser( VNumber user_id ) { return m_usersId.removeOne( user_id ); }
inline const QString& Group::privateId() const { return m_privateId; }
inline void Group::setPrivateId( const QString& new_value ) { m_privateId = new_value; }
inline const QDateTime& Group::lastModified() const { return m_lastModified; }
inline void Group::setLastModified( const QDateTime& new_value ) { m_lastModified = new_value; }
inline void Group::setChatType( Group::ChatType new_value ) { m_chatType = new_value; }
inline Group::ChatType Group::chatType() const { return m_chatType; }

#endif // BEEBEEP_GROUP_H
