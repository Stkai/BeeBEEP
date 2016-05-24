
macx: {
  CONFIG += app_bundle
  QMAKE_INFO_PLIST = $$PWD/misc/Info.plist
}

win32: {
  QMAKE_LFLAGS += /INCREMENTAL:NO
  QMAKE_LFLAGS_WINDOWS = /SUBSYSTEM:WINDOWS,5.01
}

INCLUDEPATH += $$PWD/src $$PWD/src/core
DESTDIR = $$PWD/test
