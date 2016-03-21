
CONFIG(debug,debug|release) {
  message( Building BeeBEEP in Debug Mode )
  DEFINES += BEEBEEP_DEBUG
} else {
  message( Building BeeBEEP in Release Mode )
}

message( Qt version: $$[QT_VERSION] )

include(../beebeep.pri)

message( Target folder: $$DESTDIR )

QT += network xml
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets multimedia printsupport

unix:!macx:!android: equals(QT_MAJOR_VERSION, 4) {
  greaterThan(QT_MINOR_VERSION, 6 ): {
    DEFINES += BEEBEEP_USE_PHONON4
    QT += phonon
  }
}

unix:!macx:!android: {
  LIBS= -lxcb -lxcb-screensaver
}

macx: {
  QMAKE_LFLAGS += -F/System/Library/Frameworks/ApplicationServices.framework
  LIBS += -framework ApplicationServices
}

win32: {
  DEFINES += _CRT_SECURE_NO_WARNINGS
  LIBS += -luser32
}

message( Qt modules: $$QT )

include(../locale/locale.pri)

TARGET = beebeep
TEMPLATE = app

include(hunspell/hunspell.pri)
include(core/core.pri)
include(gui/gui.pri)
include(desktop/desktop.pri)
include(override/override.pri)
include(utils/utils.pri)

HEADERS += BeeApplication.h \
  Interfaces.h \
  Settings.h


SOURCES += BeeApplication.cpp \
  BeeApplication_mac.cpp \
  Main.cpp \
  Settings.cpp


RESOURCES += beebeep.qrc emojis.qrc

win32: RC_FILE = beebeep.rc
macx: ICON = beebeep.icns

win32|macx|unix:!android: equals(QT_MAJOR_VERSION, 4):lesserThan(QT_MINOR_VERSION, 7) {
    message( MDns disabled due the old Qt version )
  } else { include(mdns/mdns.pri) }


message( Libs: $$LIBS )
message( Defines: $$DEFINES )
