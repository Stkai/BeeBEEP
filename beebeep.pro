include(beebeep.pri)

TEMPLATE = subdirs

contains(DEFINES, USE_QXMPP) {
  SUBDIRS += qxmpp
}

SUBDIRS += src plugins

CONFIG += ordered

macx:CONFIG -= app_bundle

TRANSLATIONS += locale/beebeep_it.ts
