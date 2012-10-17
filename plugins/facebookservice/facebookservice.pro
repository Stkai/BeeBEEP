include(../plugins.pri)

TARGET = beefacebook
TEMPLATE = lib

DEFINES += BEE_FACEBOOKSERVICE_LIBRARY

SOURCES += FacebookService.cpp

HEADERS += FacebookService.h\
        facebookservice_global.h

symbian {
    MMP_RULES += EXPORTUNFROZEN
    TARGET.UID3 = 0xE61CE10B
    TARGET.CAPABILITY = 
    TARGET.EPOCALLOWDLLDATA = 1
    addFiles.sources = beefacebook.dll
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
    facebookservice.qrc
