
INCLUDEPATH += $$PWD

HEADERS += desktop/BeeApplication.h \
  desktop/GuiAddUser.h \
  desktop/GuiAskPassword.h \
  desktop/GuiChat.h \
  desktop/GuiChatItem.h \
  desktop/GuiChatList.h \
  desktop/GuiChatMessage.h \
  desktop/GuiCreateGroup.h \
  desktop/GuiConfig.h \
  desktop/GuiEditVCard.h \
  desktop/GuiEmoticons.h \
  desktop/GuiFileInfoItem.h \
  desktop/GuiFileInfoList.h \
  desktop/GuiFileSharing.h \
  desktop/GuiFloatingChat.h \
  desktop/GuiGroupItem.h \
  desktop/GuiGroupList.h \
  desktop/GuiHome.h \
  desktop/GuiIconProvider.h \
  desktop/GuiLanguage.h \
  desktop/GuiLog.h \
  desktop/GuiMain.h \
  desktop/GuiMessageEdit.h \
  desktop/GuiNetwork.h \
  desktop/GuiPluginManager.h \
  desktop/GuiPresetMessageList.h \
  desktop/GuiRefusedChat.h \
  desktop/GuiSavedChat.h \
  desktop/GuiSavedChatItem.h \
  desktop/GuiSavedChatList.h \
  desktop/GuiScreenShot.h \
  desktop/GuiShareBox.h \
  desktop/GuiShareBoxFileInfoItem.h \
  desktop/GuiShareBoxFileInfoList.h \
  desktop/GuiShareLocal.h \
  desktop/GuiShareNetwork.h \
  desktop/GuiShortcut.h \
  desktop/GuiSystemTray.h \
  desktop/GuiTransferFile.h \
  desktop/GuiUserItem.h \
  desktop/GuiUserList.h \
  desktop/GuiVCard.h \
  desktop/GuiWizard.h \
  desktop/GuiWorkgroups.h


SOURCES +=  desktop/BeeApplication.cpp \
  desktop/GuiAddUser.cpp \
  desktop/GuiAskPassword.cpp \
  desktop/GuiChat.cpp \
  desktop/GuiChatItem.cpp \
  desktop/GuiChatList.cpp \
  desktop/GuiChatMessage.cpp \
  desktop/GuiCreateGroup.cpp \
  desktop/GuiEditVCard.cpp \
  desktop/GuiEmoticons.cpp \
  desktop/GuiFileInfoItem.cpp \
  desktop/GuiFileInfoList.cpp \
  desktop/GuiFileSharing.cpp \
  desktop/GuiFloatingChat.cpp \
  desktop/GuiGroupItem.cpp \
  desktop/GuiGroupList.cpp \
  desktop/GuiHome.cpp \
  desktop/GuiIconProvider.cpp \
  desktop/GuiLanguage.cpp \
  desktop/GuiLog.cpp \
  desktop/GuiMain.cpp \
  desktop/GuiMessageEdit.cpp \
  desktop/GuiNetwork.cpp \
  desktop/GuiPluginManager.cpp \
  desktop/GuiPresetMessageList.cpp \
  desktop/GuiRefusedChat.cpp \
  desktop/GuiSavedChat.cpp \
  desktop/GuiSavedChatItem.cpp \
  desktop/GuiSavedChatList.cpp \
  desktop/GuiScreenShot.cpp \
  desktop/GuiShareBox.cpp \
  desktop/GuiShareBoxFileInfoItem.cpp \
  desktop/GuiShareBoxFileInfoList.cpp \
  desktop/GuiShareLocal.cpp \
  desktop/GuiShareNetwork.cpp \
  desktop/GuiShortcut.cpp \
  desktop/GuiSystemTray.cpp \
  desktop/GuiTransferFile.cpp \
  desktop/GuiUserItem.cpp \
  desktop/GuiUserList.cpp \
  desktop/GuiVCard.cpp \
  desktop/GuiWizard.cpp \
  desktop/GuiWorkgroups.cpp \
  desktop/Main.cpp

win32: SOURCES += desktop/BeeApplication_win.cpp
macx: SOURCES += desktop/BeeApplication_mac.cpp

FORMS += desktop/GuiAddUser.ui \
  desktop/GuiAskPassword.ui \
  desktop/GuiChat.ui \
  desktop/GuiChatList.ui \
  desktop/GuiCreateGroup.ui \
  desktop/GuiEditVCard.ui \
  desktop/GuiGroupList.ui \
  desktop/GuiHome.ui \
  desktop/GuiLanguage.ui \
  desktop/GuiNetwork.ui \
  desktop/GuiPluginManager.ui \
  desktop/GuiRefusedChat.ui \
  desktop/GuiSavedChat.ui \
  desktop/GuiSavedChatList.ui \
  desktop/GuiShareBox.ui \
  desktop/GuiShareLocal.ui \
  desktop/GuiShareNetwork.ui \
  desktop/GuiShortcut.ui \
  desktop/GuiUserList.ui \
  desktop/GuiVCard.ui \
  desktop/GuiWizard.ui \
  desktop/GuiWorkgroups.ui
