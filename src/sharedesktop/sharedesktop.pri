
DEFINES += BEEBEEP_USE_SHAREDESKTOP

INCLUDEPATH += $$PWD

HEADERS += sharedesktop/ShareDesktop.h \
           sharedesktop/ShareDesktopData.h \
           sharedesktop/ShareDesktopJob.h \
           sharedesktop/GuiShareDesktop.h

SOURCES += sharedesktop/ShareDesktop.cpp \
           sharedesktop/ShareDesktopData.cpp \
           sharedesktop/ShareDesktopJob.cpp \
           sharedesktop/GuiShareDesktop.cpp \
           core/CoreShareDesktop.cpp

FORMS += sharedesktop/GuiShareDesktop.ui
