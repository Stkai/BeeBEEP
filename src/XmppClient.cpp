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

#include "XmppClient.h"
#include "FileInfo.h"
#include "Settings.h"
#include "QXmppRosterManager.h"
#include "QXmppVCardIq.h"
#include "QXmppVCardManager.h"


XmppClient::XmppClient( QObject* parent )
  : QXmppClient( parent ), m_clientUser(),
    m_iconPath( ":/images/jabber.png" ),
    m_connectionState( XmppClient::Offline )
{
  setService( "Jabber" );

  mp_transferManager = new QXmppTransferManager;

  connect( &(rosterManager()), SIGNAL( rosterReceived() ), this, SIGNAL( rosterReceived() ) );
  connect( &(rosterManager()), SIGNAL( presenceChanged( const QString&, const QString& ) ), this, SIGNAL( presenceChanged( const QString&, const QString& ) ) );
  connect( &(vCardManager()), SIGNAL( vCardReceived( const QXmppVCardIq& ) ), this, SIGNAL( vCardReceived( const QXmppVCardIq& ) ) );
  connect( &(vCardManager()), SIGNAL( clientVCardReceived() ), this, SIGNAL( clientVCardReceived() ) );
}

void XmppClient::setupManagers()
{
  mp_transferManager->setSupportedMethods( QXmppTransferJob::InBandMethod | QXmppTransferJob::SocksMethod );
  addExtension( mp_transferManager );
  connect( mp_transferManager, SIGNAL( fileReceived( QXmppTransferJob* ) ), this, SLOT( checkFileTransferRequest( QXmppTransferJob* ) ) );
}

void XmppClient::sendFile( const QString& bare_jid, const FileInfo& fi )
{
  qDebug() << "XMPP> sending file" << fi.name() << "to" << bare_jid;
  QXmppTransferJob *job = mp_transferManager->sendFile( bare_jid, fi.path(), QString::number( fi.id() ) );
  qDebug() << "XMPP> started file transfer job with sid" << job->sid();
  setupFileTransferJob( job );
}

void XmppClient::setupFileTransferJob( QXmppTransferJob* job )
{
  connect( job, SIGNAL( error( QXmppTransferJob::Error ) ), this, SLOT( checkFileTransferError( QXmppTransferJob::Error ) ) );
  connect( job, SIGNAL( finished() ),this, SLOT( checkFileTransferFinished() ) );
  connect( job, SIGNAL( progress( qint64, qint64 ) ),  this, SLOT( checkFileTransferProgress( qint64, qint64 ) ) );
}

void XmppClient::checkFileTransferRequest( QXmppTransferJob* job )
{
  // Disabled
  qDebug() << "XMPP> file transfer request from" << job->jid() << ":" << job->fileName();
  job->abort();
}

void XmppClient::checkFileTransferError( QXmppTransferJob::Error tje )
{
  QXmppTransferJob *job = qobject_cast<QXmppTransferJob*>(sender());
  if( !job )
  {
    qWarning() << "Unable to cast qobject in QXmppTransferJob in XmppClient::checkFileTransferError";
    return;
  }
  qDebug() << "XMPP> error occurred" << tje << "transferring file" << job->fileName() << "to" << job->jid();
}

void XmppClient::checkFileTransferFinished()
{
  QXmppTransferJob *job = qobject_cast<QXmppTransferJob*>(sender());
  if( !job )
  {
    qWarning() << "Unable to cast qobject in QXmppTransferJob in XmppClient::checkFileTransferFinished";
    return;
  }
  qDebug() << "XMPP> transfer file" << job->fileName() << "to" << job->jid() << "finished";
}

void XmppClient::checkFileTransferProgress( qint64 bytes_transferred, qint64 total_bytes )
{
  QXmppTransferJob *job = qobject_cast<QXmppTransferJob*>(sender());
  if( !job )
  {
    qWarning() << "Unable to cast qobject in QXmppTransferJob in XmppClient::checkFileTransferProgress";
    return;
  }

  qDebug() << "XMPP> transfer file" << job->fileName() << "to" << job->jid() << "in progress:" << bytes_transferred << "of" << total_bytes << "bytes";
}

void XmppClient::sendVCard( const QXmppVCardIq& vciq )
{
  if( !isConnected() )
    return;

  QByteArray photo_hash = Settings::instance().localUser().vCard().photoHash();

  vCardManager().setClientVCard( vciq );
  QXmppPresence presence = clientPresence();
  if( vCardManager().isClientVCardReceived() )
  {
    if( photo_hash.isEmpty() )
      presence.setVCardUpdateType( QXmppPresence::VCardUpdateNoPhoto );
    else
      presence.setVCardUpdateType( QXmppPresence::VCardUpdateValidPhoto );
  }
  else
  {
    presence.setVCardUpdateType( QXmppPresence::VCardUpdateNone );
  }

  presence.setPhotoHash( photo_hash );
  sendPacket( vciq );
  setClientPresence( presence );
}

