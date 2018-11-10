include(beebeep.pri)
include(locale/locale.pri)

TEMPLATE = subdirs

SUBDIRS += src plugins

CONFIG += ordered

TRANSLATIONS = $$BEEBEEP_TRANSLATIONS

