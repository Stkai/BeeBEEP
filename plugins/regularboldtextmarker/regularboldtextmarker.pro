include(../plugins.pri)

TARGET = regularboldtextmarker
TEMPLATE = lib

DEFINES += REGULARBOLDTEXTMARKER_LIBRARY

SOURCES += RegularBoldTextMarker.cpp

HEADERS += RegularBoldTextMarker.h\
        regularboldtextmarker_global.h

RESOURCES += \
    regularboldtextmarker.qrc
