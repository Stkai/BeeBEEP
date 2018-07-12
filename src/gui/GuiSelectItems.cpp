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

#include "GuiSelectItems.h"


GuiSelectItems::GuiSelectItems( QWidget *parent )
 : QDialog( parent )
{
  setupUi( this );
  setObjectName( "GuiSelectItems" );

  mp_tw->setHeaderHidden( true );
  mp_tw->setColumnCount( 1 );
  mp_tw->setRootIsDecorated( false );
  mp_tw->setSortingEnabled( true );
  mp_tw->setSelectionMode( QTreeWidget::MultiSelection );

  connect( mp_pbOk, SIGNAL( clicked() ), this, SLOT( onOkClicked() ) );
  connect( mp_pbCancel, SIGNAL( clicked() ), this, SLOT( onCancelClicked() ) );
  connect( mp_pbClear, SIGNAL( clicked() ), this, SLOT( onClearClicked() ) );

}

void GuiSelectItems::onOkClicked()
{
  accept();
}

void GuiSelectItems::onCancelClicked()
{
  reject();
}

void GuiSelectItems::onClearClicked()
{
  mp_tw->clearSelection();
}
