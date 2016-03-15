
CONFIG(debug,debug|release) {
  message( Building BeeBEEP in Debug Mode )
  DEFINES += BEEBEEP_DEBUG
} else {
  message( Building BeeBEEP in Release Mode )
}

message( Qt version: $$[QT_VERSION] )

include(../beebeep.pri)

message( Target folder: $$DESTDIR )

QT += network xml
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets multimedia printsupport

unix:!macx:!android: {
  LIBS= -lxcb -lxcb-screensaver
}

macx: {
  QMAKE_LFLAGS += -F/System/Library/Frameworks/ApplicationServices.framework
  LIBS += -framework ApplicationServices
}

win32: {
  DEFINES += _CRT_SECURE_NO_WARNINGS
  LIBS += -luser32
}

message( Qt modules: $$QT )

include(../locale/locale.pri)

TARGET = beebeep
TEMPLATE = app

include(hunspell/hunspell.pri)
include(override/override.pri)

HEADERS += AudioManager.h \
  Avatar.h \
  BeeApplication.h \
  BeeUtils.h \
  Broadcaster.h \
  BuildFileShareList.h \
  BuildSavedChatList.h \
  Chat.h \
  ChatManager.h \
  ChatMessage.h \
  ChatMessageData.h \
  ColorManager.h \
  Config.h \
  Connection.h \
  ConnectionSocket.h \
  Core.h \
  Emoticon.h \
  EmoticonManager.h \
  FileInfo.h \
  FileShare.h \
  FileTransfer.h \
  FileTransferPeer.h \
  GAnalytics.h \
  Group.h \
  GuiAddUser.h \
  GuiAskPassword.h \
  GuiChat.h \
  GuiChatItem.h \
  GuiChatList.h \
  GuiChatMessage.h \
  GuiCreateGroup.h \
  GuiConfig.h \
  GuiEditVCard.h \
  GuiEmoticons.h \
  GuiFileInfoItem.h \
  GuiFileInfoList.h \
  GuiFloatingChat.h \
  GuiGroupItem.h \
  GuiGroupList.h \
  GuiHome.h \
  GuiIconProvider.h \
  GuiLanguage.h \
  GuiLog.h \
  GuiMain.h \
  GuiMessageEdit.h \
  GuiPluginManager.h \
  GuiSavedChat.h \
  GuiSavedChatItem.h \
  GuiSavedChatList.h \
  GuiScreenShot.h \
  GuiSearchUser.h \
  GuiShareLocal.h \
  GuiShareNetwork.h \
  GuiShortcut.h \
  GuiSystemTray.h \
  GuiTransferFile.h \
  GuiUserItem.h \
  GuiUserList.h \
  GuiVCard.h \
  GuiWizard.h \
  HistoryManager.h \
  HistoryMessage.h \
  HttpDownloader.h \
  Interfaces.h \
  Listener.h \
  Log.h \
  Message.h \
  MessageManager.h \
  MessageRecord.h \
  NetworkAddress.h \
  NetworkEntry.h \
  NetworkManager.h \
  PluginManager.h \
  Protocol.h \
  Random.h \
  Rijndael.h \
  SaveChatList.h \
  ShortcutManager.h \
  Settings.h \
  SpellChecker.h \
  SpellCheckerHighlighter.h \
  Tips.h \
  Updater.h \
  User.h \
  UserList.h \
  UserManager.h \
  UserRecord.h \
  UserStatusRecord.h \
  VCard.h \
  Version.h \
  Job.h


SOURCES += AudioManager.cpp \
  Avatar.cpp \
  BeeApplication.cpp \
  BeeApplication_mac.cpp \
  BeeUtils.cpp \
  Broadcaster.cpp \
  BuildFileShareList.cpp \
  BuildSavedChatList.cpp \
  Chat.cpp \
  ChatManager.cpp \
  ChatMessage.cpp \
  ChatMessageData.cpp \
  ColorManager.cpp \
  Connection.cpp \
  ConnectionSocket.cpp \
  Core.cpp \
  CoreChat.cpp \
  CoreConnection.cpp \
  CoreDispatcher.cpp \
  CoreFileTransfer.cpp \
  CoreParser.cpp \
  CoreUser.cpp \
  Emojis.cpp \
  Emoticon.cpp \
  EmoticonManager.cpp \
  FileInfo.cpp \
  FileShare.cpp \
  FileTransfer.cpp \
  FileTransferDownload.cpp \
  FileTransferPeer.cpp \
  FileTransferUpload.cpp \
  GAnalytics.cpp \
  Group.cpp \
  GuiAddUser.cpp \
  GuiAskPassword.cpp \
  GuiChat.cpp \
  GuiChatItem.cpp \
  GuiChatList.cpp \
  GuiChatMessage.cpp \
  GuiCreateGroup.cpp \
  GuiEditVCard.cpp \
  GuiEmoticons.cpp \
  GuiFileInfoItem.cpp \
  GuiFileInfoList.cpp \
  GuiFloatingChat.cpp \
  GuiGroupItem.cpp \
  GuiGroupList.cpp \
  GuiHome.cpp \
  GuiIconProvider.cpp \
  GuiLanguage.cpp \
  GuiLog.cpp \
  GuiMain.cpp \
  GuiMessageEdit.cpp \
  GuiPluginManager.cpp \
  GuiSavedChat.cpp \
  GuiSavedChatItem.cpp \
  GuiSavedChatList.cpp \
  GuiScreenShot.cpp \
  GuiSearchUser.cpp \
  GuiShareLocal.cpp \
  GuiShareNetwork.cpp \
  GuiShortcut.cpp \
  GuiSystemTray.cpp \
  GuiTransferFile.cpp \
  GuiUserItem.cpp \
  GuiUserList.cpp \
  GuiVCard.cpp \
  GuiWizard.cpp \
  HttpDownloader.cpp \
  HistoryManager.cpp \
  HistoryMessage.cpp \
  Listener.cpp \
  Log.cpp \
  Main.cpp \
  Message.cpp \
  MessageManager.cpp \
  MessageRecord.cpp \
  NetworkAddress.cpp \
  NetworkEntry.cpp \
  NetworkManager.cpp \
  PluginManager.cpp \
  Protocol.cpp \
  Rijndael.cpp \
  SaveChatList.cpp \
  ShortcutManager.cpp \
  Settings.cpp \
  SpellChecker.cpp \
  SpellCheckerHighlighter.cpp \
  Updater.cpp \
  User.cpp \
  UserList.cpp \
  UserManager.cpp \
  UserRecord.cpp \
  UserStatusRecord.cpp \
  VCard.cpp


FORMS += GuiAddUser.ui \
  GuiAskPassword.ui \
  GuiChat.ui \
  GuiCreateGroup.ui \
  GuiEditVCard.ui \
  GuiHome.ui \
  GuiLanguage.ui \
  GuiLog.ui \
  GuiPluginManager.ui \
  GuiSavedChat.ui \
  GuiScreenShot.ui \
  GuiSearchUser.ui \
  GuiShareLocal.ui \
  GuiShareNetwork.ui \
  GuiShortcut.ui \
  GuiUserList.ui \
  GuiVCard.ui \
  GuiWizard.ui


RESOURCES += beebeep.qrc emojis.qrc

win32: RC_FILE = beebeep.rc
macx: ICON = beebeep.icns

message( Libs: $$LIBS )
