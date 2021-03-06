//////////////////////////////////////////////////////////////////////
//
// BeeBEEP Copyright (C) 2010-2021 Marco Mastroddi
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
  inline const QString& savedChatsAuthCode() const;
  inline const QString& unsentMessagesAuthCode() const;
  inline qint64 elapsedTime() const;
  inline int protocolVersion() const;

signals:
  void listCompleted();

public slots:
  void buildList();

protected:
  void loadSavedChats( QDataStream* );
  void loadUnsentMessages();
  QString checkAuthCodeFromFileHeader( const QStringList& file_header, const QString& file_name );
  void clearCacheItems();
  void removePartiallyDownloadedFiles();

private:
  QMap<QString, QString> m_savedChats;
  QList<MessageRecord> m_unsentMessages;
  QString m_savedChatsAuthCode;
  QString m_unsentMessagesAuthCode;
  qint64 m_elapsedTime;
  int m_protocolVersion;

};


// Inline Functions
inline const QMap<QString, QString>& BuildSavedChatList::savedChats() const { return m_savedChats; }
inline const QList<MessageRecord>& BuildSavedChatList::unsentMessages() const { return m_unsentMessages; }
inline const QString& BuildSavedChatList::savedChatsAuthCode() const { return m_savedChatsAuthCode; }
inline const QString& BuildSavedChatList::unsentMessagesAuthCode() const { return m_unsentMessagesAuthCode; }
inline qint64 BuildSavedChatList::elapsedTime() const { return m_elapsedTime; }
inline int BuildSavedChatList::protocolVersion() const { return m_protocolVersion; }

#endif // BEEBEEP_BUILDSAVEDCHATLIST_H
