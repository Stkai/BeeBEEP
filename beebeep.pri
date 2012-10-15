
DEFINES += USE_QXMPP

DESTDIR = $$PWD/test

Debug {
  DEFINES += BEEBEEP_DEBUG
}

Release {
  DEFINES += BEEBEEP_RELEASE
}

contains(DEFINES, USE_QXMPP) {
   message(Building QXmpp version...)
}
