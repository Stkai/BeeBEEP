# Common definitions

QT += network

BEEBEEP_INCLUDE_DIR = $$PWD/src

Debug {
  DEFINES += BEEBEEP_DEBUG
}

Release {
  DEFINES += QT_NO_WARNING_OUTPUT QT_NO_DEBUG_OUTPUT
}
