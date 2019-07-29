
win32-msvc: {
  QMAKE_LFLAGS += /INCREMENTAL:NO
  QMAKE_LFLAGS_WINDOWS = /SUBSYSTEM:WINDOWS,5.01
}

INCLUDEPATH += $$PWD/src $$PWD/src/core
DESTDIR = $$PWD/test

CONFIG(debug,debug|release) {
  OBJECTS_DIR = $$PWD/build/debug/qt$$QT_MAJOR_VERSION
} else {
  OBJECTS_DIR = $$PWD/build/release/qt$$QT_MAJOR_VERSION
}

UI_DIR = $$OBJECTS_DIR
MOC_DIR = $$OBJECTS_DIR
RCC_DIR = $$OBJECTS_DIR
