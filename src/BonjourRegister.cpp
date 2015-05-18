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
 : QObject( parent ), mp_dnss( 0 ), mp_socket( 0 ), m_registeredService()
{
}

BonjourRegister::~BonjourRegister()
{
  if( mp_dnss )
  {
    DNSServiceRefDeallocate( mp_dnss );
    mp_dnss = 0;
  }
}

void BonjourRegister::registerService( const BonjourRecord& bonjour_record, int service_port )
{
  if( mp_dnss )
  {
    qWarning() << "Bonjour has already registered a service like this:" << bonjour_record.serviceName();
    return;
  }

  DNSServiceErrorType error_code = DNSServiceRegister( &mp_dnss, 0, 0, bonjour_record.serviceName().toUtf8().constData(),
                                                 bonjour_record.registeredType().toUtf8().constData(),
                                                 bonjour_record.replyDomain().isEmpty() ? 0 : bonjour_record.replyDomain().toUtf8().constData(),
                                                 0, service_port, 0, 0, BonjourRegisterService, this );
  int error_code_int = (int)error_code;

  if( error_code != kDNSServiceErr_NoError )
  {
    qWarning() << "Bonjour register service has an error:" << error_code_int;
    emit error( error_code_int );
  }
  else
  {
    int socket_descriptor = DNSServiceRefSockFD( mp_dnss );
    if( socket_descriptor < 0 )
    {
      qWarning() << "Bonjour register has an invalid socket descriptor:" << socket_descriptor;
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

void BonjourRegister::socketIsReadyRead()
{
  DNSServiceErrorType error_code = DNSServiceProcessResult( mp_dnss );
  int error_code_int = (int)error_code;

  if( error_code != kDNSServiceErr_NoError )
  {
    qWarning() << "Bonjour register has an error in process result:" << error_code_int;
    emit error( error_code_int );
  }
}

void BonjourRegister::BonjourRegisterService( DNSServiceRef, DNSServiceFlags flags,
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
    service_register->setRegisteredService( BonjourRecord( service_name, registered_type, reply_domain ) );
    emit service_register->serviceRegistered();
  }
}
