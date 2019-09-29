
win32-msvc: {
  QMAKE_LFLAGS += /INCREMENTAL:NO
  QMAKE_LFLAGS_WINDOWS = /SUBSYSTEM:WINDOWS,5.01
}

INCLUDEPATH += $$PWD/src $$PWD/src/core
DESTDIR = $$PWD/test
DEFINES += QT_DEPRECATED_WARNINGS

unix:!macx:!android: {
  CONFIG(debug,debug|release) {
    OBJECTS_DIR = $$PWD/build/debug/qt-$$QT_VERSION-$$QMAKE_HOST.arch
  } else {
    OBJECTS_DIR = $$PWD/build/release/qt-$$QT_VERSION-$$QMAKE_HOST.arch
  }

  UI_DIR = $$OBJECTS_DIR
  MOC_DIR = $$OBJECTS_DIR
  RCC_DIR = $$OBJECTS_DIR
}
