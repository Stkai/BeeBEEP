include(../qxmpp.pri)

QT -= gui

TEMPLATE = lib

CONFIG += $$QXMPP_LIBRARY_TYPE
DEFINES += QXMPP_BUILD
DEFINES += $$QXMPP_INTERNAL_DEFINES
INCLUDEPATH += $$QXMPP_INCLUDEPATH $$QXMPP_INTERNAL_INCLUDES
LIBS += $$QXMPP_INTERNAL_LIBS

!isEmpty(QXMPP_USE_SPEEX) {
    DEFINES += QXMPP_USE_SPEEX
    LIBS += -lspeex
}

!isEmpty(QXMPP_USE_THEORA) {
    DEFINES += QXMPP_USE_THEORA
    LIBS += -ltheoradec -ltheoraenc
}

!isEmpty(QXMPP_USE_VPX) {
    DEFINES += QXMPP_USE_VPX
    LIBS += -lvpx
}

# Target definition
TARGET = $$QXMPP_LIBRARY_NAME
VERSION = $$QXMPP_VERSION

include(base/base.pri)
include(client/client.pri)
include(server/server.pri)

HEADERS += $$INSTALL_HEADERS

