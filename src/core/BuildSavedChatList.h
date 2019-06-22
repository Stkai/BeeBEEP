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

#ifndef BEEBEEP_BUILDSAVEDCHATLIST_H
#define BEEBEEP_BUILDSAVEDCHATLIST_H

#include "MessageRecord.h"


class BuildSavedChatList : public QObject
{
  Q_OBJECT

public:
  explicit BuildSavedChatList( QObject* parent = Q_NULLPTR );

  inline const QMap<QString, QString>& savedChats() const;
  inline const QList<MessageRecord>& unsentMessages() const;
  inline int elapsedTime() const;

signals:
  void listCompleted();

public slots:
  void buildList();

protected:
  void loadSavedChats( QDataStream* );
  void loadUnsentMessages();

private:
  QMap<QString, QString> m_savedChats;
  QList<MessageRecord> m_unsentMessages;
  int m_elapsedTime;

};


// Inline Functions
inline const QMap<QString, QString>& BuildSavedChatList::savedChats() const { return m_savedChats; }
inline const QList<MessageRecord>& BuildSavedChatList::unsentMessages() const { return m_unsentMessages; }
inline int BuildSavedChatList::elapsedTime() const { return m_elapsedTime; }

#endif // BEEBEEP_BUILDSAVEDCHATLIST_H
