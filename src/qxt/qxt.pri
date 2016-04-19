
INCLUDEPATH += $$PWD

HEADERS +=  qxt/qxtglobal.h \
            qxt/qxtglobalshortcut.h \
            qxt/qxtglobalshortcut_p.h

SOURCES +=  qxt/qxtglobalshortcut.cpp

mac: SOURCES += qxt/qxtglobalshortcut_mac.cpp
unix:!mac:!android: SOURCES += qxt/qxtglobalshortcut_x11.cpp
win32: SOURCES += qxt/qxtglobalshortcut_win.cpp

