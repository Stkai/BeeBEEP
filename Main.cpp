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

#define LOGFILE_ENABLED


#include <QApplication>
#ifdef Q_OS_SYMBIAN
#include "sym_iap_util.h"
#endif
#include "GuiMain.h"
#if defined( LOGFILE_ENABLED )
#include "Log.h"
#endif
#include "Protocol.h"
#include "Settings.h"



bool SetTranslator( QTranslator* translator, QString new_locale )
{
  if( !new_locale.endsWith( ".qm" ) )
    new_locale.prepend( ":/locale/beebeep_" );
  if( !translator->load( new_locale ) )
    return false;
  qApp->installTranslator( translator );
  return true;
}




int main( int argc, char *argv[] )
{
#ifdef Q_OS_SYMBIAN
  qt_SetDefaultIap();
#endif
  QApplication app( argc, argv );
  Q_INIT_RESOURCE( beebeep );

  /* Load Settings */
  Settings::instance().load( true );

#if defined( LOGFILE_ENABLED )
  /* Starting Logs */
  Log::boot( Settings::instance().logPath() );
  qInstallMsgHandler( Log::MessageHandler );
#endif

  /* Apply system language */
  QTranslator translator;
  SetTranslator( &translator, Settings::instance().language() );

  /* Init Protocol */
  (void)Protocol::instance();

  /* Show Main Window */
  GuiMain mw;
#ifdef Q_OS_SYMBIAN
  mw.showMaximized();
#else
  QByteArray ba = Settings::instance().guiGeometry();
  if( !ba.isEmpty() )
    mw.restoreGeometry( Settings::instance().guiGeometry() );
  else
    mw.resize( QSize( 600, 340 ) );
  mw.show();
#endif

  // Starting connection to BeeBEEP Network
  QTimer::singleShot( 500, &mw, SLOT( startBeeBeep() ) );

  /* Event Loop */
  int iRet = app.exec();

  /* CleanUp */
  Protocol::close();
  Settings::instance().save();
  Settings::close();
#if defined( LOGFILE_ENABLED )
  Log::close();
#endif
  /* Exit */
  return iRet;
}
