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
// $Id: Interfaces.h 346 2015-04-05 16:12:37Z mastroddi $
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
  virtual bool parseBeforeSending() const = 0;

  virtual QString openCommand() const = 0;
  virtual QString closeCommand() const = 0;
  virtual QString openString() const = 0;
  virtual QString closeString() const = 0;
  virtual void initParser( const QString& ) = 0; // pass the string between open and close command
  virtual QString parseString( const QString& ) = 0; // FALSE break the loop

};

// Inline Functions
inline void PluginInterface::setEnabled( bool new_value ) { m_isEnabled = new_value; }
inline bool PluginInterface::isEnabled() const { return m_isEnabled; }

#if QT_VERSION >= 0x050000
Q_DECLARE_INTERFACE( TextMarkerInterface, "beebeep.plugin.TextMarkerInterface/2.0" )
#else
Q_DECLARE_INTERFACE( TextMarkerInterface, "beebeep.plugin.TextMarkerInterface/1.5" )
#endif


#endif // BEEBEEP_INTERFACES_H
