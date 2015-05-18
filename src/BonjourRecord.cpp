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

#include "BonjourRecord.h"


BonjourRecord::BonjourRecord()
  : m_serviceName( "" ), m_registeredType( "" ), m_replyDomain( "" )
{}

BonjourRecord::BonjourRecord( const BonjourRecord& br )
{
  (void)operator=( br );
}

BonjourRecord::BonjourRecord(const char *service_name, const char *registered_type, const char *reply_domain )
{
  m_serviceName = QString::fromUtf8( service_name );
  m_registeredType = QString::fromUtf8( registered_type );
  m_replyDomain = QString::fromUtf8( reply_domain );
}

BonjourRecord& BonjourRecord::operator=( const BonjourRecord& br )
{
  if( this != &br )
  {
    m_serviceName = br.m_serviceName;
    m_registeredType = br.m_registeredType;
    m_replyDomain = br.m_replyDomain;
  }
  return *this;
}

/*
 *

The .lib distributed by Apple can be used only if you are compiling the Qt application with the MSVC compiler.

Otherwise, like you said, you need a GCC-compatible library (.a). To do that you need to do the following steps:

    Run the reimp tool [0] on the .lib: reimp dnssd.lib. A file DLLStub.obj will be generated.
    Run the gendef tool [1] on the .dll: gendef dnssd.dll. A file dnssd.def will be generated. The .dll can be obtained from: C:\Windows\System32 if you are using the 32 bit or from C:\Windows\SysWOW64 for the 64 bit version.
    Assemble the final .a: dlltool -k -d dnssd.def -l libdnssd.a.
    Add the right path int the .pro file, to the newly created library: LIBS += -L"/path/to/the/library/file" -ldnssd

[0] - http://sourceforge.net/projects/mingw/files/MinGW/Extension/mingw-utils/mingw-utils-0.4-1/

[1] - http://sourceforge.net/projects/mingw/files/MinGW/Extension/gendef/gendef-1.0.1346/ - gendef is a better alternative to pexports, because it can convert the stdcall-type libraries from MSVC to the GCC ones, so you can get a proper .def file.

PS: I know the author got it working, but I felt there should be some more detailed instructions on how to get it done -- the information is scattered around the internet.

*/
