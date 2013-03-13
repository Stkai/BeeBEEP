
DEFINES += USE_QXMPP

win32:DESTDIR = $$PWD/test
unix:DESTDIR = $$PWD/test

Debug {
  DEFINES += BEEBEEP_DEBUG
}

Release {
  DEFINES += BEEBEEP_RELEASE
}

