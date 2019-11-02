//////////////////////////////////////////////////////////////////////
//
// BeeBEEP Copyright (C) 2010-2019 Marco Mastroddi
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

#include "MDnsObject.h"


MDnsObject::MDnsObject( QObject* parent )
 : QObject( parent ), mp_dnss( 0 ), mp_socket( 0 ), m_record()
{
  setObjectName( "MDnsObject" );
}

MDnsObject::~MDnsObject()
{
  cleanUp();
}

void MDnsObject::cleanUp()
{
  if( mp_dnss )
  {
    DNSServiceRefDeallocate( mp_dnss );
    mp_dnss = 0;
  }

  if( mp_socket )
  {
    mp_socket->disconnect();
    mp_socket->deleteLater();
    mp_socket = 0;
  }
}


void MDnsObject::socketIsReadyRead( int socket_descriptor )
{
  if( mp_socket->socket() != socket_descriptor )
  {
    qDebug() << qPrintable( objectName() ) << "is activated from another socket" << socket_descriptor;
    return;
  }

  if( mp_socket->isEnabled() )
    mp_socket->setEnabled( false );

  DNSServiceErrorType error_code = DNSServiceProcessResult( mp_dnss );
  int error_code_int = (int)error_code;

  if( error_code != kDNSServiceErr_NoError )
  {
    qWarning() << qPrintable( objectName() ) << "has an error in process result:" << error_code_int;
    emit error( error_code_int );
    return;
  }

  mp_socket->setEnabled( true );
}

bool MDnsObject::checkErrorAndReadSocket( DNSServiceErrorType error_code )
{
  int error_code_int = (int)error_code;

  if( error_code != kDNSServiceErr_NoError )
  {
    qWarning() << qPrintable( objectName() ) << "has found an error with code:" << error_code_int;
    emit error( error_code_int );
    return false;
  }

  int socket_descriptor = DNSServiceRefSockFD( mp_dnss );
  if( socket_descriptor < 0 )
  {
    qWarning() << qPrintable( objectName() ) << "has an invalid socket descriptor:" << socket_descriptor;
    error_code_int = (int)kDNSServiceErr_Invalid;
    emit error( error_code_int );
    return false;
  }

  if( mp_socket )
    return false;

  mp_socket = new QSocketNotifier( socket_descriptor, QSocketNotifier::Read, this );
  connect( mp_socket, SIGNAL( activated( int ) ), this, SLOT( socketIsReadyRead( int ) ) );
  return true;
}
