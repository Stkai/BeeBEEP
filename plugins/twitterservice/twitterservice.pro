include(../plugins.pri)

TARGET = beetwitter
TEMPLATE = lib

DEFINES += BEE_TWITTERSERVICE_LIBRARY

SOURCES += TwitterService.cpp

HEADERS += TwitterService.h\
        twitterservice_global.h

symbian {
    MMP_RULES += EXPORTUNFROZEN
    TARGET.UID3 = 0xE61CE10B
    TARGET.CAPABILITY = 
    TARGET.EPOCALLOWDLLDATA = 1
    addFiles.sources = beetwitter.dll
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
    twitterservice.qrc
