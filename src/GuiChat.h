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

#ifndef BEEBEEP_GUICHAT_H
#define BEEBEEP_GUICHAT_H

#include "ui_GuiChat.h"
#include "UserList.h"
class Chat;
class ChatMessage;
class Emoticon;


class GuiChat : public QWidget, private Ui::GuiChatWidget
{
  Q_OBJECT

public:
  GuiChat( QWidget* parent = 0 );

  void setupToolBar( QToolBar* );
  void updateAction( bool is_connected, int connected_users );

  bool setChatId( VNumber, bool );
  inline VNumber chatId() const;
  void updateUser( const User& );
  bool hasUser( VNumber );

  void reloadChatUsers();
  inline bool reloadChat();
  inline const QString& chatName() const;
  void ensureLastMessageVisible();

  void updateShortcuts();

  inline QSplitter* chatSplitter() const;

signals:
  void newMessage( VNumber, const QString& );
  void writing( VNumber );
  void nextChat();
  void openUrl( const QUrl& );
  void sendFileFromChatRequest( VNumber, const QString& );
  void createGroupRequest();
  void createChatRequest();
  void createGroupFromChatRequest( VNumber );
  void editGroupRequestFromChat( VNumber );
  void chatToClear( VNumber );
  void leaveThisChat( VNumber );
  void showChatMenuRequest();
  void showVCardRequest( VNumber, bool );
  void detachChatRequest( VNumber );
  void saveStateAndGeometryRequest();

public slots:
  void appendChatMessage( VNumber, const ChatMessage& );
  void addEmoticon( const Emoticon& );
  void ensureFocusInChat();

protected:
  void enableDetachButtons();
  void setLastMessageTimestamp( const QDateTime& );
  void setChatUsers();
  QString chatMessageToText( const ChatMessage& );
  bool messageCanBeShowed( const ChatMessage& );
  bool historyCanBeShowed();
  void setChatFont( const QFont& );
  void setChatFontColor( const QString& );
  void setChatBackgroundColor( const QString& );
  User findUser( VNumber );
  bool isActiveUser( const Chat&, const User& ) const;
  void dragEnterEvent( QDragEnterEvent* );
  void dropEvent( QDropEvent* );

private slots:
  void sendMessage();
  void checkWriting();
  void customContextMenu( const QPoint& );
  void showChatMessageFilterMenu();
  void changeChatMessageFilter();
  void checkAnchorClicked( const QUrl& );
  void selectFontColor();
  void selectBackgroundColor();
  void selectFont();
  void saveChat();
  void clearChat();
  void leaveThisGroup();
  void sendFile();
  void showUserVCard();
  void showMembersMenu();
  void showLocalUserVCard();
  void showGroupWizard();
  void detachThisChat();
  void editChatMembers();
  void checkAndSendUrls( const QMimeData* );
  void checkAndSendImage( const QMimeData* );

private:
  VNumber m_chatId;
  QString m_chatName;
  UserList m_chatUsers;
  VNumber m_lastMessageUserId;
  bool m_isFloating;

  QMenu* mp_menuChat;
  QAction* mp_actSendFile;
  QAction* mp_actGroupAdd;
  QAction* mp_actClear;
  QAction* mp_actLeave;
  QAction* mp_actCreateGroup;
  QAction* mp_actCreateGroupChat;
  QAction* mp_actSelectBackgroundColor;
  QAction* mp_actGroupWizard;

  QMenu* mp_menuMembers;
  QSplitter* mp_splitter;

  QPalette m_defaultChatPalette;

  QShortcut* mp_scFocusInChat;

};


// Inline Functions
inline VNumber GuiChat::chatId() const { return m_chatId; }
inline const QString& GuiChat::chatName() const { return m_chatName; }
inline bool GuiChat::reloadChat() { return setChatId( m_chatId, m_isFloating ); }
inline QSplitter* GuiChat::chatSplitter() const { return mp_splitter; }

#endif // BEEBEEP_GUICHAT_H
