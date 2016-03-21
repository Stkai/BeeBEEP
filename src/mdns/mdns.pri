
INCLUDEPATH += $$PWD

win32|macx|unix:!android: DEFINES += BEEBEEP_USE_MULTICAST_DNS
win32|macx|unix:!android: HEADERS += mdns/MDnsBrowser.h mdns/MDnsManager.h mdns/MDnsObject.h mdns/MDnsRecord.h mdns/MDnsRegister.h mdns/MDnsResolver.h
win32|macx|unix:!android: SOURCES += mdns/MDnsBrowser.cpp mdns/MDnsManager.cpp mdns/MDnsObject.cpp mdns/MDnsRecord.cpp mdns/MDnsRegister.cpp mdns/MDnsResolver.cpp
win32: QMAKE_LFLAGS += /NODEFAULTLIB:libcmt
win32: LIBS += -L$$PWD/../../mdns-lib/ -ldnssd
win32: INCLUDEPATH += $$PWD/../../mdns-lib
win32: DEPENDPATH += $$PWD/../../mdns-lib
win32:!win32-g++: PRE_TARGETDEPS += $$PWD/../../mdns-lib/dnssd.lib
else:win32-g++: PRE_TARGETDEPS += $$PWD/../../mdns-lib/libdnssd.a
unix:!macx:!android: LIBS += -ldns_sd
