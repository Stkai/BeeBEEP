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

#ifndef BEEBEEP_FIREWALLMANAGER_H
#define BEEBEEP_FIREWALLMANAGER_H

#include "Config.h"


class FirewallManager
{
// Singleton Object
  static FirewallManager* mp_instance;

public:
  bool allowApplication( const QString& app_name, const QString& app_path );

  static FirewallManager& instance()
  {
    if( !mp_instance )
      mp_instance = new FirewallManager();
    return *mp_instance;
  }

  static void close()
  {
    if( mp_instance )
    {
      delete mp_instance;
      mp_instance = Q_NULLPTR;
    }
  }

protected:
  FirewallManager();


};

#endif // BEEBEEP_FIREWALLMANAGER_H
