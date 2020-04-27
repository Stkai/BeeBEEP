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

#ifndef BEEBEEP_GUIAUDIOLEVEL_H
#define BEEBEEP_GUIAUDIOLEVEL_H

#include "Config.h"

class GuiAudioLevel : public QWidget
{
  Q_OBJECT
public:
  explicit GuiAudioLevel( QWidget *parent = 0 );

  void setLevel( qreal );

protected:
  void paintEvent( QPaintEvent *event ) override;

private:
  qreal m_level;

};


#endif // BEEBEEP_GUIAUDIOLEVEL_H
