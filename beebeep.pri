
win32-msvc: {
  QMAKE_LFLAGS += /INCREMENTAL:NO
  lessThan( QT_MAJOR_VERSION, 5) | lessThan(QT_MINOR_VERSION, 10) {
    QMAKE_LFLAGS_WINDOWS = /SUBSYSTEM:WINDOWS,5.01
  } else {
    QMAKE_LFLAGS_WINDOWS = /SUBSYSTEM:WINDOWS
  }
}

INCLUDEPATH += $$PWD/src $$PWD/src/core
DESTDIR = $$PWD/test

unix:!macx:!android: {
  CONFIG(debug,debug|release) {
    OBJECTS_DIR = $$PWD/build/debug/qt-$$QT_VERSION-$$QMAKE_HOST.arch
  } else {
    OBJECTS_DIR = $$PWD/build/release/qt-$$QT_VERSION-$$QMAKE_HOST.arch
  }

  UI_DIR = $$OBJECTS_DIR
  MOC_DIR = $$OBJECTS_DIR
  RCC_DIR = $$OBJECTS_DIR

  isEmpty(PREFIX) {
    PREFIX = /usr/local
  }
  isEmpty(BINDIR) {
    BINDIR = $$PREFIX/bin
  }
  isEmpty(DATADIR) {
    DATADIR = $$PREFIX/share
  }
  isEmpty(PLUGINDIR) {
    PLUGINDIR = $$PREFIX/lib/beebeep
  }

  data.files = locale/*.qm src/images/beebeep.png misc/beep.wav
  data.path = $$DATADIR/beebeep/

  desktop.files = scripts/debian_amd64/beebeep.desktop
  desktop.path = $$DATADIR/applications/

  appdata.files = scripts/debian_amd64/beebeep.appdata.xml
  appdata.path = $$DATADIR/metainfo/

  INSTALLS += data desktop appdata
}
