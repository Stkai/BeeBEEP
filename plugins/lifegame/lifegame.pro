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

RESOURCES += \
    lifegame.qrc

FORMS += \
    GuiLife.ui
