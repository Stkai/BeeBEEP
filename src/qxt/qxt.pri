
INCLUDEPATH += $$PWD
win32|macx|unix:!android: DEFINES += BEEBEEP_USE_QXT

HEADERS +=  qxt/qxtglobal.h \
            qxt/qxtglobalshortcut.h \
            qxt/qxtglobalshortcut_p.h

SOURCES +=  qxt/qxtglobalshortcut.cpp

macx: SOURCES += qxt/qxtglobalshortcut_mac.cpp
macx: LIBS += -framework Carbon
unix:!macx:!android: SOURCES += qxt/qxtglobalshortcut_x11.cpp
unix:!macx:!android: LIBS += -lX11
win32: SOURCES += qxt/qxtglobalshortcut_win.cpp

