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

#ifndef BEE_LIFEGAME_H
#define BEE_LIFEGAME_H

#include "lifegame_global.h"
#include "Interfaces.h"
#include <QObject>

class GuiLife;


class BEELIFEGAMESHARED_EXPORT LifeGame : public QObject, public GameInterface
{
  Q_OBJECT
  Q_INTERFACES( GameInterface )
#if QT_VERSION >= 0x050000
  Q_PLUGIN_METADATA(IID "beebeep.plugin.GameInterface/2.0")
#endif
public:
  LifeGame();
  ~LifeGame();
  
  QString name() const;
  QString version() const;
  QString help() const;
  QString author() const;
  QIcon icon() const;
  QString iconFileName() const;
  int priority() const;
  QString coreVersion() const;

  QWidget* mainWindow() const;
  void pause();
  bool isPaused() const;

  void setSettings( QStringList );
  QStringList settings() const;


private:
  GuiLife* mp_life;
	
};

#endif // BEE_LIFEGAME_H
