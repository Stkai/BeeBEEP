include(../plugins.pri)

TARGET = beewindowslive
TEMPLATE = lib

DEFINES += BEE_WINDOWSLIVESERVICE_LIBRARY

SOURCES += WindowsLiveService.cpp

HEADERS += WindowsLiveService.h\
        windowsliveservice_global.h

symbian {
    MMP_RULES += EXPORTUNFROZEN
    TARGET.UID3 = 0xE61CE10B
    TARGET.CAPABILITY = 
    TARGET.EPOCALLOWDLLDATA = 1
    addFiles.sources = beewindowslive.dll
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
    windowsliveservice.qrc
