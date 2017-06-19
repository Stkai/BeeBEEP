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

#include "BeeUtils.h"
#include "GuiShareDesktop.h"
#include "ImageOptimizer.h"
#include "User.h"


GuiShareDesktop::GuiShareDesktop( QWidget *parent )
 : QMainWindow( parent ), m_userId( ID_INVALID ), m_lastImage()
{
  setObjectName( "GuiShareDesktop" );
  setWindowIcon( QIcon( ":/images/beebeep.png" ) );

  mp_scrollArea = new QScrollArea( this );
  mp_scrollArea->setBackgroundRole( QPalette::Dark );
  mp_scrollArea->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding );
  mp_scrollArea->setWidgetResizable( false );
  mp_lView = new QLabel( this );
  mp_lView->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding );
  setImageSize( QSize( 400, 300 ) );
  mp_scrollArea->setWidget( mp_lView );
  setCentralWidget( mp_scrollArea );
  m_lastUpdate = QDateTime::currentDateTime();
  m_toDelete = false;
}

void GuiShareDesktop::setUser( const User& u )
{
  m_userId = u.id();
  onUserChanged( u );
}

void GuiShareDesktop::setImageSize( const QSize& pix_size )
{
  mp_lView->setMinimumSize( pix_size );
}

void GuiShareDesktop::updateImage( const QImage& img )
{
  m_lastUpdate = QDateTime::currentDateTime();
  m_lastImage = ImageOptimizer::instance().mergeImage( m_lastImage, img );
  mp_lView->setPixmap( QPixmap::fromImage( m_lastImage ) );
  mp_lView->setToolTip( QString( "%1 %2" ).arg( tr( "last update" ) ).arg( Bee::dateTimeToString( m_lastUpdate ) ) );
}

void GuiShareDesktop::onUserChanged( const User& u )
{
  QString s_title = QString( "%1 - %2" ).arg( u.name(), tr( "Shared desktop" ) );
  setWindowTitle( s_title );

  if( !u.isStatusConnected() )
    close();
}

void GuiShareDesktop::closeEvent( QCloseEvent* e )
{
  QWidget::closeEvent( e );
  emit shareDesktopClosed( m_userId );
  e->accept();
}

void GuiShareDesktop::onTickEvent( int )
{
  if( m_toDelete )
    return;

  if( m_lastUpdate.secsTo( QDateTime::currentDateTime() ) > 15 )
  {
    m_toDelete = true;
    emit shareDesktopDeleteRequest( m_userId );
  }
}
