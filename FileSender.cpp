//////////////////////////////////////////////////////////////////////
//
// This file is part of BeeBEEP.
//
// BeeBEEP is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published
// by the Free Software Foundation, either version 3 of the License,
// or (at your option) any later version.
//
// BeeBEEP is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with BeeBEEP.  If not, see <http://www.gnu.org/licenses/>.
//
// Author: Marco Mastroddi (marco.mastroddi(AT)gmail.com)
//
// $Id$
//
//////////////////////////////////////////////////////////////////////

#include "FileSender.h"
#include "Protocol.h"


FileSender::FileSender( const QFileInfo& file_info, const QString& file_key, QObject* parent )
  : QObject( parent ), m_fileInfo( file_info ), m_key( file_key ), m_bytesSent( 0 )
{
  mp_server = new QTcpServer( this );
  mp_socket = NULL;
  connect( mp_server, SIGNAL( newConnection() ), this, SLOT( checkConnection() ) );
  QTimer::singleShot( 100, this, SLOT( startServer() ) );
}

void FileSender::startServer()
{
  if( !m_fileInfo.exists() )
  {
    emit error( tr( "%1: not found." ).arg( m_fileInfo.absoluteFilePath() ) );
    return;
  }

  if( !mp_server->listen() )
  {
    emit error( tr( "%1: unable to start the transfer." ).arg( m_fileInfo.absoluteFilePath() ) );
    return;
  }

  while( !mp_server->isListening() )
    continue;

  emit fileReady();
}

void FileSender::checkConnection()
{
  QTcpSocket *s = mp_server->nextPendingConnection();
  connect( s, SIGNAL( readyRead() ), this, SLOT( readConnectionData() ) );
  connect( s, SIGNAL( error( QAbstractSocket::SocketError ) ), this, SLOT( socketError( QAbstractSocket::SocketError ) ) );
  connect( s, SIGNAL( disconnected() ), this, SLOT( socketDisconnected() ) );
}

void FileSender::socketDisconnected()
{
  QTcpSocket* s = qobject_cast<QTcpSocket*>( sender() );
  if( s )
    closeSocket( s );
}

void FileSender::socketError( QAbstractSocket::SocketError )
{
  QTcpSocket* s = qobject_cast<QTcpSocket*>( sender() );
  if( s )
    closeSocket( s );
}

void FileSender::closeSocket( QTcpSocket* s )
{
  if( mp_socket && s == mp_socket )
  {
    if( m_file.isOpen() )
      m_file.close();
    mp_socket = NULL;
  }
  s->deleteLater();
}

void FileSender::readConnectionData()
{
  QTcpSocket *s = qobject_cast<QTcpSocket*>( sender() );
  if( !s )
    return;

  if( !s->canReadLine() )
    return;

  QByteArray line = s->readLine( MAX_BUFFER_SIZE );
  if( line.size() <= Protocol::instance().messageMinimumSize() )
    return;
  line.chop( 1 ); // remove the final char \n
  Message m = Protocol::instance().toMessage( line );
  if( !m.isValid() )
  {
    qWarning() << "Skip message cause error occcurred:" << line;
    return;
  }

  if( m.type() != Message::File )
  {
    qWarning() << "File message not arrived from connection:" << s->peerAddress().toString();
    closeSocket( s );
    return;
  }

  if( m.text() != m_key )
  {
    qWarning() << "File message not arrived from connection:" << s->peerAddress().toString();
    closeSocket( s );
    return;
  }

  sendFile( s );
}

void FileSender::sendFile( QTcpSocket* s )
{
  if( mp_socket )
  {
    return;
  }

  mp_server->close();

  mp_socket = s;

  if( m_file.isOpen() )
    m_file.close();

  m_file.setFileName( m_fileInfo.absoluteFilePath() );
  if( !m_file.open( QFile::ReadOnly ) )
  {
    return;
  }

  m_bytesSent = 0;
  QTimer::singleShot( 100, this, SLOT( sendData() ) );
}

void FileSender::sendData()
{
  if( !mp_socket )
    return;

  if( !m_file.isOpen() )
    return;

  QByteArray array_data = m_file.read( 64000 );
  m_bytesSent += mp_socket->write( array_data );

  emit bytesSent( m_bytesSent, m_fileInfo.size() );

  if( m_bytesSent == m_fileInfo.size() )
  {
    m_file.close();
    emit finished();
    return;
  }

  QTimer::singleShot( 100, this, SLOT( sendData() ) );
}
