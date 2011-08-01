# -------------------------------------------------
# Project created by QtCreator 2010-05-08T18:54:22
# -------------------------------------------------
QT += network
TARGET = beebeep
TEMPLATE = app
TRANSLATIONS = locale/beebeep_it.ts
SOURCES += Listener.cpp \
    Connection.cpp \
    PeerManager.cpp \
    GuiChat.cpp \
    Message.cpp \
    Protocol.cpp \
    User.cpp \
    BeeBeep.cpp \
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
    FileTransferServer.cpp \
    FileTransferClient.cpp \
    FileTransferRead.cpp
HEADERS += sym_iap_util.h \
    Listener.h \
    Connection.h \
    Config.h \
    PeerManager.h \
    GuiChat.h \
    Message.h \
    Protocol.h \
    User.h \
    Version.h \
    BeeBeep.h \
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
    FileTransferServer.h \
    FileTransferClient.h \
    FileTransferRead.h
FORMS += GuiChat.ui
RESOURCES += beebeep.qrc
RC_FILE = beebeep.rc
OTHER_FILES +=

Debug {
  DEFINES += BEEBEEP_DEBUG
}
