//////////////////////////////////////////////////////////////////////
//
// BeeBEEP Copyright (C) 2010-2019 Marco Mastroddi
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

#include "BeeUtils.h"
#include "GuiShareDesktop.h"
#include "ImageOptimizer.h"
#include "Settings.h"
#include "User.h"


GuiShareDesktop::GuiShareDesktop( QWidget *parent )
 : QMainWindow( parent ), m_userId( ID_INVALID ), m_lastImage()
{
  setupUi( this );
  setObjectName( "GuiShareDesktop" );
  setWindowIcon( QIcon( ":/images/beebeep.png" ) );

  m_lastUpdate = QDateTime::currentDateTime();
  m_toDelete = false;
}

void GuiShareDesktop::setUser( const User& u )
{
  m_userId = u.id();
  onUserChanged( u );
}

void GuiShareDesktop::updateImage( const QImage& img, const QString& image_type, QRgb diff_color )
{
  m_lastUpdate = QDateTime::currentDateTime();
  if( ImageOptimizer::instance().imageTypeHasTransparentColor( image_type ) )
    m_lastImage = ImageOptimizer::instance().mergeImage( m_lastImage, img, diff_color );
  else
    m_lastImage = img;

  QPixmap pix;
  if( Settings::instance().shareDesktopFitToScreen() )
  {
    QRect available_geometry = geometry();
    QImage fit_img;
    if( img.width() > available_geometry.width() )
      fit_img = m_lastImage.scaledToWidth( available_geometry.width() );
    else
      fit_img = m_lastImage;

    if( fit_img.height() > available_geometry.height() )
      fit_img = fit_img.scaledToHeight( available_geometry.height() );

    pix = QPixmap::fromImage( fit_img );
  }
  else
    pix = QPixmap::fromImage( m_lastImage );

  mp_lView->setPixmap( pix );
  statusBar()->showMessage( QString( "%1: %2 %3 (%4) [%5x%6]" ).arg( tr( "Shared desktop" ) ).arg( tr( "last update" ) )
                            .arg( Bee::dateTimeToString( m_lastUpdate ) ).arg( m_lastUpdate.toString( "ss::zzz" ) )
                            .arg( pix.width() ).arg( pix.height() ) );
  pix = QPixmap();
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
