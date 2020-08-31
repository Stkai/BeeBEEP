//////////////////////////////////////////////////////////////////////
//
// BeeBEEP Copyright (C) 2010-2020 Marco Mastroddi
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

#ifndef BEEBEEP_ICONMANAGER_H
#define BEEBEEP_ICONMANAGER_H

#include "Config.h"


class IconManager
{
// Singleton Object
  static IconManager* mp_instance;

public:
  inline QIcon icon( const QString& icon_name ) const;
  QString iconPath( const QString& icon_name ) const;
  inline const QString& defaultSourcePath() const;
  inline const QString& currentSourcePath() const;
  inline void setSourcePath( const QString& );
  inline const QString& sourcePath() const;
  inline QString toHtml( const QString& icon_name, const QString& icon_alt, int icon_width = 0, int icon_height = 0 );
  inline bool hasDefaultIcons() const;

  static IconManager& instance()
  {
    if( !mp_instance )
      mp_instance = new IconManager();
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
  IconManager();
  inline QString iconPath( const QString& source_path, const QString& icon_name ) const;
  inline QString defaulIconPath( const QString& icon_name ) const;
  inline QString customIconPath( const QString& icon_name ) const;

private:
  QString m_defaultSourcePath;
  QString m_sourcePath;

};


// Inline Functions
inline QIcon IconManager::icon( const QString& icon_name ) const { return QIcon( iconPath( icon_name ) ); }
inline QString IconManager::defaulIconPath( const QString& icon_name ) const { return QString( "%1/%2" ).arg( m_defaultSourcePath, icon_name ); }
inline QString IconManager::customIconPath( const QString& icon_name ) const { return QString( "%1/%2" ).arg( m_sourcePath, icon_name ); }
inline const QString& IconManager::defaultSourcePath() const { return m_defaultSourcePath; }
inline const QString& IconManager::currentSourcePath() const { return hasDefaultIcons() ? m_defaultSourcePath : m_sourcePath; }
inline void IconManager::setSourcePath( const QString& new_value ) { m_sourcePath = new_value; }
inline const QString& IconManager::sourcePath() const { return m_sourcePath; }
inline QString IconManager::toHtml( const QString& icon_name, const QString& icon_alt, int icon_width, int icon_height ) { return QString( "<img src='%1' width=%2 height=%3 border=0 alt=' %4 ' />" ).arg( iconPath( icon_name ) ).arg( icon_width > 0 ? icon_width : 16 ).arg( icon_height > 0 ? icon_height : 16 ).arg( icon_alt ); }
inline bool IconManager::hasDefaultIcons() const { return m_sourcePath.isEmpty(); }

#endif // BEEBEEP_ICONMANAGER_H
