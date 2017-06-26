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

  void setupToolBar( QToolBar* chat_bar );
  void updateActions( const Chat&, bool is_connected, int connected_users );

  bool setChat( const Chat& );
  inline VNumber chatId() const;
  void ensureLastMessageVisible();
  void updateShortcuts();
  void updateActionsOnFocusChanged();
  void setChatFont( const QFont& );
  bool appendChatMessage( const Chat&, const ChatMessage& );

  inline QSplitter* chatSplitter() const;
  void editChatMembers();

  void onTickEvent( int );

signals:
  void newMessage( VNumber, const QString& );
  void writing( VNumber );
  void nextChat();
  void openUrl( const QUrl& );
  void sendFileFromChatRequest( VNumber, const QString& );
  void editGroupRequest( VNumber );
  void chatToClear( VNumber );
  void showChatMenuRequest();
  void saveStateAndGeometryRequest();
  void toggleVisibilityEmoticonsPanelRequest();
  void toggleVisibilityPresetMessagesPanelRequest();
  void hideRequest();
#ifdef BEEBEEP_USE_SHAREDESKTOP
  void shareDesktopToChatRequest( VNumber, bool );
  void screenshotToChatRequest( VNumber );
#endif

public slots:
  void addEmoticon( const Emoticon& );
  void ensureFocusInChat();
  void addText( const QString& );

protected:
  void setLastMessageTimestamp( const QDateTime& );
  QString chatMessageToText( const ChatMessage& );
  bool messageCanBeShowed( const ChatMessage& );
  bool historyCanBeShowed();
  void setChatFontColor( const QString& );
  void setChatBackgroundColor( const QString& );
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
  void sendFile();
  void sendFolder();
  void checkAndSendUrls( const QMimeData* );
  void checkAndSendImage( const QMimeData* );
  void onUseReturnToSendMessageClicked();
  void onSpellCheckerActionClicked();
  void onCompleterActionClicked();
  void printChat();
  void showFindTextInChatDialog();
  void findNextTextInChat();
  void openSelectedTextAsUrl();
  void resetChatFontToDefault();
#ifdef BEEBEEP_USE_SHAREDESKTOP
  void shareDesktopToChat();
  void sendScreenshotToChat();
  void sendScreenshotToChat_Private();
  void enableScreenshotAction();
#endif

protected:
  void findTextInChat( const QString& );
  void updateChat();

private:
  VNumber m_chatId;
  VNumber m_lastMessageUserId;

  QMenu* mp_menuContext;
  QMenu* mp_menuFilters;
  QAction* mp_actSendFile;
  QAction* mp_actClear;
  QAction* mp_actSelectBackgroundColor;
  QAction* mp_actSpellChecker;
  QAction* mp_actCompleter;
  QAction* mp_actUseReturnToSendMessage;
  QAction* mp_actPrint;
  QAction* mp_actFindTextInChat;
  QAction* mp_actSendFolder;
  QAction* mp_actSaveAs;
  QAction* mp_actRestoreDefaultFont;
#ifdef BEEBEEP_USE_SHAREDESKTOP
  QAction* mp_actShareDesktop;
  QAction* mp_actScreenshot;
#endif

  QSplitter* mp_splitter;

  QPalette m_defaultChatPalette;

  QShortcut* mp_scFocusInChat;
  QShortcut* mp_scFindNextTextInChat;
  QShortcut* mp_scViewEmoticons;
  QString m_lastTextFound;

};


// Inline Functions
inline VNumber GuiChat::chatId() const { return m_chatId; }
inline QSplitter* GuiChat::chatSplitter() const { return mp_splitter; }

#endif // BEEBEEP_GUICHAT_H
