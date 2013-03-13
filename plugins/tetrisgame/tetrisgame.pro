include(../plugins.pri)

TARGET = beetetris
TEMPLATE = lib
QT += widgets

DEFINES += BEE_TETRISGAME_LIBRARY

SOURCES += TetrisGame.cpp \
    GuiTetris.cpp \
    TetrisPiece.cpp \
    TetrisBoard.cpp

HEADERS += TetrisGame.h\
        tetrisgame_global.h \
    GuiTetris.h \
    TetrisPiece.h \
    TetrisBoard.h

symbian {
    MMP_RULES += EXPORTUNFROZEN
    TARGET.UID3 = 0xE61CE10B
    TARGET.CAPABILITY = 
    TARGET.EPOCALLOWDLLDATA = 1
    addFiles.sources = beetetris.dll
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
    tetrisgame.qrc

FORMS += \
    GuiTetris.ui





