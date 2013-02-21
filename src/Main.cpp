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

#undef QXMPP_LOGFILE

#include <QApplication>
#ifdef Q_OS_SYMBIAN
#include "sym_iap_util.h"
#endif
#include "ChatManager.h"
#include "ColorManager.h"
#include "FileShare.h"
#include "GuiMain.h"
#include "Log.h"
#include "PluginManager.h"
#include "UserManager.h"
#include "Protocol.h"
#include "Random.h"
#include "Settings.h"
#ifdef USE_QXMPP
#ifdef QXMPP_LOGFILE
  #include "QXmppLogger.h"
#endif
#endif


bool SetTranslator( QTranslator* translator, const QString& prog_name, const QString& locale_folder, const QString& lang )
{
  QString new_locale = QString( "%1/%2_%3.qm").arg( locale_folder, prog_name.toLower(), lang );
  if( !translator->load( new_locale ) )
  {
    qDebug() << new_locale << "file not found";
    return false;
  }
  else
    qDebug() << "Language file" << new_locale << "found. Installed";
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

  /* Randomize */
  Random::init();

  /* Load Settings */
  Settings::instance().load();

#ifdef BEEBEEP_RELEASE
  /* Starting Logs */
  Log::boot();
  qInstallMsgHandler( Log::MessageHandler );
#endif

#ifdef USE_QXMPP
  qDebug() << "Running QXmpp Version";
#endif

  /* Apply system language */
  QTranslator translator;
  SetTranslator( &translator, Settings::instance().programName(), Settings::instance().localePath(), Settings::instance().language() );

  /* Init Protocol */
  (void)Protocol::instance();

  /* Init Color Manager */
  (void)ColorManager::instance();

  /* Init User Manager */
  (void)UserManager::instance();

  /* Init Chat Manager */
  (void)ChatManager::instance();

  /* Init File Sharing */
  (void)FileShare::instance();

  /* Init Plugins */
  PluginManager::instance().loadPlugins();

  /* Xmpp Logger */
#ifdef USE_QXMPP
#ifdef QXMPP_LOGFILE
  QXmppLogger::getLogger()->setLoggingType( QXmppLogger::FileLogging );
#endif
#endif

  /* Show Main Window */
  GuiMain mw;
#ifdef Q_OS_SYMBIAN
  mw.showMaximized();
#else
  QByteArray ba = Settings::instance().guiGeometry();
  if( !ba.isEmpty() )
  {
    mw.restoreGeometry( Settings::instance().guiGeometry() );
    ba = Settings::instance().guiState();
    if( !ba.isEmpty() )
      mw.restoreState( Settings::instance().guiState() );
  }
  else
    mw.resize( QSize( 640, 420 ) );

  mw.checkWindowFlagsAndShow();
#endif

  // Starting connection to BeeBEEP Network
  QTimer::singleShot( 500, &mw, SLOT( startStopCore() ) );

  /* Event Loop */
  int iRet = app.exec();

  /* CleanUp */
  FileShare::close();
  ChatManager::close();
  UserManager::close();
  ColorManager::close();
  Protocol::close();
  PluginManager::close();
  Settings::instance().save();
  Settings::close();

#ifdef BEEBEEP_RELEASE
  Log::close();
#endif
  /* Exit */
  return iRet;
}
