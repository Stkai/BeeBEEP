
CONFIG(debug,debug|release) {
  message( Building BeeBEEP in Debug Mode )
  DEFINES += BEEBEEP_DEBUG
} else {
  message( Building BeeBEEP in Release Mode )
}

macx {
  CONFIG(debug,debug|release) {
    CONFIG -= app_bundle
    message( Building BeeBEEP without BUNDLE )
  } else {
    CONFIG += app_bundle
    message( Building BeeBEEP BUNDLE )
  }
}

message( Qt version: $$[QT_VERSION] )

include(../beebeep.pri)

message( Target folder: $$DESTDIR )

TEMPLATE = app
!macx:DEFINES += MAKE_BEEBEEP_PORTABLE
#DEFINES += BEEBEEP_DISABLE_FILE_TRANSFER
#DEFINES += BEEBEEP_DISABLE_SEND_MESSAGE
#DEFINES += BEEBEEP_DISABLE_VIDEO_CALL
#DEFINES += BEEBEEP_USE_WEBENGINE

TARGET = beebeep

QT += network xml
greaterThan(QT_MAJOR_VERSION, 4): {
  QT += widgets printsupport
}

win32|unix {
  greaterThan(QT_MAJOR_VERSION, 4): {
    QT += multimedia
    DEFINES += BEEBEEP_USE_VOICE_CHAT
  }
}

unix:!macx:!android: equals(QT_MAJOR_VERSION, 4) {
  greaterThan(QT_MINOR_VERSION, 6): {
    DEFINES += BEEBEEP_USE_PHONON4
    QT += phonon
  }
}

unix:!macx:!android: {
  LIBS += -lxcb -lxcb-screensaver
  CONFIG += largefile
}

macx {
  QMAKE_LFLAGS += -F/System/Library/Frameworks/ApplicationServices.framework
  LIBS += -framework ApplicationServices
  lessThan( QT_MAJOR_VERSION, 5) | lessThan(QT_MINOR_VERSION, 9) {
    QMAKE_INFO_PLIST = ../misc/Info_legacy.plist
  } else {
    QMAKE_INFO_PLIST = ../misc/Info.plist
  }
}

win32 {
  DEFINES += _CRT_SECURE_NO_WARNINGS
  LIBS += -luser32
}

message( Qt modules: $$QT )

include(../locale/locale.pri)
include(hunspell/hunspell.pri)
include(core/core.pri)
include(ecdh/ecdh.pri)
include(gui/gui.pri)
include(desktop/desktop.pri)
include(override/override.pri)
include(utils/utils.pri)
win32|unix {
  include(qxt/qxt.pri)
  include(sharedesktop/sharedesktop.pri)
  contains(DEFINES, BEEBEEP_USE_VOICE_CHAT) {
    include(voicechat/voicechat.pri)
  }
}

#!contains( DEFINES, BEEBEEP_DISABLE_VIDEO_CALL ) {
#  greaterThan( QT_MAJOR_VERSION, 4 ): {
#    greaterThan( QT_MINOR_VERSION, 4 ): {
#      message( Custom module added: videocall )
#      include(videocall/videocall.pri)
# } } } else {
#   message( Custom module disabled: videocall )
# }

HEADERS += Interfaces.h

RESOURCES += beebeep.qrc emojis.qrc emojis2x_1.qrc emojis2x_2.qrc

win32: RC_FILE = beebeep.rc
macx: ICON = beebeep.icns
macx: include(mdns/mdns.pri)

message( Config: $$CONFIG )
message( Libs: $$LIBS )
message( Defines: $$DEFINES )
message( Resources: $$RESOURCES )
macx: message( Info.plist: $$QMAKE_INFO_PLIST )
message( Object dir: $$OBJECTS_DIR )
unix: message( CXX flags: $$QMAKE_CXXFLAGS )
message( QT Plugins: $$QTPLUGIN )
