include(../beebeep.pri)

QT += network xml
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets multimedia

CONFIG(debug,debug|release) {
  message( Build BeeBEEP in Debug Mode )
  DEFINES += BEEBEEP_DEBUG
} else {
  message( Build BeeBEEP in Release Mode )
}

win32: {
  DEFINES += BEEBEEP_USE_BONJOUR
  INCLUDEPATH += $$PWD/../bonjour
  DEPENDPATH += $$PWD/../bonjour
  LIBS += -L$$PWD/../bonjour/ -ldnssd
}

message(Qt version: $$[QT_VERSION])

unix:!macx {
  LIBS= -lxcb -lxcb-screensaver
}

macx: {
  QMAKE_LFLAGS += -F/System/Library/Frameworks/ApplicationServices.framework
  LIBS += -framework ApplicationServices
  TARGET = BeeBEEP
} else {
  TARGET = beebeep
}

TEMPLATE = app

SOURCES += Listener.cpp \
    FileTransferPeer.cpp \
    Connection.cpp \
    GuiChat.cpp \
    Message.cpp \
    Protocol.cpp \
    User.cpp \
    GuiUserList.cpp \
    GuiMessageEdit.cpp \
    Emoticon.cpp \
    EmoticonManager.cpp \
    Main.cpp \
    Settings.cpp \
    Chat.cpp \
    GuiMain.cpp \
    ChatMessage.cpp \
    BeeUtils.cpp \
    Rijndael.cpp \
    FileInfo.cpp \
    ConnectionSocket.cpp \
    FileTransferUpload.cpp \
    FileTransferDownload.cpp \
    FileTransfer.cpp \
    GuiTransferFile.cpp \
    ColorManager.cpp \
    Core.cpp \
    CoreParser.cpp \
    CoreConnection.cpp \
    CoreUser.cpp \
    CoreChat.cpp \
    CoreDispatcher.cpp \
    CoreFileTransfer.cpp \
    UserList.cpp \
    Broadcaster.cpp \
    VCard.cpp \
    GuiVCard.cpp \
    GuiEditVCard.cpp \
    GuiUserItem.cpp \
    PluginManager.cpp \
    GuiPluginManager.cpp \
    ChatMessageData.cpp \
    UserManager.cpp \
    ChatManager.cpp \
    GuiChatItem.cpp \
    GuiChatList.cpp \
    GuiSearchUser.cpp \
    GuiWizard.cpp \
    FileShare.cpp \
    GuiShareLocal.cpp \
    GuiShareNetwork.cpp \
    GuiSystemTray.cpp \
    GuiAskPassword.cpp \
    GuiLog.cpp \
    Log.cpp \
    GuiChatMessage.cpp \
    GuiSavedChatList.cpp \
    GuiSavedChatItem.cpp \
    GuiSavedChat.cpp \
    BeeApplication.cpp \
    GuiScreenShot.cpp \
    BuildFileShareList.cpp \
    BuildSavedChatList.cpp \
    SaveChatList.cpp \
    GuiFileInfoItem.cpp \
    GuiIconProvider.cpp \
    Group.cpp \
    GuiGroupItem.cpp \
    GuiGroupList.cpp \
    GuiCreateGroup.cpp \
    GuiLanguage.cpp \
    Avatar.cpp \
    GuiAddUser.cpp \
    UserRecord.cpp

win32: {
  SOURCES += BonjourBrowser.cpp \
    BonjourRecord.cpp \
    BonjourRegister.cpp \
    BonjourResolver.cpp
}

HEADERS += \
    Listener.h \
    Connection.h \
    Config.h \
    GuiChat.h \
    Message.h \
    Protocol.h \
    User.h \
    Version.h \
    GuiUserList.h \
    GuiMessageEdit.h \
    Emoticon.h \
    EmoticonManager.h \
    Settings.h \
    Chat.h \
    GuiMain.h \
    ChatMessage.h \
    BeeUtils.h \
    Rijndael.h \
    Log.h \
    FileInfo.h \
    ConnectionSocket.h \
    FileTransferPeer.h \
    FileTransferUpload.h \
    FileTransferDownload.h \
    FileTransfer.h \
    Tips.h \
    Random.h \
    GuiTransferFile.h \
    ColorManager.h \
    Core.h \
    UserList.h \
    Broadcaster.h \
    VCard.h \
    GuiVCard.h \
    GuiEditVCard.h \
    GuiUserItem.h \
    Interfaces.h \
    PluginManager.h \
    GuiPluginManager.h \
    ChatMessageData.h \
    UserManager.h \
    ChatManager.h \
    GuiChatItem.h \
    GuiChatList.h \
    GuiSearchUser.h \
    GuiWizard.h \
    FileShare.h \
    GuiShareLocal.h \
    GuiShareNetwork.h \
    GuiSystemTray.h \
    GuiAskPassword.h \
    GuiLog.h \
    GuiChatMessage.h \
    GuiSavedChatList.h \
    GuiSavedChatItem.h \
    GuiSavedChat.h \
    BeeApplication.h \
    GuiScreenShot.h \
    BuildFileShareList.h \
    BuildSavedChatList.h \
    SaveChatList.h \
    GuiFileInfoItem.h \
    GuiIconProvider.h \
    Group.h \
    GuiGroupItem.h \
    GuiGroupList.h \
    GuiCreateGroup.h \
    GuiLanguage.h \
    Avatar.h \
    GuiAddUser.h \
    UserRecord.h

win32: {
  HEADERS += BonjourBrowser.h \
    BonjourRecord.h \
    BonjourRegister.h \
    BonjourResolver.h
}


FORMS += GuiChat.ui \
    GuiVCard.ui \
    GuiEditVCard.ui \
    GuiPluginManager.ui \
    GuiSearchUser.ui \
    GuiWizard.ui \
    GuiShareLocal.ui \
    GuiShareNetwork.ui \
    GuiAskPassword.ui \
    GuiLog.ui \
    GuiSavedChat.ui \
    GuiScreenShot.ui \
    GuiCreateGroup.ui \
    GuiLanguage.ui \
    GuiAddUser.ui

RESOURCES += beebeep.qrc
win32: RC_FILE = beebeep.rc

ICON = beebeep.icns


