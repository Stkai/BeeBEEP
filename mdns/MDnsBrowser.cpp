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

#include "MDnsBrowser.h"


MDnsBrowser::MDnsBrowser( QObject *parent )
 : MDnsObject( parent )
{
  setObjectName( "MDnsBrowser" );
}

void MDnsBrowser::stop()
{
  cleanUp();
}

void MDnsBrowser::browseForService( const QString& service_name )
{
  if( mp_dnss )
  {
    qWarning() << objectName() << "is already browsing the service:" << m_record.serviceName();
    return;
  }

  m_record.setServiceName( service_name );
  DNSServiceErrorType error_code = DNSServiceBrowse( &mp_dnss, 0, 0, service_name.toUtf8().constData(), 0, MDnsBrowseReply, this );
  checkErrorAndReadSocket( error_code );
}

void MDnsBrowser::MDnsBrowseReply( DNSServiceRef, DNSServiceFlags flags, quint32,
                                         DNSServiceErrorType error_code, const char *service_name,
                                         const char *registered_type, const char *reply_domain,
                                         void *browser_service_ref )
{
  MDnsBrowser *service_browser = static_cast<MDnsBrowser*>( browser_service_ref );
  if( error_code != kDNSServiceErr_NoError )
  {
    int error_code_int = (int)error_code;
    emit service_browser->error( error_code_int );
  }
  else
  {
    MDnsRecord bonjour_record( service_name, registered_type, reply_domain );

    if( flags & kDNSServiceFlagsAdd )
      emit service_browser->newRecordFound( bonjour_record );
    else
      emit service_browser->recordToRemove( bonjour_record );

    //if( !(flags & kDNSServiceFlagsMoreComing) )
    //No more is coming
  }
}
