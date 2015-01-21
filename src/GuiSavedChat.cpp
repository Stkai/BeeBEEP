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

#include "GuiSavedChat.h"
#include "ChatManager.h"


GuiSavedChat::GuiSavedChat( QWidget* parent )
 : QWidget( parent )
{
  setupUi( this );
  setObjectName( "GuiSavedChat" );
}

void GuiSavedChat::showSavedChat( const QString& chat_name )
{
  QString html_text = QString( "%1: <b>%2</b> <br />" ).arg( tr( "Saved chat" ) ).arg( chat_name );
  if( !ChatManager::instance().chatHasSavedText( chat_name ) )
    html_text += QString( "<br />*** %1 ***<br />" ).arg( tr( "Empty" ) );
  else
    html_text += QString( "<br />%1<br /><br /><br />" ).arg( ChatManager::instance().chatSavedText( chat_name ) );

  mp_teSavedChat->setText( html_text );

  QScrollBar *bar = mp_teSavedChat->verticalScrollBar();
  bar->setValue( bar->maximum() );
}
