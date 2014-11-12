
DESTDIR = $$PWD/test

macx: CONFIG += app_bundle

Debug {
  DEFINES += BEEBEEP_DEBUG
}

Release {
  DEFINES += BEEBEEP_RELEASE
}

