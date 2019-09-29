
DEFINES += BEEBEEP_USE_OPUS_CODEC

INCLUDEPATH += $$PWD/include
DEPENDPATH += $$PWD/lib
LIBS += -L$$PWD/lib/ -lopus

win32-g++: PRE_TARGETDEPS += $$PWD/lib/libopus.a
else:win32:!win32-g++: PRE_TARGETDEPS += $$PWD/lib/opus.lib
else:unix: PRE_TARGETDEPS += $$PWD/lib/libopus.a
