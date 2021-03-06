
DEFINES += BEEBEEP_USE_HUNSPELL HUNSPELL_STATIC

INCLUDEPATH += $$PWD

HEADERS +=  hunspell/hunspell.h \
            hunspell/hunvisapi.h \
            hunspell/affentry.hxx \
            hunspell/affixmgr.hxx \
            hunspell/atypes.hxx \
            hunspell/baseaffix.hxx \
            hunspell/csutil.hxx \
            hunspell/filemgr.hxx \
            hunspell/hashmgr.hxx \
            hunspell/htypes.hxx \
            hunspell/hunspell.hxx \
            hunspell/hunzip.hxx \
            hunspell/langnum.hxx \
            hunspell/phonet.hxx \
            hunspell/replist.hxx \
            hunspell/suggestmgr.hxx \
            hunspell/utf_info.hxx \
            hunspell/w_char.hxx

SOURCES +=  hunspell/affentry.cxx \
            hunspell/affixmgr.cxx \
            hunspell/csutil.cxx \
            hunspell/filemgr.cxx \
            hunspell/hashmgr.cxx \
            hunspell/hunspell.cxx \
            hunspell/hunzip.cxx \
            hunspell/phonet.cxx \
            hunspell/replist.cxx \
            hunspell/suggestmgr.cxx
