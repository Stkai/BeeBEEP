
DEFINES += BEEBEEP_USE_VIDEOCALL

INCLUDEPATH += $$PWD

HEADERS += videocall/GuiVideoCall.h \
           videocall/GuiVideoSettings.h \
           videocall/VideoCallManager.h \
           videocall/VideoCallClient.h \
           videocall/VideoCallServer.h

SOURCES += videocall/GuiVideoCall.cpp \
           videocall/GuiVideoSettings.cpp \
           videocall/VideoCallManager.cpp \
           videocall/VideoCallClient.cpp \
           videocall/VideoCallServer.cpp \
           core/CoreVideoCall.cpp


FORMS += videocall/GuiVideoSettings.ui
