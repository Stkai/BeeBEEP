include(../beebeep.pri)

QT += network xml
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets multimedia

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
  GuiFileInfoItem.h \
  GuiFileInfoList.h \
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
  Interfaces.h \
  Listener.h \
  Log.h \
  Message.h \
  NetworkManager.h \
  PluginManager.h \
  Protocol.h \
  Random.h \
  Rijndael.h \
  SaveChatList.h \
  Settings.h \
  Tips.h \
  User.h \
  UserList.h \
  UserManager.h \
  UserRecord.h \
  VCard.h \
  Version.h \
  Job.h \
    GuiEmoticons.h

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
  GuiFileInfoItem.cpp \
  GuiFileInfoList.cpp \
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
  Listener.cpp \
  Log.cpp \
  Main.cpp \
  Message.cpp \
  NetworkManager.cpp \
  PluginManager.cpp \
  Protocol.cpp \
  Rijndael.cpp \
  SaveChatList.cpp \
  Settings.cpp \
  User.cpp \
  UserList.cpp \
  UserManager.cpp \
  UserRecord.cpp \
  VCard.cpp \
    GuiEmoticons.cpp

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


RESOURCES += beebeep.qrc \
    emojis.qrc
win32: RC_FILE = beebeep.rc

macx: ICON = beebeep.icns

