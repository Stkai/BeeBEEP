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

#include "BonjourBrowser.h"


BonjourBrowser::BonjourBrowser( QObject *parent )
 : QObject( parent ), mp_dnss( 0 ), mp_socket( 0 ), m_records(), m_serviceType( "" )
{
}

BonjourBrowser::~BonjourBrowser()
{
  if( mp_dnss )
  {
    DNSServiceRefDeallocate( mp_dnss );
    mp_dnss = 0;
  }
}

void BonjourBrowser::browseForServiceType( const QString& service_type )
{
  m_serviceType = service_type;
  DNSServiceErrorType error_code = DNSServiceBrowse( &mp_dnss, 0, 0, service_type.toUtf8().constData(), 0, BonjourBrowseReply, this );
  int error_code_int = (int)error_code;

  if( error_code != kDNSServiceErr_NoError )
  {
    qWarning() << "Bonjour browse service has an error:" << error_code_int;
    emit error( error_code_int );
  }
  else
  {
    int socket_descriptor = DNSServiceRefSockFD( mp_dnss );
    if( socket_descriptor < 0 )
    {
      qWarning() << "Bonjour browser has an invalid socket descriptor:" << socket_descriptor;
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

void BonjourBrowser::socketIsReadyRead()
{
  DNSServiceErrorType error_code = DNSServiceProcessResult( mp_dnss );
  int error_code_int = (int)error_code;

  if( error_code != kDNSServiceErr_NoError )
  {
    qWarning() << "Bonjour browser has an error in process result:" << error_code_int;
    emit error( error_code_int );
  }
}

bool BonjourBrowser::addRecord( const BonjourRecord& bonjour_record )
{
  if( m_records.contains( bonjour_record ) )
    return false;

  m_records.append( bonjour_record );
  return true;
}

void BonjourBrowser::BonjourBrowseReply( DNSServiceRef, DNSServiceFlags flags, quint32,
                                         DNSServiceErrorType error_code, const char *service_name,
                                         const char *registered_type, const char *reply_domain,
                                         void *browser_service_ref )
{
  BonjourBrowser *service_browser = static_cast<BonjourBrowser*>( browser_service_ref );
  if( error_code != kDNSServiceErr_NoError )
  {
    int error_code_int = (int)error_code;
    emit service_browser->error( error_code_int );
  }
  else
  {
    BonjourRecord bonjour_record( service_name, registered_type, reply_domain );
    if( flags & kDNSServiceFlagsAdd )
      service_browser->addRecord( bonjour_record );
    else
      service_browser->removeRecord( bonjour_record );

    if( !(flags & kDNSServiceFlagsMoreComing) )
      emit service_browser->recordsChanged();
  }
}
