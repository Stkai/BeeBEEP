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

bool CheckValidArguments( int argc, char *argv[] )
{
  // Qt security issue: https://www.thezdi.com/blog/2019/4/3/loading-up-a-pair-of-qt-bugs-detailing-cve-2019-1636-and-cve-2019-6739
  for( int i = 0; i < argc; i++ )
  {
    if( QString::fromLocal8Bit( argv[i] ).contains( "platformpluginpath", Qt::CaseInsensitive ) )
    {
      qWarning() << "[WARNING] Argument -platformpluginpath has been disabled for security reasons.";
      qDebug() << "Use the qt.conf file if you want to assign a different path for plugins.";
      return false;
    }
  }
  return true;
}

int main( int argc, char *argv[] )
{
  if( !CheckValidArguments( argc, argv ) )
  {
    qDebug() << "Check your arguments and restart the application.";
    return 1;
  }

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
  qDebug() << "Qt prefix path:" << qPrintable( QDir::toNativeSeparators( QLibraryInfo::location( QLibraryInfo::PrefixPath ) ) );
  qDebug() << "Qt libraries path:" << qPrintable( QDir::toNativeSeparators( QLibraryInfo::location( QLibraryInfo::LibrariesPath ) ) );
  qDebug() << "Qt binaries path:" << qPrintable( QDir::toNativeSeparators( QLibraryInfo::location( QLibraryInfo::BinariesPath ) ) );
  qDebug() << "Qt plugins path:" << qPrintable( QDir::toNativeSeparators( QLibraryInfo::location( QLibraryInfo::PluginsPath ) ) );
  qDebug() << "Qt data path:" << qPrintable( QDir::toNativeSeparators( QLibraryInfo::location( QLibraryInfo::DataPath ) ) );
  qDebug() << "Qt settings path:" << qPrintable( QDir::toNativeSeparators( QLibraryInfo::location( QLibraryInfo::SettingsPath ) ) );
  qDebug() << "Qt paths are shipped with Qt libraries. Edit qt.conf file to change them.";
  #if QT_VERSION >= 0x050600
  if( bee_app.testAttribute( Qt::AA_EnableHighDpiScaling ) )
    qDebug( "Icons: high DPI scaling enabled" );
#endif

  Settings::instance().setDefaultFolders();
  Settings::instance().loadRcFile();
  Settings::instance().createLocalUser( "" );
  Settings::instance().setDataFolder();
  qDebug() << "Settings path:" << qPrintable( Settings::instance().currentSettingsFilePath() );
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
  NetworkManager::instance().searchLocalHostAddress();
  Settings::instance().setLocalUserHost( NetworkManager::instance().localHostAddress(), Settings::instance().localUser().networkAddress().hostPort() );

  /* Init Hive */
  (void)Hive::instance();

  /* Init Color Manager */
  (void)ColorManager::instance();

  /* Init Protocol */
  (void)Protocol::instance();
  qDebug() << "Connection key exchange method selected:" << (Settings::instance().isConnectionKeyExchangeOnlyECDH() ? "ECDH Only" : "auto" );

  /* Init User Manager */
  (void)UserManager::instance();

  /* Init Message Manager */
  MessageManager::instance().loadSavedMessagesAuthCode();

  /* Init Chat Manager */
  (void)ChatManager::instance();

  /* Init Icon Manager */
  IconManager::instance().setSourcePath( Settings::instance().iconSourcePath() );

  /* Init Emoticon Manager */
  EmoticonManager::instance().setRecentEmoticonsCount( qMax( 8, Settings::instance().emoticonInRecentMenu() ) );
  EmoticonManager::instance().loadRecentEmoticons( Settings::instance().recentEmoticons() );
  EmoticonManager::instance().loadFavoriteEmoticons( Settings::instance().favoriteEmoticons() );

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

  /* Init BeeCore */
  Core bee_core;
  bee_core.init();
  QObject::connect( &bee_app, SIGNAL( networkConfigurationChanged( const QNetworkConfiguration& ) ), &bee_core, SLOT( updateNetworkConfiguration( const QNetworkConfiguration& ) ) );

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

  /* Event Loop */
  qDebug() << "Enter in the main event loop";
  int iRet = bee_app.exec();
  qDebug() << "Check and process the remaining events in loop";

  /* Check Icon Provider */
  qDebug() << "IconProvider has load in cache" << GuiIconProvider::instance().cacheSize() << "icons";

  /* Save final session */
  Settings::instance().setRecentEmoticons( EmoticonManager::instance().saveRencentEmoticons() );
  Settings::instance().setFavoriteEmoticons( EmoticonManager::instance().saveFavoriteEmoticons() );
  Settings::instance().loadRcFile();
  Settings::instance().save();

  /* CleanUp */
  bee_app.processEvents( QEventLoop::AllEvents, 2000 );
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
