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

#include "BonjourRegister.h"


BonjourRegister::BonjourRegister( QObject *parent )
 : BonjourObject( parent )
{
  setObjectName( "BonjourRegister" );
}

void BonjourRegister::unregisterService()
{
  cleanUp();
}

bool BonjourRegister::registerService( const BonjourRecord& bonjour_record, int service_port )
{
  if( mp_dnss )
  {
    qWarning() << objectName() << "has already registered a service:" << m_record.name();
    return false;
  }

  DNSServiceErrorType error_code = DNSServiceRegister( &mp_dnss, 0, 0, bonjour_record.serviceName().toUtf8().constData(),
                                                 bonjour_record.registeredType().toUtf8().constData(),
                                                 bonjour_record.replyDomain().isEmpty() ? 0 : bonjour_record.replyDomain().toUtf8().constData(),
                                                 0, service_port, 0, 0, BonjourRegisterService, this );
  if( !checkErrorAndReadSocket( error_code ) )
  {
    qWarning() << objectName() << "can not register" << bonjour_record.name() << "on port" << service_port;
    return false;
  }
  else
    return true;
}

void BonjourRegister::BonjourRegisterService( DNSServiceRef, DNSServiceFlags,
                                              DNSServiceErrorType error_code, const char *service_name,
                                              const char *registered_type, const char *reply_domain,
                                              void *register_service_ref )
{
  BonjourRegister *service_register = static_cast<BonjourRegister*>( register_service_ref );
  if( error_code != kDNSServiceErr_NoError )
  {
    int error_code_int = (int)error_code;
    emit service_register->error( error_code_int );
  }
  else
  {
    service_register->setRecord( BonjourRecord( service_name, registered_type, reply_domain ) );
    emit service_register->serviceRegistered();
  }
}
