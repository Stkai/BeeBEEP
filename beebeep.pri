
macx: {
  CONFIG += app_bundle
  QMAKE_INFO_PLIST = $$PWD/src/Info.plist
  QMAKE_MAC_SDK = macosx10.11
}

win32: {
  QMAKE_LFLAGS_DEBUG += /INCREMENTAL:NO
  QMAKE_LFLAGS += /INCREMENTAL:NO
}

DESTDIR = $$PWD/test
