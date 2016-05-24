include(../plugins.pri)

os2: {
 TARGET=numberte
} else {
 TARGET = numbertextmarker
}

TEMPLATE = lib

DEFINES += NUMBERTEXTMARKER_LIBRARY

SOURCES += NumberTextMarker.cpp

HEADERS += NumberTextMarker.h\
        numbertextmarker_global.h

RESOURCES += \
    numbertextmarker.qrc
