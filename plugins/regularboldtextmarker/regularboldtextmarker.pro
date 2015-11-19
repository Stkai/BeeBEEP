include(../plugins.pri)

message(RegularBoldTextMarker target folder: $$DESTDIR)

TARGET = regularboldtextmarker
TEMPLATE = lib

DEFINES += REGULARBOLDTEXTMARKER_LIBRARY

SOURCES += RegularBoldTextMarker.cpp

HEADERS += RegularBoldTextMarker.h\
        regularboldtextmarker_global.h

RESOURCES += \
    regularboldtextmarker.qrc
