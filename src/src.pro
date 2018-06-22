
CONFIG(debug,debug|release) {
  message( Building BeeBEEP in Debug Mode )
  DEFINES += BEEBEEP_DEBUG
} else {
  message( Building BeeBEEP in Release Mode )
}

message( Qt version: $$[QT_VERSION] )

include(../beebeep.pri)

message( Target folder: $$DESTDIR )

TEMPLATE = app
!macx:DEFINES += MAKE_BEEBEEP_PORTABLE
#DEFINES += BEEBEEP_DISABLE_FILE_TRANSFER
#DEFINES += BEEBEEP_DISABLE_SEND_MESSAGE

TARGET = beebeep

QT += network xml
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets multimedia printsupport

unix:!macx:!android: equals(QT_MAJOR_VERSION, 4) {
  greaterThan(QT_MINOR_VERSION, 6): {
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

include(qxt/qxt.pri)
include(../locale/locale.pri)
include(hunspell/hunspell.pri)
include(core/core.pri)
include(gui/gui.pri)
include(desktop/desktop.pri)
include(override/override.pri)
include(utils/utils.pri)
include(sharedesktop/sharedesktop.pri)

HEADERS += Interfaces.h Tips.h

RESOURCES += beebeep.qrc emojis.qrc

win32: RC_FILE = beebeep.rc
macx: ICON = beebeep.icns
macx: include(mdns/mdns.pri)

message( Libs: $$LIBS )
message( Defines: $$DEFINES )
