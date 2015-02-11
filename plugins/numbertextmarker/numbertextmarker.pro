include(../plugins.pri)

TARGET = numbertextmarker
TEMPLATE = lib

DEFINES += NUMBERTEXTMARKER_LIBRARY

SOURCES += NumberTextMarker.cpp

HEADERS += NumberTextMarker.h\
        numbertextmarker_global.h

RESOURCES += \
    numbertextmarker.qrc
