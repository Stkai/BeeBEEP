include(../beebeep.pri)

QT += network xml
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets multimedia printsupport

CONFIG(debug,debug|release) {
  message( Build BeeBEEP in Debug Mode )
  DEFINES += BEEBEEP_DEBUG
} else {
  message( Build BeeBEEP in Release Mode )
}

message(Qt version: $$[QT_VERSION])

unix:!macx {
  lessThan(QT_MAJOR_VERSION, 5): QT += phonon
  LIBS= -lxcb -lxcb-screensaver
}

win32: LIBS += -luser32

macx: {
  QMAKE_LFLAGS += -F/System/Library/Frameworks/ApplicationServices.framework
  LIBS += -framework ApplicationServices
  TARGET = BeeBEEP
} else {
  TARGET = beebeep
}

TEMPLATE = app

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
  NetworkManager.h \
  PluginManager.h \
  Protocol.h \
  Random.h \
  Rijndael.h \
  SaveChatList.h \
  ShortcutManager.h \
  ShutdownMonitor.h \
  Settings.h \
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
  NetworkManager.cpp \
  PluginManager.cpp \
  Protocol.cpp \
  Rijndael.cpp \
  SaveChatList.cpp \
  ShortcutManager.cpp \
  ShutdownMonitor.cpp \
  Settings.cpp \
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
  GuiUserList.ui \
  GuiVCard.ui \
  GuiWizard.ui


RESOURCES += beebeep.qrc emojis.qrc

win32: RC_FILE = beebeep.rc
macx: ICON = beebeep.icns

win32|macx|unix: DEFINES += BEEBEEP_USE_MULTICAST_DNS
win32|macx|unix: HEADERS += MDnsBrowser.h MDnsManager.h MDnsObject.h MDnsRecord.h MDnsRegister.h MDnsResolver.h
win32|macx|unix: SOURCES += MDnsBrowser.cpp MDnsManager.cpp MDnsObject.cpp MDnsRecord.cpp MDnsRegister.cpp MDnsResolver.cpp
win32: QMAKE_LFLAGS += /NODEFAULTLIB:libcmt
win32: LIBS += -L$$PWD/../mdns-lib/ -ldnssd
win32: INCLUDEPATH += $$PWD/../mdns-lib
win32: DEPENDPATH += $$PWD/../mdns-lib
win32:!win32-g++: PRE_TARGETDEPS += $$PWD/../mdns-lib/dnssd.lib
else:win32-g++: PRE_TARGETDEPS += $$PWD/../mdns-lib/libdnssd.a
unix:!macx: LIBS += -ldns_sd
