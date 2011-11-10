include(qxmpp.pri)

TEMPLATE = lib

CONFIG += staticlib

# Header files
HEADERS = QXmppUtils.h \
    QXmppArchiveIq.h \
    QXmppArchiveManager.h \
    QXmppBindIq.h \
    QXmppByteStreamIq.h \
    QXmppCallManager.h \
    QXmppClient.h \
    QXmppClientExtension.h \
    QXmppCodec.h \
    QXmppConfiguration.h \
    QXmppConstants.h \
    QXmppDataForm.h \
    QXmppDialback.h \
    QXmppDiscoveryIq.h \
    QXmppDiscoveryManager.h \
    QXmppElement.h \
    QXmppEntityTimeIq.h \
    QXmppEntityTimeManager.h \
    QXmppGlobal.h \
    QXmppIbbIq.h \
    QXmppIncomingClient.h \
    QXmppIncomingServer.h \
    QXmppInvokable.h \
    QXmppIq.h \
    QXmppJingleIq.h \
    QXmppLogger.h \
    QXmppMessage.h \
    QXmppMucIq.h \
    QXmppMucManager.h \
    QXmppNonSASLAuth.h \
    QXmppOutgoingClient.h \
    QXmppOutgoingServer.h \
    QXmppPacket.h \
    QXmppPasswordChecker.h \
    QXmppPingIq.h \
    QXmppPresence.h \
    QXmppPubSubIq.h \
    QXmppReconnectionManager.h \
    QXmppRemoteMethod.h \
    QXmppRosterIq.h \
    QXmppRosterManager.h \
    QXmppRpcIq.h \
    QXmppRpcManager.h \
    QXmppRtpChannel.h \
    QXmppSaslAuth.h \
    QXmppServer.h \
    QXmppServerExtension.h \
    QXmppServerPlugin.h \
    QXmppSrvInfo.h \
    QXmppSessionIq.h \
    QXmppSocks.h \
    QXmppStanza.h \
    QXmppStream.h \
    QXmppStreamFeatures.h \
    QXmppStreamInitiationIq.h \
    QXmppStun.h \
    QXmppTransferManager.h \
    QXmppVCardManager.h \
    QXmppVCardIq.h \
    QXmppVersionIq.h \
    QXmppVersionManager.h \
	QXmppSrvInfo_p.h

# Source files
SOURCES += QXmppUtils.cpp \
    QXmppArchiveIq.cpp \
    QXmppArchiveManager.cpp \
    QXmppBindIq.cpp \
    QXmppByteStreamIq.cpp \
    QXmppCallManager.cpp \
    QXmppClient.cpp \
    QXmppClientExtension.cpp \
    QXmppCodec.cpp \
    QXmppConfiguration.cpp \
    QXmppConstants.cpp \
    QXmppDataForm.cpp \
    QXmppDialback.cpp \
    QXmppDiscoveryIq.cpp \
    QXmppDiscoveryManager.cpp \
    QXmppElement.cpp \
    QXmppEntityTimeIq.cpp \
    QXmppEntityTimeManager.cpp \
    QXmppGlobal.cpp \
    QXmppIbbIq.cpp \
    QXmppIncomingClient.cpp \
    QXmppIncomingServer.cpp \
    QXmppInvokable.cpp \
    QXmppIq.cpp \
    QXmppJingleIq.cpp \
    QXmppLogger.cpp \
    QXmppMessage.cpp \
    QXmppMucIq.cpp \
    QXmppMucManager.cpp \
    QXmppNonSASLAuth.cpp \
    QXmppOutgoingClient.cpp \
    QXmppOutgoingServer.cpp \
    QXmppPacket.cpp \
    QXmppPasswordChecker.cpp \
    QXmppPingIq.cpp \
    QXmppPresence.cpp \
    QXmppPubSubIq.cpp \
    QXmppReconnectionManager.cpp \
    QXmppRemoteMethod.cpp \
    QXmppRosterIq.cpp \
    QXmppRosterManager.cpp \
    QXmppRpcIq.cpp \
    QXmppRpcManager.cpp \
    QXmppRtpChannel.cpp \
    QXmppSaslAuth.cpp \
    QXmppServer.cpp \
    QXmppServerExtension.cpp \
    QXmppSrvInfo.cpp \
    QXmppSessionIq.cpp \
    QXmppSocks.cpp \
    QXmppStanza.cpp \
    QXmppStream.cpp \
    QXmppStreamFeatures.cpp \
    QXmppStreamInitiationIq.cpp \
    QXmppStun.cpp \
    QXmppTransferManager.cpp \
    QXmppVCardManager.cpp \
    QXmppVCardIq.cpp \
    QXmppVersionIq.cpp \
    QXmppVersionManager.cpp

# Plugins
DEFINES += QT_STATICPLUGIN
HEADERS += \
    server/mod_disco.h \
    server/mod_ping.h \
    server/mod_presence.h \
    server/mod_proxy65.h \
    server/mod_stats.h \
    server/mod_time.h \
    server/mod_version.h
SOURCES += \
    server/mod_disco.cpp \
    server/mod_ping.cpp \
    server/mod_presence.cpp \
    server/mod_proxy65.cpp \
    server/mod_stats.cpp \
    server/mod_time.cpp \
    server/mod_version.cpp
