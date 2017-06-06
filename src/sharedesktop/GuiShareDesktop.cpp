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
 : QWidget( parent ), m_ownerId( ID_INVALID ), m_lastImage()
{
  setObjectName( "GuiShareDesktop" );
  setupUi( this );
  setWindowIcon( QIcon( ":/images/beebeep.png" ) );
}

void GuiShareDesktop::setOwner( const User& u )
{
  m_ownerId = u.id();
  onUserChanged( u );
}

void GuiShareDesktop::updatePixmap( const QPixmap& pix )
{
  mp_lView->setPixmap( pix.scaled( width()-40, height()-40, Qt::IgnoreAspectRatio ) );
  m_lastImage = pix;
}

void GuiShareDesktop::onUserChanged( const User& u )
{
  QString s_title = QString( "%1 - %2" ).arg( u.name(), tr( "Desktop" ) );
  setWindowTitle( s_title );
}

void GuiShareDesktop::closeEvent( QCloseEvent* e )
{
  QWidget::closeEvent( e );
  emit shareDesktopClosed( m_ownerId );
  e->accept();
}
