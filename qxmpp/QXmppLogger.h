/*
 * Copyright (C) 2008-2011 The QXmpp developers
 *
 * Author:
 *  Manjeet Dahiya
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


#ifndef QXMPPLOGGER_H
#define QXMPPLOGGER_H

#include <QObject>

#ifdef QXMPP_LOGGABLE_TRACE
#define qxmpp_loggable_trace(x) QString("%1(0x%2) %3").arg(metaObject()->className(), QString::number(reinterpret_cast<qint64>(this), 16), x)
#else
#define qxmpp_loggable_trace(x) (x)
#endif

/// \brief The QXmppLogger class represents a sink for logging messages. 
///
/// \ingroup Core

class QXmppLogger : public QObject
{
    Q_OBJECT

public:
    /// This enum describes how log message are handled.
    enum LoggingType
    {
        NoLogging = 0,      ///< Log messages are discarded
        FileLogging = 1,    ///< Log messages are written to a file
        StdoutLogging = 2,  ///< Log messages are written to the standard output
        SignalLogging = 4,  ///< Log messages are emitted as a signal

        // Deprecated
        /// \cond
        NONE = 0,   ///< DEPRECATED Log messages are discarded
        FILE = 1,   ///< DEPRECATED Log messages are written to a file
        STDOUT = 2  ///< DEPRECATED Log messages are written to the standard output
        /// \endcond
    };

    /// This enum describes a type of log message.
    enum MessageType
    {
        NoMessage = 0,          ///< No message type
        DebugMessage = 1,       ///< Debugging message
        InformationMessage = 2, ///< Informational message
        WarningMessage = 4,     ///< Warning message
        ReceivedMessage = 8,    ///< Message received from server
        SentMessage = 16,       ///< Message sent to server
        AnyMessage = 31,        ///< Any message type
    };
    Q_DECLARE_FLAGS(MessageTypes, MessageType)

    QXmppLogger(QObject *parent = 0);
    static QXmppLogger* getLogger();

    QXmppLogger::LoggingType loggingType();
    void setLoggingType(QXmppLogger::LoggingType type);

    QString logFilePath();
    void setLogFilePath(const QString &path);

    QXmppLogger::MessageTypes messageTypes();
    void setMessageTypes(QXmppLogger::MessageTypes types);

public slots:
    void log(QXmppLogger::MessageType type, const QString& text);

signals:
    /// This signal is emitted whenever a log message is received.
    void message(QXmppLogger::MessageType type, const QString &text);

private:
    static QXmppLogger* m_logger;
    QXmppLogger::LoggingType m_loggingType;
    QString m_logFilePath;
    QXmppLogger::MessageTypes m_messageTypes;
};

/// \brief The QXmppLoggable class represents a source of logging messages. 
///
/// \ingroup Core

class QXmppLoggable : public QObject
{
    Q_OBJECT

public:
    QXmppLoggable(QObject *parent = 0);

protected:
    /// \cond
    virtual void childEvent(QChildEvent *event);
    /// \endcond

    /// Logs a debugging message.
    ///
    /// \param message

    void debug(const QString &message)
    {
        emit logMessage(QXmppLogger::DebugMessage, qxmpp_loggable_trace(message));
    }

    /// Logs an informational message.
    ///
    /// \param message

    void info(const QString &message)
    {
        emit logMessage(QXmppLogger::InformationMessage, qxmpp_loggable_trace(message));
    }

    /// Logs a warning message.
    ///
    /// \param message

    void warning(const QString &message)
    {
        emit logMessage(QXmppLogger::WarningMessage, qxmpp_loggable_trace(message));
    }

    /// Logs a received packet.
    ///
    /// \param message

    void logReceived(const QString &message)
    {
        emit logMessage(QXmppLogger::ReceivedMessage, qxmpp_loggable_trace(message));
    }

    /// Logs a sent packet.
    ///
    /// \param message

    void logSent(const QString &message)
    {
        emit logMessage(QXmppLogger::SentMessage, qxmpp_loggable_trace(message));
    }

signals:
    /// This signal is emitted to send logging messages.
    void logMessage(QXmppLogger::MessageType type, const QString &msg);
};

Q_DECLARE_OPERATORS_FOR_FLAGS(QXmppLogger::MessageTypes)
#endif // QXMPPLOGGER_H
