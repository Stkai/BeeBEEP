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

RESOURCES += \
    tetrisgame.qrc

FORMS += \
    GuiTetris.ui
