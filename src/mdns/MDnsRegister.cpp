//////////////////////////////////////////////////////////////////////
//
// BeeBEEP Copyright (C) 2010-2021 Marco Mastroddi
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
// along with BeeBEEP. If not, see <http://www.gnu.org/licenses/>.
//
// Author: Marco Mastroddi <marco.mastroddi(AT)gmail.com>
//
// $Id$
//
//////////////////////////////////////////////////////////////////////

#include "MDnsRegister.h"


MDnsRegister::MDnsRegister( QObject *parent )
 : MDnsObject( parent ), m_servicePort( 0 ), m_serviceIsRegistered( false )
{
  setObjectName( "MDnsRegister" );
}

void MDnsRegister::unregisterService()
{
  m_serviceIsRegistered = false;
  m_servicePort = 0;
  cleanUp();
}

bool MDnsRegister::registerService( const MDnsRecord& mdns_record, int service_port )
{
  if( mp_dnss )
  {
    qWarning() << qPrintable( objectName() ) << "has already registered a service" << m_record.registeredType() << "on port" << m_servicePort;
    return false;
  }

  qDebug() << qPrintable( objectName() ) << "is starting to register service" << mdns_record.registeredType() << "on port" << service_port;

  DNSServiceErrorType error_code = DNSServiceRegister( &mp_dnss, 0, 0, mdns_record.serviceName().toUtf8().constData(),
                                                 mdns_record.registeredType().toUtf8().constData(),
                                                 mdns_record.replyDomain().isEmpty() ? 0 : mdns_record.replyDomain().toUtf8().constData(),
                                                 0, service_port, 0, 0, (DNSServiceRegisterReply)MDnsRegisterService, this );
  if( !checkErrorAndReadSocket( error_code ) )
  {
    qWarning() << qPrintable( objectName() ) << "can not register service" << mdns_record.registeredType() << "on port" << service_port;
    return false;
  }
  else
  {
    m_servicePort = service_port;
    qDebug() << qPrintable( objectName() ) << "tries to register service" << mdns_record.registeredType() << "on port" << service_port;
    return true;
  }
}

void MDnsRegister::MDnsRegisterService( DNSServiceRef, DNSServiceFlags,
                                              DNSServiceErrorType error_code, const char *service_name,
                                              const char *registered_type, const char *reply_domain,
                                              void *register_service_ref )
{
  MDnsRegister *service_register = static_cast<MDnsRegister*>( register_service_ref );
  if( error_code != kDNSServiceErr_NoError )
  {
    int error_code_int = (int)error_code;
    qWarning() << "MDnsRegister has found an error with code:" << error_code_int;
    emit service_register->error( error_code_int );
  }
  else
  {
    MDnsRecord mdr( service_name, registered_type, reply_domain );
    service_register->setRecord( mdr );
    service_register->setServiceRegistered( true );
    emit service_register->serviceRegistered();
  }
}
