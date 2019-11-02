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

#ifndef BEEBEEP_GUIICONPROVIDER_H
#define BEEBEEP_GUIICONPROVIDER_H

#include "Config.h"
class FileInfo;


class GuiIconProvider
{
// Singleton Object
  static GuiIconProvider* mp_instance;

public:
  QIcon findIcon( const FileInfo& );
  QIcon iconFromFileType( int file_type );
  inline int cacheSize() const;
  void clearCache();

  static GuiIconProvider& instance()
  {
    if( !mp_instance )
      mp_instance = new GuiIconProvider();
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
  GuiIconProvider();

private:
  QMap<QString, QIcon> m_cache;
  QFileIconProvider m_provider;

};

// Inline Functions
inline int GuiIconProvider::cacheSize() const { return m_cache.size(); }

#endif // BEEBEEP_GUIICONPROVIDER_H
