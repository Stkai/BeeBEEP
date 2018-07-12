
DEFINES += BEEBEEP_USE_QXT

INCLUDEPATH += $$PWD

HEADERS +=  qxt/qxtglobal.h \
            qxt/qxtglobalshortcut.h \
            qxt/qxtglobalshortcut_p.h

SOURCES +=  qxt/qxtglobalshortcut.cpp

macx: SOURCES += qxt/qxtglobalshortcut_mac.cpp
macx: LIBS += -framework Carbon
unix:!macx:!android: greaterThan(QT_MAJOR_VERSION, 4): {
  QT += x11extras
}
unix:!macx:!android: SOURCES += qxt/qxtglobalshortcut_x11.cpp
unix:!macx:!android: LIBS += -lX11
win32: SOURCES += qxt/qxtglobalshortcut_win.cpp
os2: SOURCES += qxt/qxtglobalshortcut_os2.cpp
