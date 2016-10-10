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

#ifndef BEEBEEP_HIVE_H
#define BEEBEEP_HIVE_H

#include "Config.h"
#include "NetworkEntry.h"


class Hive
{
// Singleton Object
  static Hive* mp_instance;

public:
  bool addNetworkAddress( const NetworkAddress& );
  inline const QList<NetworkAddress>& networkAddresses() const;


  static Hive& instance()
  {
    if( !mp_instance )
      mp_instance = new Hive();
    return *mp_instance;
  }

  static void close()
  {
    if( mp_instance )
    {
      delete mp_instance;
      mp_instance = NULL;
    }
  }

protected:
  Hive();

private:
  QList<NetworkAddress> m_networkAddresses;

};


// Inline Function
inline const QList<NetworkAddress>& Hive::networkAddresses() const { return m_networkAddresses; }

#endif // BEEBEEP_HIVE_H
