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

#include "BonjourResolver.h"


BonjourResolver::BonjourResolver( QObject* parent )
 : QObject( parent ), mp_dnss( 0 ), mp_socket( 0 ), m_servicePort( -1 )
{
}

BonjourResolver::~BonjourResolver()
{
  cleanupAfterResolve();
}

void BonjourResolver::cleanupAfterResolve()
{
  if( mp_dnss )
  {
    DNSServiceRefDeallocate( mp_dnss );
    mp_dnss = 0;
    delete mp_socket;
    m_servicePort = -1;
  }
}

void BonjourResolver::resolve( const BonjourRecord& bonjour_record )
{
  if( mp_dnss )
  {
    qWarning() << "Bonjour resolver already has a resolve in process";
    return;
  }

  DNSServiceErrorType error_code = DNSServiceResolve(&mp_dnss, 0, 0,
                                                bonjour_record.serviceName().toUtf8().constData(),
                                                bonjour_record.registeredType().toUtf8().constData(),
                                                bonjour_record.replyDomain().toUtf8().constData(),
                                                (DNSServiceResolveReply)BonjourResolveReply, this );

  int error_code_int = (int)error_code;

  if( error_code != kDNSServiceErr_NoError )
  {
    qWarning() << "Bonjour resolver service has an error:" << error_code_int;
    emit error( error_code_int );
  }
  else
  {
    int socket_descriptor = DNSServiceRefSockFD( mp_dnss );
    if( socket_descriptor < 0 )
    {
      qWarning() << "Bonjour resolver has an invalid socket descriptor:" << socket_descriptor;
      error_code_int = (int)kDNSServiceErr_Invalid;
      emit error( error_code_int );
    }
    else
    {
      mp_socket = new QSocketNotifier( socket_descriptor, QSocketNotifier::Read, this );
      connect( mp_socket, SIGNAL( activated( int ) ), this, SLOT( socketIsReadyRead() ) );
    }
  }
}

void BonjourResolver::socketIsReadyRead()
{
  DNSServiceErrorType error_code = DNSServiceProcessResult( mp_dnss );
  int error_code_int = (int)error_code;

  if( error_code != kDNSServiceErr_NoError )
  {
    qWarning() << "Bonjour resolver has an error in process result:" << error_code_int;
    emit error( error_code_int );
  }
}


void BonjourResolver::BonjourResolveReply( DNSServiceRef, DNSServiceFlags,
                                    quint32, DNSServiceErrorType error_code,
                                    const char*, const char* host_target, quint16 service_port,
                                    quint16, const char*, void *resolver_service_ref )
{
  BonjourResolver *service_resolver = static_cast<BonjourResolver*>( resolver_service_ref );

  if( error_code != kDNSServiceErr_NoError )
  {
    int error_code_int = (int)error_code;
    emit service_resolver->error( error_code_int );
    return;
  }

  service_resolver->setServicePort( service_port );

  QHostInfo::lookupHost( QString::fromUtf8( host_target ), service_resolver, SLOT( completeConnection( const QHostInfo& ) ) );
}

void BonjourResolver::completeConnection( const QHostInfo& host_info )
{
  emit resolved( host_info, m_servicePort );
  QMetaObject::invokeMethod( this, "cleanupAfterResolve", Qt::QueuedConnection );
}
