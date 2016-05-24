include(../plugins.pri)

os2: {
 TARGET=rainbowt
} else {
 TARGET = rainbowtextmarker
}

TEMPLATE = lib

DEFINES += RAINBOWTEXTMARKER_LIBRARY

SOURCES += RainbowTextMarker.cpp

HEADERS += RainbowTextMarker.h\
        rainbowtextmarker_global.h

RESOURCES += \
    rainbowtextmarker.qrc
