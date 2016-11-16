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
  void updateActions( bool is_connected, int connected_users );

  bool setChatId( VNumber, bool );
  inline VNumber chatId() const;
  void updateUser( const User& );
  bool hasUser( VNumber );

  void reloadChatUsers();
  bool reloadChat();
  inline const QString& chatName() const;
  void ensureLastMessageVisible();

  void updateShortcuts();
  void updateActionsOnFocusChanged();

  inline QSplitter* chatSplitter() const;

  void setChatReadByUser( VNumber );


signals:
  void newMessage( VNumber, const QString& );
  void writing( VNumber );
  void nextChat();
  void openUrl( const QUrl& );
  void sendFileFromChatRequest( VNumber, const QString& );
  void createGroupFromChatRequest( VNumber );
  void editGroupRequestFromChat( VNumber );
  void chatToClear( VNumber );
  void leaveThisChat( VNumber );
  void showChatMenuRequest();
  void showVCardRequest( VNumber, bool );
  void detachChatRequest( VNumber );
  void saveStateAndGeometryRequest();
  void toggleVisibilityEmoticonsPanelRequest();
  void toggleVisibilityPresetMessagesPanelRequest();
  void sendBuzzToUserRequest( VNumber );

public slots:
  void appendChatMessage( VNumber, const ChatMessage& );
  void addEmoticon( const Emoticon& );
  void ensureFocusInChat();
  void addText( const QString& );

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
  void updateSpellCheckerToolTip();
  void updateCompleterToolTip();
  void updateUseReturnKeyToSendMessageToolTip();

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
  void sendFolder();
  void showUserVCard();
  void showMembersMenu();
  void showLocalUserVCard();
  void showGroupWizard();
  void detachThisChat();
  void editChatMembers();
  void checkAndSendUrls( const QMimeData* );
  void checkAndSendImage( const QMimeData* );
  void onUseReturnToSendMessageClicked();
  void onSpellCheckerActionClicked();
  void onCompleterActionClicked();
  void printChat();
  void showFindTextInChatDialog();
  void findNextTextInChat();
  void openSelectedTextAsUrl();
  void sendBuzz();

protected:
  void findTextInChat( const QString& );

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
  QAction* mp_actSelectBackgroundColor;
  QAction* mp_actGroupWizard;
  QAction* mp_actSpellChecker;
  QAction* mp_actCompleter;
  QAction* mp_actUseReturnToSendMessage;
  QAction* mp_actPrint;
  QAction* mp_actFindTextInChat;
  QAction* mp_actSendFolder;
  QAction* mp_actSaveAs;
  QAction* mp_actBuzz;

  QMenu* mp_menuMembers;
  QSplitter* mp_splitter;

  QPalette m_defaultChatPalette;

  QShortcut* mp_scFocusInChat;
  QShortcut* mp_scFindNextTextInChat;
  QShortcut* mp_scViewEmoticons;
  QString m_lastTextFound;

};


// Inline Functions
inline VNumber GuiChat::chatId() const { return m_chatId; }
inline const QString& GuiChat::chatName() const { return m_chatName; }
inline QSplitter* GuiChat::chatSplitter() const { return mp_splitter; }

#endif // BEEBEEP_GUICHAT_H
