include(../plugins.pri)

TARGET = rainbowtextmarker
TEMPLATE = lib

DEFINES += RAINBOWTEXTMARKER_LIBRARY

SOURCES += RainbowTextMarker.cpp

HEADERS += RainbowTextMarker.h\
        rainbowtextmarker_global.h

symbian {
    MMP_RULES += EXPORTUNFROZEN
    TARGET.UID3 = 0xE61CE10B
    TARGET.CAPABILITY = 
    TARGET.EPOCALLOWDLLDATA = 1
    addFiles.sources = rainbowtextmarker.dll
    addFiles.path = !:/sys/bin
    DEPLOYMENT += addFiles
}

unix:!symbian {
    maemo5 {
        target.path = /opt/usr/lib
    } else {
        target.path = /usr/lib
    }
    INSTALLS += target
}

OTHER_FILES +=

RESOURCES += \
    rainbowtextmarker.qrc
