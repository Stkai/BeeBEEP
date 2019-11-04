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


#include "AudioManager.h"
#include "BeeApplication.h"
#include "ChatManager.h"
#include "ColorManager.h"
#include "Core.h"
#include "EmoticonManager.h"
#include "FileShare.h"
#include "GuiConfig.h"
#include "GuiIconProvider.h"
#include "GuiMain.h"
#include "HistoryManager.h"
#include "Hive.h"
#include "IconManager.h"
#ifdef BEEBEEP_USE_SHAREDESKTOP
  #include "ImageOptimizer.h"
#endif
#include "Log.h"
#include "MessageManager.h"
#include "NetworkManager.h"
#include "PluginManager.h"
#include "UserManager.h"
#include "Protocol.h"
#include "Random.h"
#include "Settings.h"
#include "ShortcutManager.h"
#ifdef BEEBEEP_USE_HUNSPELL
  #include "SpellChecker.h"
#endif


bool SetTranslator( QTranslator* translator, QString language_folder, QString lang )
{
  if( lang.isEmpty() )
  {
    qDebug() << "Language option is empty and default language is installed";
    return false;
  }

  QString language_file_path = Settings::instance().languageFilePath( language_folder, lang );
  if( !translator->load( language_file_path ) )
  {
    qDebug() << qPrintable( language_file_path ) << "file not found. Language" << lang << "will not be installed";
    return false;
  }
  else
    qDebug() << "Language file" << qPrintable( language_file_path ) << "found and" << lang << "installed";
  qApp->installTranslator( translator );
  return true;
}


