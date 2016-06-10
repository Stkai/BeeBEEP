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
#ifdef Q_OS_WIN
  #include <windows.h>
#endif


GuiShareDesktop::GuiShareDesktop( QWidget *parent )
 : QMainWindow( parent )
{
  setObjectName( "GuiShareDesktop" );
  setWindowIcon( QIcon( ":/images/beebeep.png" ) );

  mp_lDesktop = new QLabel( this );
  mp_lDesktop->setScaledContents( true );

  setCentralWidget( mp_lDesktop );

}

void GuiShareDesktop::setOwner( const User& u )
{
  m_ownerId = u.id();
  onUserChanged( u );
}

void GuiShareDesktop::updatePixmap( const QPixmap& pix )
{
  mp_lDesktop->setPixmap( pix.scaled( 800, 600 ) );
}

void GuiShareDesktop::onUserChanged( const User& u )
{
  QString s_title = QString( "%1 - %2" ).arg( u.name(), tr( "Desktop" ) );
  setWindowTitle( s_title );
}

void GuiShareDesktop::closeEvent( QCloseEvent* e )
{
  QMainWindow::closeEvent( e );
  emit shareDesktopClosed( m_ownerId );
  e->accept();
}

void GuiShareDesktop::toggleStaysOnTop()
{
    /*
#ifdef Q_OS_WIN
  if( Settings::instance().stayOnTop() )
    SetWindowPos( (HWND)winId(), HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE );
  else
    SetWindowPos( (HWND)winId(), HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE );
#else
  Qt::WindowFlags w_flags = this->windowFlags();
  if( Settings::instance().stayOnTop() )
    w_flags |= Qt::WindowStaysOnTopHint;
  else
    w_flags &= ~Qt::WindowStaysOnTopHint;
  setWindowFlags( w_flags );
#endif
  show();
  */
}
