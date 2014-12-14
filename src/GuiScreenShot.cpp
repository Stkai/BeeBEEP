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
// $Id: GuiLog.cpp 251 2014-10-16 15:26:36Z mastroddi $
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

  connect( mp_pbShot, SIGNAL( clicked() ), this, SLOT( doScreenShot() ) );
  connect( mp_pbSave, SIGNAL( clicked() ), this, SLOT( doSave() ) );
  connect( mp_pbSend, SIGNAL( clicked() ), this, SLOT( doSend() ) );

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
    mp_pbSave->setEnabled( false );
    mp_pbSend->setEnabled( false );
  }
  else
  {
    mp_labelScreenShot->setPixmap( m_screenShot.scaled( mp_labelScreenShot->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation ) );
    mp_pbSave->setEnabled( true );
    mp_pbSend->setEnabled( true );
  }
}

void GuiScreenShot::doScreenShot()
{
  if( mp_cbHide->isChecked() )
    emit( hideRequest() );

  mp_pbShot->setDisabled( true );

  int delay_time = mp_sbDelay->value();
  if( delay_time < 1 )
    delay_time = 100;
  else
    delay_time *= 1000;

  QTimer::singleShot( delay_time, this, SLOT( captureScreen() ) );
}

void GuiScreenShot::captureScreen()
{
  m_screenShot = QPixmap(); // clear image for low memory situations on embedded devices.
  m_screenShot = QPixmap::grabWindow( QApplication::desktop()->winId() );
  updateScreenShot();

  emit( showRequest() );

  mp_pbShot->setEnabled( true );
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

  m_screenShot.save( file_path, screenshot_format.toAscii() );
}

void GuiScreenShot::doSend()
{
  QString screenshot_format = "png";
  QString screenshot_initial_path = Settings::instance().lastDirectorySelected() +
                                    tr( "/beesshottmp-%1." ).arg( Bee::dateTimeStringSuffix( QDateTime::currentDateTime() ) )
                                    + screenshot_format;
  QString file_path = Bee::uniqueFilePath( screenshot_initial_path );

  if( !m_screenShot.save( file_path, screenshot_format.toAscii() ) )
  {
    QMessageBox::warning( this, Settings::instance().programName(),
      tr( "Unable to save temporary file: %1" ).arg( file_path ) );
    return;
  }

  Settings::instance().addTemporaryFilePath( file_path );
  emit( screenShotToSend( file_path ) );
}

