include(../beebeep.pri)

TEMPLATE = subdirs

contains(DEFINES, USE_QXMPP) {
  SUBDIRS += facebookservice \
            gtalkservice \
            jabberservice \
            windowsliveservice \
            twitterservice
}

SUBDIRS += rainbowtextmarker \
			numbertextmarker \
			tetrisgame


