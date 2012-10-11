include(../beebeep.pri)
QT += network xml
TARGET = beebeep
TEMPLATE = app
TRANSLATIONS = locale/beebeep_it.ts
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
    XmppManager.cpp \
    CoreXmpp.cpp \
    GuiNetwork.cpp \
    GuiNetworkLogin.cpp \
    XmppClient.cpp \
    UserManager.cpp \
    ChatManager.cpp \
    GuiChatItem.cpp \
    GuiChatList.cpp \
    NetworkAccount.cpp \
    GuiSearchUser.cpp \
    GuiWizard.cpp \
    FileShare.cpp \
    GuiShareLocal.cpp \
    GuiShareNetwork.cpp
HEADERS += sym_iap_util.h \
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
    XmppManager.h \
    GuiNetwork.h \
    GuiNetworkLogin.h \
    XmppClient.h \
    UserManager.h \
    ChatManager.h \
    GuiChatItem.h \
    GuiChatList.h \
    NetworkAccount.h \
    GuiSearchUser.h \
    GuiWizard.h \
    FileShare.h \
    GuiShareLocal.h \
    GuiShareNetwork.h
FORMS += GuiChat.ui \
    GuiVCard.ui \
    GuiEditVCard.ui \
    GuiPluginManager.ui \
    GuiNetwork.ui \
    GuiNetworkLogin.ui \
    GuiSearchUser.ui \
    GuiWizard.ui \
    GuiShareLocal.ui \
    GuiShareNetwork.ui
RESOURCES += beebeep.qrc
RC_FILE = beebeep.rc

LIBS += -L$$DESTDIR -lqxmpp

INCLUDEPATH += $$PWD/../qxmpp/base $$PWD/../qxmpp/client $$PWD/../qxmpp/server
DEPENDPATH += $$PWD/../qxmpp/base $$PWD/../qxmpp/client $$PWD/../qxmpp/server





























