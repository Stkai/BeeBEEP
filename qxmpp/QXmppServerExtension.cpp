/*
 * Copyright (C) 2008-2011 The QXmpp developers
 *
 * Author:
 *  Jeremy Lainé
 *
 * Source:
 *  http://code.google.com/p/qxmpp
 *
 * This file is a part of QXmpp library.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 */

#include <QDomElement>
#include <QMetaClassInfo>
#include <QStringList>

#include "QXmppLogger.h"
#include "QXmppServer.h"
#include "QXmppServerExtension.h"

class QXmppServerExtensionPrivate
{
public:
    QXmppServer *server;
};

QXmppServerExtension::QXmppServerExtension()
    : d(new QXmppServerExtensionPrivate)
{
    d->server = 0;
}

QXmppServerExtension::~QXmppServerExtension()
{
    delete d;
}

/// Returns the discovery features to add to the server.
///

QStringList QXmppServerExtension::discoveryFeatures() const
{
    return QStringList();
}

/// Returns the discovery items to add to the server.
///

QStringList QXmppServerExtension::discoveryItems() const
{
    return QStringList();
}

/// Returns the extension's name.
///

QString QXmppServerExtension::extensionName() const
{
    int index = metaObject()->indexOfClassInfo("ExtensionName");
    if (index < 0)
        return QString();
    const char *name = metaObject()->classInfo(index).value();
    return QString::fromLatin1(name);
}

/// Handles an incoming XMPP stanza.
///
/// Return true if no further processing should occur, false otherwise.
///
/// \param stream The QXmppStream on which the stanza was received.
/// \param stanza The received stanza.

bool QXmppServerExtension::handleStanza(QXmppStream *stream, const QDomElement &stanza)
{
    Q_UNUSED(stream);
    Q_UNUSED(stanza);
    return false;
}

/// Returns the list of subscribers for the given JID.
///
/// \param jid

QStringList QXmppServerExtension::presenceSubscribers(const QString &jid)
{
    Q_UNUSED(jid);
    return QStringList();
}

/// Returns the list of subscriptions for the given JID.
///
/// \param jid

QStringList QXmppServerExtension::presenceSubscriptions(const QString &jid)
{
    Q_UNUSED(jid);
    return QStringList();
}

/// Returns the extension's statistics.
///

QVariantMap QXmppServerExtension::statistics() const
{
    return QVariantMap();
}

/// Sets the extension's statistics.
///

void QXmppServerExtension::setStatistics(const QVariantMap &statistics)
{
    Q_UNUSED(statistics);
}

/// Starts the extension.
///
/// Return true if the extension was started, false otherwise.

bool QXmppServerExtension::start()
{
    return true;
}

/// Stops the extension.

void QXmppServerExtension::stop()
{
}

/// Returns the server which loaded this extension.

QXmppServer *QXmppServerExtension::server()
{
    return d->server;
}

/// Sets the server which loaded this extension.
///
/// \param server

void QXmppServerExtension::setServer(QXmppServer *server)
{
    d->server = server;
}

