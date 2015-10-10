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
 : MDnsObject( parent ), m_isActive( false )
{
  setObjectName( "MDnsBrowser" );
}

void MDnsBrowser::stop()
{
  m_isActive = false;
  cleanUp();
}

bool MDnsBrowser::browseForService( const QString& service_type )
{
  if( mp_dnss )
  {
    qWarning() << objectName() << "is already browsing the service:" << m_record.name();
    return false;
  }

#ifdef BEEBEEP_DEBUG
  qDebug() << objectName() << "starts to browse for the service:" << service_type;
#endif

  m_record.setRegisteredType( service_type );
  DNSServiceErrorType error_code = DNSServiceBrowse( &mp_dnss, 0, 0, service_type.toUtf8().constData(), 0, MDnsBrowseReply, this );
  m_isActive = checkErrorAndReadSocket( error_code );
  return m_isActive;
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
    MDnsRecord mdns_record( service_name, registered_type, reply_domain );

    //if( !(flags & kDNSServiceFlagsMoreComing) )
    //No more is coming, we can update the GUI...

    if( flags & kDNSServiceFlagsAdd )
      emit service_browser->newRecordFound( mdns_record );
    else
      emit service_browser->recordToRemove( mdns_record );

  }
}
