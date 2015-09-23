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

#include "GuiScreenShot.h"
#include "BeeUtils.h"
#include "Settings.h"


GuiScreenShot::GuiScreenShot( QWidget* parent )
  : QWidget( parent )
{
  setupUi( this );
  setObjectName( "GuiScreenShot" );
  mp_lTitle->setText( QString( "<b>%1</b>" ).arg( tr( "Make a Screenshot" ) ) );
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
  mp_sbDelay->setValue( 0 );
  mp_sbDelay->setSuffix( QString( " " ) + tr( "s" ) + QString( " " ) );
  bar->addWidget( mp_sbDelay );

  bar->addSeparator();

  mp_cbHide = new QCheckBox( this );
  mp_cbHide->setObjectName( "mp_cbHide" );
  mp_cbHide->setChecked(true);
  mp_cbHide->setText( tr( "Hide this window" ) + QString( "   " ) );
  mp_cbHide->setToolTip( tr( "Hide this window before capture screenshot" ) );
  bar->addWidget( mp_cbHide );

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

  bar->addSeparator();

  mp_actShot = bar->addAction( QIcon( ":/images/screenshot.png" ), tr( "Capture" ), this, SLOT( doScreenShot() ) );
  mp_actShot->setStatusTip( tr( "Capture a screenshot of your desktop" ) );
  mp_actSend = bar->addAction( QIcon( ":/images/send.png" ), tr( "Send" ), this, SLOT( doSend() ) );
  mp_actSend->setStatusTip( tr( "Send the captured screenshot to an user" ) );
  mp_actSave = bar->addAction( QIcon( ":/images/save-as.png" ), tr( "Save" ), this, SLOT( doSave() ) );
  mp_actSave->setStatusTip( tr( "Save the captured screenshot as file" ) );
  mp_actDelete = bar->addAction( QIcon( ":/images/delete.png" ), tr( "Delete" ), this, SLOT( doDelete() ) );
  mp_actDelete->setStatusTip( tr( "Delete the captured screenshot" ) );

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
    emit( hideRequest() );

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

#if QT_VERSION >= 0x050000
  QScreen *screen = QGuiApplication::primaryScreen();
  if( screen )
    m_screenShot = screen->grabWindow( 0 );
#else
  if( mp_cbRetina->isChecked() )
    m_screenShot = QPixmap::grabWindow( QApplication::desktop()->winId(), 0, 0, QApplication::desktop()->width() * 2, QApplication::desktop()->height() * 2  );
  else
    m_screenShot = QPixmap::grabWindow( QApplication::desktop()->winId() );
#endif

#ifdef BEEBEEP_DEBUG
  qDebug() << "Screenshot width" << m_screenShot.width() << "height" << m_screenShot.height();
#endif

  updateScreenShot();

  emit( showRequest() );

  mp_actShot->setEnabled( true );
}

void GuiScreenShot::doSave()
{
  QString screenshot_format = "png";
  QString screenshot_initial_path = Settings::instance().lastDirectorySelected() +
                                    tr( "/beesshot-%1." ).arg( Bee::dateTimeStringSuffix( QDateTime::currentDateTime() ) )
                                    + screenshot_format;

  QString file_path = QFileDialog::getSaveFileName( this, tr( "Save As" ),
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
  QString screenshot_initial_path = Settings::instance().lastDirectorySelected() +
                                    tr( "/beesshottmp-%1." ).arg( Bee::dateTimeStringSuffix( QDateTime::currentDateTime() ) )
                                    + screenshot_format;
  QString file_path = Bee::uniqueFilePath( screenshot_initial_path );

  if( !m_screenShot.save( file_path, screenshot_format.toLatin1() ) )
  {
    QMessageBox::warning( this, Settings::instance().programName(),
      tr( "Unable to save temporary file: %1" ).arg( file_path ) );
    return;
  }

  Settings::instance().addTemporaryFilePath( file_path );
  emit( screenShotToSend( file_path ) );
}

void GuiScreenShot::doDelete()
{
  m_screenShot = QPixmap();
  updateScreenShot();
}

