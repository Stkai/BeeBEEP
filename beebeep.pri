
macx: {
  CONFIG += app_bundle
  QMAKE_INFO_PLIST = $$PWD/misc/Info.plist
  QMAKE_MAC_SDK = macosx10.11
}

win32: {
  QMAKE_LFLAGS += /INCREMENTAL:NO
}

INCLUDEPATH += $$PWD/src
DESTDIR = $$PWD/test
