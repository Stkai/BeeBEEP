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

#include "GuiSelectItems.h"
#include "Settings.h"


GuiSelectItems::GuiSelectItems( QWidget *parent )
 : QDialog( parent ), m_textToSearch( "" )
{
  setupUi( this );
  setObjectName( "GuiSelectItems" );
  setModal( true );

  mp_tw->setHeaderHidden( true );
  mp_tw->setColumnCount( 1 );
  mp_tw->setRootIsDecorated( false );
  mp_tw->setSortingEnabled( true );
  mp_tw->setSelectionMode( QTreeWidget::MultiSelection );

  connect( mp_leSearch, SIGNAL( textChanged( const QString& ) ), this, SLOT( searchText( const QString& ) ) );
  connect( mp_pbOk, SIGNAL( clicked() ), this, SLOT( onOkClicked() ) );
  connect( mp_pbCancel, SIGNAL( clicked() ), this, SLOT( onCancelClicked() ) );
  connect( mp_pbClear, SIGNAL( clicked() ), this, SLOT( onClearClicked() ) );
  connect( mp_pbClearFilter, SIGNAL( clicked() ), this, SLOT( onClearFilterClicked() ) );
}

void GuiSelectItems::onOkClicked()
{
  bool item_hidden_is_selected = false;
  QTreeWidgetItemIterator it( mp_tw );
  while( *it )
  {
    if( (*it)->isSelected() && (*it)->isHidden() )
    {
      item_hidden_is_selected = true;
      break;
    }
    ++it;
  }

  if( item_hidden_is_selected )
  {
    int q_answer = QMessageBox::question( this, Settings::instance().programName(),
                     tr( "One or more elements of the list have been selected but are hidden by the search text. Do you still want to add them?" ),
                     tr( "Yes" ), tr( "No" ), tr( "Cancel" ), 0, 2 );
    if( q_answer == 0 )
    {
      QTreeWidgetItemIterator its( mp_tw );
      while( *its )
      {
        if( (*its)->isSelected() && (*its)->isHidden() )
          (*its)->setHidden( false );
        ++its;
      }
    }
    else if( q_answer == 1 )
    {
      QTreeWidgetItemIterator itd( mp_tw );
      while( *itd )
      {
        if( (*itd)->isSelected() && (*itd)->isHidden() )
          (*itd)->setSelected( false );
        ++itd;
      }
    }
    else
      return;
  }
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

void GuiSelectItems::onClearFilterClicked()
{
  mp_leSearch->setText( "" );
}

void GuiSelectItems::resetList()
{
  if( mp_tw->topLevelItemCount() > 0 )
    mp_tw->clear();
}

void GuiSelectItems::updateList()
{
  QTreeWidgetItemIterator it( mp_tw );
  int tw_cols = mp_tw->columnCount();
  while( *it )
  {
    bool txt_found = m_textToSearch.isEmpty();
    if( !txt_found )
    {
      for( int i = 0; i < tw_cols; i++ )
      {
        txt_found = (*it)->text( i ).contains( m_textToSearch, Qt::CaseInsensitive );
        if( txt_found )
            break;
      }
    }
    (*it)->setHidden( !txt_found );
    ++it;
  }
}

void GuiSelectItems::sortList()
{
  mp_tw->sortItems( 0, Qt::AscendingOrder );
}

void GuiSelectItems::searchText( const QString& txt )
{
  QString text_to_search = txt.trimmed().toLower();
  if( m_textToSearch == text_to_search )
    return;

  m_textToSearch = text_to_search;
  updateList();
}