int main( int argc, char *argv[] )
{
#if QT_VERSION >= 0x050600
  // Windows with 4k monitors, icons are too big... linux is about to test... MacOSX is ok
  #ifdef Q_OS_MAC
    QCoreApplication::setAttribute( Qt::AA_EnableHighDpiScaling, true );
  #endif
#endif
  BeeApplication bee_app( argc, argv );
  (void)Settings::instance();
  bee_app.setApplicationName( Settings::instance().programName() );
  bee_app.setOrganizationName( Settings::instance().organizationName() );
  bee_app.setOrganizationDomain( Settings::instance().organizationDomain() );
  bee_app.setApplicationVersion( Settings::instance().version( false, false, false ) );

  if( bee_app.otherInstanceExists() )
  {
    qDebug() << Settings::instance().programName() << "closed";
    Settings::close();
    return 0;
  }

  /* Enable internal logs */
  Log::installMessageHandler();
  // Next line removes QSslSocket: cannot resolve SSL_* warnings
#if QT_VERSION >= 0x050000
  QLoggingCategory::setFilterRules( QStringLiteral( "qt.network.ssl.warning=false" ) );
#endif
  /* Randomize */
  Random::init();

  /* Load Settings */
  qDebug() << "Starting BeeBEEP" << qPrintable( Settings::instance().version( true, false, true ) )
           << "for" << qPrintable( Settings::instance().operatingSystem( true ) )
           << "and Qt" << QT_VERSION_STR;
#if QT_VERSION >= 0x050600
  if( bee_app.testAttribute( Qt::AA_EnableHighDpiScaling ) )
    qDebug( "Icons: high DPI scaling enabled" );
#endif
  Settings::instance().setDefaultFolders();
  Settings::instance().loadRcFile();
  Settings::instance().setDataFolder();
  qDebug() << "Settings path:" << qPrintable( Settings::instance().currentSettingsFilePath() );
  Settings::instance().createLocalUser( "" );
  Settings::instance().load();
  Settings::instance().createApplicationUuid();
  Log::instance().setMaxLogLines( Settings::instance().maxLogLines() );
  qDebug() << "Log has set max lines to:" << Settings::instance().maxLogLines();
#ifdef BEEBEEP_DEBUG
  QFont chat_font = Settings::instance().chatFont();
  qDebug() << "Font selected for chat:" << chat_font.toString();
  qDebug() << "Font pixel size:" << chat_font.pixelSize();
  qDebug() << "Font point size:" << chat_font.pointSize();
  qDebug() << "Font height:" << static_cast<int>(QFontMetrics( chat_font).height() );
  qDebug() << "Emoticon size in chat:" << Settings::instance().emoticonSizeInChat();
  qDebug() << "Emoticon size in edit:" << Settings::instance().emoticonSizeInEdit();
#endif

  if( !Settings::instance().allowMultipleInstances() )
    bee_app.preventMultipleInstances();

  /* Starting File Logs */
  if( Settings::instance().logToFile() )
    Log::instance().bootFileStream( Settings::instance().logFilePath() );

  /* Apply system language */
  QTranslator translator;
  if( !SetTranslator( &translator, Settings::instance().languagePath(), Settings::instance().language() ) )
  {
    qDebug() << "Looking for language in default folders...";
    SetTranslator( &translator, Settings::instance().defaultLanguageFolderPath(), Settings::instance().language() );
  }

  /* Init Network Manager */
  (void)NetworkManager::instance();
  NetworkManager::instance().searchLocalHostAddress();
  Settings::instance().setLocalUserHost( NetworkManager::instance().localHostAddress(), Settings::instance().localUser().networkAddress().hostPort() );

  /* Init Hive */
  (void)Hive::instance();

  /* Init Color Manager */
  (void)ColorManager::instance();

  /* Init Protocol */
  (void)Protocol::instance();

  /* Init User Manager */
  (void)UserManager::instance();

  /* Init Message Manager */
  (void)MessageManager::instance();

  /* Init Chat Manager */
  (void)ChatManager::instance();

  /* Init Icon Manager */
  IconManager::instance().setSourcePath( Settings::instance().iconSourcePath() );

  /* Init Emoticon Manager */
  EmoticonManager::instance().loadRecentEmoticons( Settings::instance().recentEmoticons(), Settings::instance().emoticonInRecentMenu() );

  /* Init History Manager */
  (void)HistoryManager::instance();

  /* Init File Sharing */
  (void)FileShare::instance();

  /* Init Icon Provider */
  (void)GuiIconProvider::instance();

#ifdef BEEBEEP_USE_SHAREDESKTOP
  (void)ImageOptimizer::instance();
#endif

  /* Init Audio Manager */
  if( AudioManager::instance().isAudioDeviceAvailable() )
  {
    if( Settings::instance().beepOnNewMessageArrived() )
      AudioManager::instance().loadBeepEffect();
  }
  else
    qWarning() << "Sound manager seems to be not available for your system";

  /* Init Shortcut Manager */
  (void)ShortcutManager::instance();
  if( !Settings::instance().shortcuts().isEmpty() )
    ShortcutManager::instance().loadFromStringList( Settings::instance().shortcuts() );

#ifdef BEEBEEP_USE_HUNSPELL
  /* Init SpellChecker */
  (void)SpellChecker::instance();
  if( !Settings::instance().dictionaryPath().isEmpty() )
    SpellChecker::instance().setDictionary( Settings::instance().dictionaryPath() );
#endif

  /* Init Plugins */
  PluginManager::instance().loadPlugins( Settings::instance().pluginPath() );

  /* Init BeeApp */
  bee_app.init();
  bee_app.setAttribute( Qt::AA_DontUseNativeMenuBar );
  if( Settings::instance().autoUserAway() )
    bee_app.setIdleTimeout( Settings::instance().userAwayTimeout() );

  Core bee_core;
  bee_core.loadUsersAndGroups();

  if( !QSystemTrayIcon::isSystemTrayAvailable() )
    qWarning() << "System tray icon is not available in this OS";

  /* Init Main Window */
  GuiMain mw;
  QObject::connect( &bee_app, SIGNAL( enteringInIdle() ), &mw, SLOT( setInIdle() ) );
  QObject::connect( &bee_app, SIGNAL( exitingFromIdle() ), &mw, SLOT( exitFromIdle() ) );
  QObject::connect( &bee_app, SIGNAL( showUp() ), &mw, SLOT( raiseOnTop() ) );
  QObject::connect( &bee_app, SIGNAL( tickEvent( int ) ), &mw, SLOT( onTickEvent( int ) ) );
  QObject::connect( &bee_app, SIGNAL( commitDataRequest( QSessionManager& ) ), &mw, SLOT( saveSession( QSessionManager& ) ), Qt::DirectConnection );
  QObject::connect( &bee_app, SIGNAL( shutdownRequest() ), &mw, SLOT( forceShutdown() ), Qt::DirectConnection );
  QObject::connect( &bee_app, SIGNAL( sleepRequest() ), &mw, SLOT( onSleepRequest() ) );
  QObject::connect( &bee_app, SIGNAL( wakeUpRequest() ), &mw, SLOT( onWakeUpRequest() ) );
  QObject::connect( &bee_app, SIGNAL( focusChanged( QWidget*, QWidget* ) ), &mw, SLOT( onApplicationFocusChanged( QWidget*, QWidget* ) ) );
  mw.loadStyle();
  QMetaObject::invokeMethod( &mw, "checkWindowFlagsAndShow", Qt::QueuedConnection );
  qDebug() << "Loading saved session";
  mw.loadSession();
  bee_app.setMainWidget( &mw );
  /* Event Loop */
  qDebug() << "Enter in the main event loop";
  int iRet = bee_app.exec();
  qDebug() << "Check and process the remaining events in loop";

  /* Check Icon Provider */
  qDebug() << "IconProvider has load in cache" << GuiIconProvider::instance().cacheSize() << "icons";

  /* Save final session */
  Settings::instance().setRecentEmoticons( EmoticonManager::instance().saveRencentEmoticons() );
  Settings::instance().loadRcFile();
  Settings::instance().save();

  /* CleanUp */
  bee_app.cleanUp();
  Settings::instance().clearTemporaryFiles();
  GuiIconProvider::close();
  FileShare::close();
  HistoryManager::close();
  ChatManager::close();
  MessageManager::close();
  UserManager::close();
  Protocol::close();
  PluginManager::close();
  Hive::close();
  NetworkManager::close();
  EmoticonManager::close();
  IconManager::close();
  ColorManager::close();
#ifdef BEEBEEP_USE_SHAREDESKTOP
  ImageOptimizer::close();
#endif
  AudioManager::close();
  ShortcutManager::close();
#ifdef BEEBEEP_USE_HUNSPELL
  SpellChecker::close();
#endif
  qDebug() << "Exit with code:" << iRet;
  Log::instance().closeFileStream();
  Log::instance().close();
  Settings::close();

  /* Exit */
  return iRet;
}
