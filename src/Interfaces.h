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
// Author: Marco Mastroddi (marco.mastroddi(AT)gmail.com)
//
// $Id$
//
//////////////////////////////////////////////////////////////////////

#ifndef BEEBEEP_INTERFACES_H
#define BEEBEEP_INTERFACES_H

#include <QtPlugin>
#include <QIcon>


class PluginInterface
{
public:
  virtual ~PluginInterface() {}

  inline void setEnabled( bool );
  inline bool isEnabled() const;

  virtual QString name() const = 0;
  virtual QString version() const = 0;
  virtual QString help() const = 0;
  virtual QString author() const = 0;
  virtual QIcon icon() const = 0;
  virtual QString iconFileName() const = 0;

  // Priority: 100 High, 1000 Normal, 10000 Low
  virtual int priority() const = 0;

  virtual QString coreVersion() const = 0;

protected:
  bool m_isEnabled;

};

class TextMarkerInterface : public PluginInterface
{
public:
  // FALSE break the loop
  virtual bool parseText( QString* ) = 0;

};

class ServiceInterface : public PluginInterface
{
public:
  virtual QString hostAddress() const = 0;
  virtual int hostPort() const = 0;

};

// Inline Functions
inline void PluginInterface::setEnabled( bool new_value ) { m_isEnabled = new_value; }
inline bool PluginInterface::isEnabled() const { return m_isEnabled; }


Q_DECLARE_INTERFACE( TextMarkerInterface, "beebeep.plugin.TextMarkerInterface/1.0" )
Q_DECLARE_INTERFACE( ServiceInterface, "beebeep.plugin.ServiceInterface/1.0" )

#endif // BEEBEEP_INTERFACES_H
