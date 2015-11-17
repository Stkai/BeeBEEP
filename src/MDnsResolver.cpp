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

#include "MDnsResolver.h"


MDnsResolver::MDnsResolver( QObject* parent )
 : MDnsObject( parent ), m_servicePort( -1 ), m_lookUpHostId( -1 )
{
  setObjectName( "MDnsResolver" );
}

MDnsResolver::~MDnsResolver()
{
  cleanUp();
}

void MDnsResolver::cleanUp()
{
  if( m_lookUpHostId != -1 )
    QHostInfo::abortHostLookup( m_lookUpHostId );
  MDnsObject::cleanUp();
}

void MDnsResolver::resolve( const MDnsRecord& mdns_record )
{
  if( mp_dnss )
  {
    qWarning() << qPrintable( objectName() ) << "already has a resolve in process. Skip this one:" << m_record.name();
    return;
  }

  m_record = mdns_record;
  DNSServiceErrorType error_code = DNSServiceResolve( &mp_dnss, 0, 0,
                                                mdns_record.serviceName().toUtf8().constData(),
                                                mdns_record.registeredType().toUtf8().constData(),
                                                mdns_record.replyDomain().toUtf8().constData(),
                                                (DNSServiceResolveReply)MDnsResolveReply, this );

  checkErrorAndReadSocket( error_code );
}

void MDnsResolver::MDnsResolveReply( DNSServiceRef, DNSServiceFlags,
                                    quint32, DNSServiceErrorType error_code,
                                    const char*, const char* host_target, quint16 service_port,
                                    quint16, const char*, void *resolver_service_ref )
{
  MDnsResolver *service_resolver = static_cast<MDnsResolver*>( resolver_service_ref );

  if( error_code != kDNSServiceErr_NoError )
  {
    int error_code_int = (int)error_code;
    qWarning() << "MDnsResolver has found an error with code:" << error_code_int;
    emit service_resolver->error( error_code_int );
    return;
  }

  service_resolver->setServicePort( service_port );

  int lookup_id = QHostInfo::lookupHost( QString::fromUtf8( host_target ), service_resolver, SLOT( lookedUp( const QHostInfo& ) ) );
  service_resolver->setLookUpHostId( lookup_id );
}

void MDnsResolver::lookedUp( const QHostInfo& host_info )
{
  m_lookUpHostId = -1;

  if( host_info.error() != QHostInfo::NoError )
  {
    qWarning() << qPrintable( objectName() ) << "can not resolve" << m_record.name();
    deleteLater();
    return;
  }

  QString local_host_name = QHostInfo::localHostName();
  if( host_info.hostName() == local_host_name )
  {
#ifdef BEEBEEP_DEBUG
    qDebug() << qPrintable( objectName() ) << "skips local host name:" << host_info.hostName();
#endif
    deleteLater();
    return;
  }

  local_host_name.append( QString( ".local." ) );
  if( host_info.hostName() == local_host_name )
  {
#ifdef BEEBEEP_DEBUG
    qDebug() << qPrintable( objectName() ) << "skips local host name :" << host_info.hostName();
#endif
    deleteLater();
    return;
  }

  emit resolved( host_info, m_servicePort );
}
