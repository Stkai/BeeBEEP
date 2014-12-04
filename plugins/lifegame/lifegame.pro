include(../plugins.pri)

TARGET = beelife
TEMPLATE = lib
QT += widgets

DEFINES += BEE_LIFEGAME_LIBRARY

SOURCES += LifeGame.cpp \
    GuiLife.cpp \
    LifeBoard.cpp

HEADERS += LifeGame.h\
        lifegame_global.h \
    GuiLife.h \
    LifeBoard.h \
    ../../src/Random.h

symbian {
    MMP_RULES += EXPORTUNFROZEN
    TARGET.UID3 = 0xE61CE10B
    TARGET.CAPABILITY = 
    TARGET.EPOCALLOWDLLDATA = 1
    addFiles.sources = beelife.dll
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
    lifegame.qrc

FORMS += \
    GuiLife.ui





