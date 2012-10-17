include(../plugins.pri)

TARGET = beejabber
TEMPLATE = lib

DEFINES += BEE_JABBERSERVICE_LIBRARY

SOURCES += JabberService.cpp

HEADERS += JabberService.h\
        jabberservice_global.h

symbian {
    MMP_RULES += EXPORTUNFROZEN
    TARGET.UID3 = 0xE61CE10B
    TARGET.CAPABILITY = 
    TARGET.EPOCALLOWDLLDATA = 1
    addFiles.sources = beejabber.dll
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
    jabberservice.qrc
