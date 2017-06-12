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

#include "GuiShareDesktop.h"
#include "User.h"

GuiShareDesktop::GuiShareDesktop( QWidget *parent )
 : QMainWindow( parent ), m_userId( ID_INVALID ), m_chatId( ID_INVALID )
{
  setObjectName( "GuiShareDesktop" );
  setWindowIcon( QIcon( ":/images/beebeep.png" ) );
  mp_scrollArea = new QScrollArea( this );
  mp_scrollArea->setBackgroundRole( QPalette::Dark );
  setCentralWidget( mp_scrollArea );
  mp_lView = new QLabel( this );
  mp_scrollArea->setWidget( mp_lView );
  mp_scrollArea->setWidgetResizable( false );
  m_lastUpdate = QDateTime::currentDateTime();
}

void GuiShareDesktop::setUser( const User& u )
{
  m_userId = u.id();
  onUserChanged( u );
}

void GuiShareDesktop::updatePixmap( const QPixmap& pix )
{
  mp_lView->setPixmap( pix );
  m_lastUpdate = QDateTime::currentDateTime();
}

void GuiShareDesktop::onUserChanged( const User& u )
{
  QString s_title = QString( "%1 - %2" ).arg( u.name(), tr( "Desktop" ) );
  setWindowTitle( s_title );

  if( !u.isStatusConnected() )
    close();
}

void GuiShareDesktop::closeEvent( QCloseEvent* e )
{
  QWidget::closeEvent( e );
  emit shareDesktopClosed( m_userId, m_chatId );
  e->accept();
}
