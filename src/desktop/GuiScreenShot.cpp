//////////////////////////////////////////////////////////////////////
//
// BeeBEEP Copyright (C) 2010-2021 Marco Mastroddi
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

#include "GuiScreenShot.h"
#include "BeeUtils.h"
#include "FileDialog.h"
#include "IconManager.h"
#include "Settings.h"


GuiScreenShot::GuiScreenShot( QWidget* parent )
  : QMainWindow( parent )
{
  setObjectName( "GuiScreenShot" );
  setWindowTitle( QString( "%1 - %2" ).arg( tr( "Make a Screenshot" ), Settings::instance().programName() ) );
  setWindowIcon( IconManager::instance().icon( "screenshot.png" ) );

  mp_labelScreenShot = new QLabel( this );
  mp_labelScreenShot->setScaledContents( true );
  mp_labelScreenShot->setAlignment( Qt::AlignCenter );
  mp_labelScreenShot->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding );
  setCentralWidget( mp_labelScreenShot );

  mp_barScreenShot = new QToolBar( tr( "Show the bar of screenshot plugin" ), this );
  addToolBar( Qt::BottomToolBarArea, mp_barScreenShot );
  mp_barScreenShot->setObjectName( "GuiScreenShotToolBar" );
  mp_barScreenShot->setIconSize( Settings::instance().mainBarIconSize() );
  mp_barScreenShot->setAllowedAreas( Qt::AllToolBarAreas );
  mp_barScreenShot->setFloatable( false );
  setupToolBar( mp_barScreenShot );
  mp_barScreenShot->toggleViewAction()->setVisible( false );
}

