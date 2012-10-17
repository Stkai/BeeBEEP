include(../plugins.pri)

TARGET = beegtalk
TEMPLATE = lib

DEFINES += BEE_GTALKSERVICE_LIBRARY

SOURCES += GTalkService.cpp

HEADERS += GTalkService.h\
        gtalkservice_global.h

symbian {
    MMP_RULES += EXPORTUNFROZEN
    TARGET.UID3 = 0xE61CE10B
    TARGET.CAPABILITY = 
    TARGET.EPOCALLOWDLLDATA = 1
    addFiles.sources = beegtalk.dll
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

RESOURCES += \
    gtalkservice.qrc
