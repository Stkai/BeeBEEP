include(../plugins.pri)

message(RainbowTextMarker target folder: $$DESTDIR)

TARGET = rainbowtextmarker
TEMPLATE = lib

DEFINES += RAINBOWTEXTMARKER_LIBRARY

SOURCES += RainbowTextMarker.cpp

HEADERS += RainbowTextMarker.h\
        rainbowtextmarker_global.h

RESOURCES += \
    rainbowtextmarker.qrc
