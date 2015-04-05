
macx: {
  CONFIG += app_bundle
  QMAKE_INFO_PLIST = $$PWD/misc/Info.plist
} else {
  DESTDIR = $$PWD/test
}
