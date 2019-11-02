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

#ifndef BEEBEEP_CHATMESSAGE_H
#define BEEBEEP_CHATMESSAGE_H

#include "Config.h"
class Message;


class ChatMessage
{
public:
  enum Type { Header, System, Chat, Connection, UserInfo, FileTransfer, History, Other, ImagePreview, Autoresponder, Voice, NumTypes };

  ChatMessage();
  ChatMessage( const ChatMessage& );
  ChatMessage( VNumber user_id, const Message&, ChatMessage::Type, bool can_be_saved );
  ChatMessage( VNumber user_id, const QString& msg, ChatMessage::Type, bool can_be_saved );

  virtual ~ChatMessage() {}

  ChatMessage& operator=( const ChatMessage& );

  inline bool isValid() const;
  inline bool isFromSystem() const;
  inline bool isFromLocalUser() const;
  inline bool isFromAutoresponder() const;
  inline bool alertCanBeSent() const;
  inline bool isImportant() const;
  inline bool isHeader() const;
  inline bool isFileTransfer() const;
  inline bool isImagePreview() const;
  inline bool isVoice() const;

  bool isChatActivity() const;
  bool isSystemActivity() const;

  inline VNumber userId() const;
  inline const QString& message() const;
  inline const QDateTime& timestamp() const;
  inline const QColor& textColor() const;
  inline ChatMessage::Type type() const;

  inline bool canBeSaved() const;

protected:
  virtual void fromMessage( const Message& );

private:
  VNumber m_userId;
  QString m_message;
  QDateTime m_timestamp;
  QColor m_textColor;
  Type m_type;
  bool m_isImportant;
  bool m_canBeSaved;

};


// Inline Functions
inline bool ChatMessage::isValid() const { return m_userId != ID_INVALID; }
inline bool ChatMessage::isFromSystem() const { return m_userId == ID_SYSTEM_MESSAGE; }
inline bool ChatMessage::isFromLocalUser() const { return m_userId == ID_LOCAL_USER; }
inline bool ChatMessage::isFromAutoresponder() const { return m_type == ChatMessage::Autoresponder; }
inline bool ChatMessage::alertCanBeSent() const { return m_isImportant || (!isFromLocalUser() && !isFromSystem() && (m_type == ChatMessage::Chat || m_type == ChatMessage::FileTransfer ||
                                                                                                                     m_type == ChatMessage::ImagePreview || m_type == ChatMessage::Autoresponder ||
                                                                                                                     m_type == ChatMessage::Voice) ); }
inline bool ChatMessage::isImportant() const { return m_isImportant; }
inline bool ChatMessage::isHeader() const { return m_type == ChatMessage::Header; }
inline bool ChatMessage::isFileTransfer() const { return m_type == ChatMessage::FileTransfer; }
inline bool ChatMessage::isImagePreview() const { return m_type == ChatMessage::ImagePreview; }
inline bool ChatMessage::isVoice() const { return m_type == ChatMessage::Voice; }
inline VNumber ChatMessage::userId() const { return m_userId; }
inline const QString& ChatMessage::message() const { return m_message; }
inline const QDateTime& ChatMessage::timestamp() const { return m_timestamp; }
inline const QColor& ChatMessage::textColor() const { return m_textColor; }
inline ChatMessage::Type ChatMessage::type() const { return m_type; }
inline bool ChatMessage::canBeSaved() const { return m_canBeSaved; }

#endif // BEEBEEP_CHATMESSAGE_H

