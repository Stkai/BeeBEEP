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
// Author: Marco Mastroddi <marco.mastroddi(AT)gmail.com>
//
// $Id$
//
//////////////////////////////////////////////////////////////////////

#include "MDnsManager.h"
#include "MDnsBrowser.h"
#include "MDnsRegister.h"
#include "MDnsResolver.h"


MDnsManager::MDnsManager( QObject* parent )
 : QObject( parent ), m_mdnsRecords(), m_userRecords(), m_isActive( false )
{
  setObjectName( "MDnsManager" );

  mp_register = new MDnsRegister( this );
  mp_browser = new MDnsBrowser( this );

  connect( mp_register, SIGNAL( serviceRegistered() ), this, SLOT( serviceIsRegistered() ) );
  connect( mp_browser, SIGNAL( newRecordFound( const MDnsRecord& ) ), this, SLOT( addMDnsRecord( const MDnsRecord& ) ) );
  connect( mp_browser, SIGNAL( recordToRemove( const MDnsRecord& ) ), this, SLOT( removeMDnsRecord( const MDnsRecord& ) ) );
}

bool MDnsManager::start( const QString& service_base_name, const QString& service_type, const QString& listener_address, int listener_port  )
{
  if( isActive() )
  {
#ifdef BEEBEEP_DEBUG
    qDebug() << qPrintable( objectName() ) << "is already active";
#endif
    return false;
  }

  MDnsRecord mdr;
  mdr.setServiceName( QString( "%1 %2:%3" ).arg( service_base_name ).arg( listener_address ).arg( listener_port ) );
  mdr.setRegisteredType( service_type );
  qDebug() << qPrintable( objectName() ) << "starting with" << mdr.name();
  if( mp_register->registerService( mdr, listener_port ) )
  {
    m_isActive = true;
    return true;
  }
  else
    return false;
}

bool MDnsManager::stop()
{
  if( !isActive() )
  {
#ifdef BEEBEEP_DEBUG
    qDebug() << qPrintable( objectName() ) << "is already closed";
#endif
    return false;
  }

  qDebug() << qPrintable( objectName() ) << "has unregistered service" << mp_register->record().name() << "on port" << mp_register->servicePort();
  mp_register->unregisterService();
  mp_browser->stop();
  m_mdnsRecords.clear();
  m_userRecords.clear();
  m_isActive = false;
  return true;
}

bool MDnsManager::browseForService()
{
  if( !mp_register->serviceIsRegistered() )
  {
    qDebug() << qPrintable( objectName() ) << "has not registered the service and can not browse for it";
    return false;
  }

  if( mp_browser->isActive() )
    mp_browser->stop();

  return mp_browser->browseForService( mp_register->record().registeredType() );
}

void MDnsManager::serviceIsRegistered()
{
  qDebug() << qPrintable( objectName() ) << "has registered service" << mp_register->record().name() << "on port" << mp_register->servicePort();
  emit serviceRegistered();
}

void MDnsManager::addMDnsRecord( const MDnsRecord& mdns_record )
{
  if( m_mdnsRecords.contains( mdns_record ) )
    return;

#ifdef BEEBEEP_DEBUG
  qDebug() << qPrintable( objectName() ) << "adds new dns record" << mdns_record.name();
#endif
  m_mdnsRecords.append( mdns_record );

  MDnsResolver* resolver = new MDnsResolver( this );
  connect( resolver, SIGNAL( resolved( const QHostInfo&, int ) ), this, SLOT( serviceResolved( const QHostInfo&, int ) ) );
  resolver->resolve( mdns_record );
#ifdef BEEBEEP_DEBUG
  qDebug() << qPrintable( objectName() ) << "is now searching for" << mdns_record.name();
#endif
}

void MDnsManager::removeMDnsRecord( const MDnsRecord& mdns_record )
{
#ifdef BEEBEEP_DEBUG
  qDebug() << qPrintable( objectName() ) << "removes dns record" << mdns_record.name();
#endif
  m_mdnsRecords.removeOne( mdns_record );
}

void MDnsManager::addUserRecord( const UserRecord& ur )
{
  if( m_userRecords.contains( ur ) )
  {
#ifdef BEEBEEP_DEBUG
    qDebug() << qPrintable( objectName() ) << "already contains user record" << ur.hostAddressAndPort();
#endif
    return;
  }

#ifdef BEEBEEP_DEBUG
  qDebug() << qPrintable( objectName() ) << "adds new user record" << ur.hostAddressAndPort();
#endif
  m_userRecords.append( ur );
  emit newUserFound( ur );
}

void MDnsManager::serviceResolved( const QHostInfo& host_info, int host_port )
{
  MDnsResolver *resolver = qobject_cast<MDnsResolver*>( sender() );
  if( !resolver )
  {
    qWarning() << qPrintable( objectName() ) << "has received a signal from invalid MDnsResolver instance";
    return;
  }

  const QList<QHostAddress>& host_addresses = host_info.addresses();

  QHostAddress ipv4_address;
  QHostAddress ipv6_address;

  foreach( QHostAddress ha, host_addresses )
  {
#ifdef BEEBEEP_DEBUG
    qDebug() << qPrintable( objectName() ) << "has resolved host" << host_info.hostName() << "with this address" << ha.toString();
#endif
    if( ha.toString().contains( ":" ) )
      ipv6_address = ha;
    else
      ipv4_address = ha;
  }

  if( !ipv4_address.isNull() || !ipv6_address.isNull() )
  {
    UserRecord ur;
    if( !ipv4_address.isNull() )
      ur.setHostAddress( ipv4_address );
    else
      ur.setHostAddress( ipv6_address );
    ur.setHostPort( host_port );
    ur.setComment( QString( "MDns[%1]" ).arg( resolver->record().name() ) );
    addUserRecord( ur );
  }

  resolver->deleteLater();
}
