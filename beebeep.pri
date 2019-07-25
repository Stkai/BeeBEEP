
macx: {
  QMAKE_INFO_PLIST = $$PWD/misc/Info.plist
}

macx: {
  CONFIG(debug,debug|release) {
    CONFIG -= app_bundle
    message( Building BeeBEEP without BUNDLE )
  } else {
    CONFIG += app_bundle
    message( Building BeeBEEP BUNDLE )
  }
}

win32-msvc: {
  QMAKE_LFLAGS += /INCREMENTAL:NO
  QMAKE_LFLAGS_WINDOWS = /SUBSYSTEM:WINDOWS,5.01
}

INCLUDEPATH += $$PWD/src $$PWD/src/core
DESTDIR = $$PWD/test
