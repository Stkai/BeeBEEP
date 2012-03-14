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

#include "GuiShareLocal.h"
#include "Settings.h"


GuiShareLocal::GuiShareLocal( QWidget *parent )
  : QWidget(parent)
{
  setupUi( this );

  connect( mp_pbAdd, SIGNAL( clicked() ), this, SLOT( addPath() ) );
  connect( mp_pbRemove, SIGNAL( clicked() ), this, SLOT( removePath() ) );
}

void GuiShareLocal::loadSettings()
{
  mp_twMyShares->clear();
  foreach( QString share_path, Settings::instance().localShare() )
  {
    new QTreeWidgetItem( mp_twMyShares, share_path );
  }

  updateShareList();
}

void GuiShareLocal::addPath()
{

}

void GuiShareLocal::removePath()
{

}

void GuiShareLocal::updateShareList()
{
  mp_twLocalShares->clear();
}
