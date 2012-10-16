
DEFINES += USE_QXMPP

DESTDIR = $$PWD/test

Debug {
  DEFINES += BEEBEEP_DEBUG
}

Release {
  DEFINES += BEEBEEP_RELEASE
}

