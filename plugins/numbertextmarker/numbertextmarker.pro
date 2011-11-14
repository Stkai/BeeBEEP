include(../plugins.pri)

TARGET = numbertextmarker
TEMPLATE = lib

DEFINES += NUMBERTEXTMARKER_LIBRARY

SOURCES += NumberTextMarker.cpp

HEADERS += NumberTextMarker.h\
        numbertextmarker_global.h

symbian {
    MMP_RULES += EXPORTUNFROZEN
    TARGET.UID3 = 0xE61CE10B
    TARGET.CAPABILITY = 
    TARGET.EPOCALLOWDLLDATA = 1
    addFiles.sources = numbertextmarker.dll
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
    numbertextmarker.qrc