void GuiScreenShot::setupToolBar( QToolBar* bar )
{
  QLabel* mp_labelDelay = new QLabel( this );
  mp_labelDelay->setObjectName( "mp_labelDelay" );
  mp_labelDelay->setAlignment( Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
  mp_labelDelay->setText( QString( "  " ) + tr( "Delay" ) + QString( "  " ) );
  mp_labelDelay->setToolTip( tr( "Delay screenshot for selected seconds" ) );
  bar->addWidget( mp_labelDelay );

  mp_sbDelay = new QSpinBox( this );
  mp_sbDelay->setObjectName( "mp_sbDelay" );
  mp_sbDelay->setMinimum( 0 );
  mp_sbDelay->setMaximum( 15 );
  mp_sbDelay->setValue( 2 );
  mp_sbDelay->setSuffix( QString( " " ) + tr( "s" ) + QString( " " ) );
  bar->addWidget( mp_sbDelay );

  bar->addSeparator();

  mp_cbHide = new QCheckBox( this );
  mp_cbHide->setObjectName( "mp_cbHide" );
  mp_cbHide->setChecked( true );
  mp_cbHide->setText( tr( "Hide this window" ) + QString( "   " ) );
  mp_cbHide->setToolTip( tr( "Hide this window before capture screenshot" ) );
  bar->addWidget( mp_cbHide );

  mp_cbCursor = new QCheckBox( this );
  mp_cbCursor->setObjectName( "mp_cbCursor" );
#if QT_VERSION >= 0x050000
  mp_cbCursor->setChecked( true );
  mp_cbCursor->setToolTip( tr( "Also grab the cursor in the screenshot" ) );
#else
  mp_cbCursor->setChecked( false );
  mp_cbCursor->setEnabled( false );
#endif
  mp_cbCursor->setText( tr( "Grab cursor" ) + QString( "   " ) );

  bar->addWidget( mp_cbCursor );

#if QT_VERSION < 0x050000
  mp_cbRetina = new QCheckBox( this );
  mp_cbRetina->setObjectName( "mp_cbRetina" );
  #ifdef Q_OS_MAC
    mp_cbRetina->setChecked( true );
  #else
    mp_cbRetina->setChecked( false );
  #endif
  mp_cbRetina->setText( tr( "Enable high dpi" ) + QString( "   " ) );
  mp_cbRetina->setToolTip( tr( "Enable high dpi support to manage, for example, Apple Retina display" ) );
  bar->addWidget( mp_cbRetina );
#endif

  bar->addSeparator();

  mp_actShot = bar->addAction( IconManager::instance().icon( "screenshot.png" ), tr( "Capture screen" ), this, SLOT( doScreenShot() ) );
  mp_actSend = bar->addAction( IconManager::instance().icon( "send.png" ), tr( "Send screenshot to a user" ), this, SLOT( doSend() ) );
  mp_actSave = bar->addAction( IconManager::instance().icon( "save-as.png" ), tr( "Save screenshot" ), this, SLOT( doSave() ) );
  mp_actDelete = bar->addAction( IconManager::instance().icon( "delete.png" ), tr( "Delete screenshot" ), this, SLOT( doDelete() ) );

  updateScreenShot();
}

void GuiScreenShot::resizeEvent( QResizeEvent* )
{
  if( !m_screenShot.isNull() )
  {
    QSize scaled_size = m_screenShot.size();
    scaled_size.scale( mp_labelScreenShot->size(), Qt::KeepAspectRatio );
    if( scaled_size != mp_labelScreenShot->pixmap()->size() )
      updateScreenShot();
  }
}

void GuiScreenShot::showUp()
{
  Bee::showUp( this );
}

void GuiScreenShot::updateScreenShot()
{
  if( m_screenShot.isNull() )
  {
    mp_labelScreenShot->setText( tr( "No screenshot available" ) );
    mp_actSave->setEnabled( false );
    mp_actSend->setEnabled( false );
    mp_actDelete->setEnabled( false );
  }
  else
  {
    mp_labelScreenShot->setPixmap( m_screenShot.scaled( mp_labelScreenShot->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation ) );
    mp_actSave->setEnabled( true );
    mp_actSend->setEnabled( true );
    mp_actDelete->setEnabled( true );
  }
}

void GuiScreenShot::doScreenShot()
{
  if( mp_cbHide->isChecked() )
    hide();

  mp_actShot->setDisabled( true );

  int delay_time = mp_sbDelay->value();
  if( delay_time < 1 )
    delay_time = 300;
  else
    delay_time *= 1000;

  QTimer::singleShot( delay_time, this, SLOT( captureScreen() ) );
}

void GuiScreenShot::captureScreen()
{
  m_screenShot = QPixmap(); // clear image for low memory situations on embedded devices.

  double device_pixel_ratio;

#if QT_VERSION >= 0x050000
  device_pixel_ratio = qApp->devicePixelRatio();
#else
  if( mp_cbRetina->isChecked() )
    device_pixel_ratio = 2.0;
  else
    device_pixel_ratio = 1.0;
#endif

#if QT_VERSION >= 0x050000
  QScreen* primary_screen = QApplication::primaryScreen();
  if( primary_screen )
  {
    m_screenShot = primary_screen->grabWindow( 0 );
    if( mp_cbCursor->isChecked() )
    {
      QPixmap cursor_pix = cursor().pixmap();
      if( cursor_pix.isNull() )
        cursor_pix = IconManager::instance().icon( "cursor.png" ).pixmap( 32, 32 );
      QPainter p( &m_screenShot );
      p.drawPixmap( cursor().pos( primary_screen ), cursor_pix );
    }
  }
  m_screenShot.setDevicePixelRatio( device_pixel_ratio );

#else
  m_screenShot = QPixmap::grabWindow( QApplication::desktop()->winId(), 0, 0,
                                      QApplication::desktop()->width() * device_pixel_ratio,
                                      QApplication::desktop()->height() * device_pixel_ratio );
#endif

#ifdef BEEBEEP_DEBUG
  qDebug() << "Screenshot width" << m_screenShot.width() << "height" << m_screenShot.height() << "and ratio" << device_pixel_ratio;
#endif

  updateScreenShot();

  if( mp_cbHide->isChecked() )
    showUp();

  mp_actShot->setEnabled( true );
}

void GuiScreenShot::doSave()
{
  QString screenshot_format = "png";
  QString screenshot_initial_path = Settings::instance().lastDirectorySelected() +
                                    tr( "/beesshot-%1." ).arg( Bee::dateTimeStringSuffix( QDateTime::currentDateTime() ) )
                                    + screenshot_format;

  QString file_path = FileDialog::getSaveFileName( this, tr( "Save As" ),
                                screenshot_initial_path,
                                tr( "%1 Files (*.%2)" )
                                .arg( screenshot_format.toUpper() )
                                .arg( screenshot_format ) );


  if( file_path.isNull() || file_path.isEmpty() )
    return;

  m_screenShot.save( file_path, screenshot_format.toLatin1() );
}

void GuiScreenShot::doSend()
{
  QString screenshot_format = "png";
  QString screenshot_initial_path = Settings::instance().cacheFolder() +
                                    tr( "/beesshottmp-%1." ).arg( Bee::dateTimeStringSuffix( QDateTime::currentDateTime() ) )
                                    + screenshot_format;
  QString file_path = Bee::uniqueFilePath( screenshot_initial_path, false );

  if( !m_screenShot.save( file_path, screenshot_format.toLatin1() ) )
  {
    QMessageBox::warning( this, Settings::instance().programName(),
      tr( "Unable to save temporary file: %1" ).arg( file_path ) );
    return;
  }

  Settings::instance().addTemporaryFilePath( file_path );
  emit screenShotToSend( file_path );
}

void GuiScreenShot::doDelete()
{
  m_screenShot = QPixmap();
  updateScreenShot();
}

