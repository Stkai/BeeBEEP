
macx: {
  CONFIG += app_bundle
  QMAKE_INFO_PLIST = $$PWD/misc/Info.plist
}

win32: QMAKE_LFLAGS_DEBUG += /INCREMENTAL:NO

DESTDIR = $$PWD/test
