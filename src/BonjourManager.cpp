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

#include "BonjourManager.h"
#include "BonjourBrowser.h"
#include "BonjourRegister.h"
#include "BonjourResolver.h"


BonjourManager::BonjourManager( QObject* parent )
 : QObject( parent ), m_bonjourRecords(), m_userRecords()
{
  mp_register = new BonjourRegister( this );
  mp_browser = new BonjourBrowser( this );

  connect( mp_register, SIGNAL( serviceRegistered() ), this, SLOT( serviceIsRegistered() ) );
  connect( mp_browser, SIGNAL( newRecordFound( const BonjourRecord& ) ), this, SLOT( addBonjourRecord( const BonjourRecord& ) ) );
  connect( mp_browser, SIGNAL( recordToRemove( const BonjourRecord& ) ), this, SLOT( removeBonjourRecord( const BonjourRecord& ) ) );

}

void BonjourManager::start( const QString& service_base_name, const QString& service_type, const QString& listener_address, int listener_port  )
{
  BonjourRecord br;
  br.setServiceName( QString( "%1 %2:%3" ).arg( service_base_name ).arg( listener_address ).arg( listener_port ) );
  br.setRegisteredType( service_type );
  qDebug() << "Multicast DNS service started with" << br.name();
  mp_register->registerService( br, listener_port );
}

void BonjourManager::stop()
{
  mp_register->unregisterService();
  mp_browser->stop();
  m_bonjourRecords.clear();
  m_userRecords.clear();
  qDebug() << "Multicast DNS service closed";
}

void BonjourManager::serviceIsRegistered()
{
  qDebug() << "Bonjour has registered service" << mp_register->record().name() << "on port" << mp_register->servicePort();
  mp_browser->browseForService( mp_register->record().registeredType() );
}

void BonjourManager::addBonjourRecord( const BonjourRecord& bonjour_record )
{
  if( m_bonjourRecords.contains( bonjour_record ) )
    return;

 #ifdef BEEBEEP_DEBUG
   qDebug() << "Bonjour adds new record" << bonjour_record.name();
 #endif
   m_bonjourRecords.append( bonjour_record );
   BonjourResolver* resolver = new BonjourResolver( this );
   connect( resolver, SIGNAL( resolved( const QHostInfo&, int ) ), this, SLOT( serviceResolved(const QHostInfo&, int ) ) );
   resolver->resolve( bonjour_record );
}

void BonjourManager::removeBonjourRecord( const BonjourRecord& bonjour_record )
{
#ifdef BEEBEEP_DEBUG
  qDebug() << "Bonjour removes record" << bonjour_record.name();
#endif
  m_bonjourRecords.removeOne( bonjour_record );
}

void BonjourManager::addUserRecord( const UserRecord& ur )
{
  if( m_userRecords.contains( ur ) )
    return;

#ifdef BEEBEEP_DEBUG
  qDebug() << "Bonjour add new user record" << ur.hostAddressAndPort();
#endif
  m_userRecords.append( ur );
  emit newUserFound( ur );
}

void BonjourManager::serviceResolved( const QHostInfo& host_info, int host_port )
{
  BonjourResolver *resolver = qobject_cast<BonjourResolver*>( sender() );
  if( !resolver )
  {
    qWarning() << "Bonjour received a signal from invalid BonjourResolver instance";
    return;
  }

  const QList<QHostAddress>& host_addresses = host_info.addresses();

  QHostAddress ipv4_address;
  QHostAddress ipv6_address;

  foreach( QHostAddress ha, host_addresses )
  {
#ifdef BEEBEEP_DEBUG
    qDebug() << "Bonjour has resolved host" << host_info.hostName() << "with this address" << ha.toString();
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
    ur.setComment( QString( "Bonjour[%1]" ).arg( resolver->record().name() ) );
    addUserRecord( ur );
  }

  resolver->deleteLater();
}

