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

#include "AudioManager.h"
#include "BeeApplication.h"
#include "ChatManager.h"
#include "ColorManager.h"
#include "EmoticonManager.h"
#include "FileShare.h"
#include "GuiConfig.h"
#include "GuiIconProvider.h"
#include "GuiMain.h"
#include "HistoryManager.h"
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
    qDebug() << language_file_path << "file not found. Language" << lang << "will not be installed";
    return false;
  }
  else
    qDebug() << "Language file" << language_file_path << "found and" << lang << "installed";
  qApp->installTranslator( translator );
  return true;
}

int main( int argc, char *argv[] )
{
  BeeApplication bee_app( argc, argv );
  (void)Settings::instance();
  bee_app.setApplicationName( Settings::instance().programName() );
  bee_app.setOrganizationName( Settings::instance().organizationName() );
  bee_app.setOrganizationDomain( Settings::instance().organizationDomain() );
  bee_app.setApplicationVersion( Settings::instance().version( false ) );

  if( bee_app.otherInstanceExists() )
  {
    qDebug() << Settings::instance().programName() << "closed";
    Settings::close();
    return 0;
  }

  /* Enable internal logs */
  Log::installMessageHandler();

  /* Randomize */
  Random::init();

  /* Load Settings */
  qDebug() << "Starting BeeBEEP" << qPrintable( Settings::instance().version( true ) )
           << "for" << qPrintable( Settings::instance().operatingSystem( true ) )
           << "and Qt" << QT_VERSION_STR;
  Settings::instance().setResourceFolder();
  Settings::instance().loadRcFile();
  Settings::instance().setDataFolder();
  qDebug() << "Settings path:" << qPrintable( Settings::instance().currentSettingsFilePath() );
  Settings::instance().load();
  Settings::instance().createLocalUser();

#ifdef BEEBEEP_DEBUG
  QFont chat_font = Settings::instance().chatFont();
  qDebug() << "Font selected for chat:" << chat_font.toString();
  qDebug() << "Font pixel size:" << chat_font.pixelSize();
  qDebug() << "Font point size:" << chat_font.pointSize();
  qDebug() << "Font height:" << (int)(QFontMetrics( chat_font).height());
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
  SetTranslator( &translator, Settings::instance().languagePath(), Settings::instance().language() );

  /* Init Network Manager */
  (void)NetworkManager::instance();

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

  /* Init Emoticon Manager */
  EmoticonManager::instance().loadRecentEmoticons( Settings::instance().recentEmoticons(), Settings::instance().emoticonInRecentMenu() );

  /* Init History Manager */
  (void)HistoryManager::instance();

  /* Init File Sharing */
  (void)FileShare::instance();

  /* Init Icon Provider */
  (void)GuiIconProvider::instance();

  /* Init Audio Manager */
  if( AudioManager::instance().isAudioDeviceAvailable() )
    qDebug() << "Sound manager is enabled";
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
  PluginManager::instance().loadPlugins();

  /* Init BeeApp */
  bee_app.init();

  if( !QSystemTrayIcon::isSystemTrayAvailable() )
    qWarning() << "System tray icon is not available in this OS";

  /* Init Main Window */
  GuiMain mw;
  QObject::connect( &bee_app, SIGNAL( enteringInIdle() ), &mw, SLOT( setInIdle() ) );
  QObject::connect( &bee_app, SIGNAL( exitingFromIdle() ), &mw, SLOT( exitFromIdle() ) );
  QObject::connect( &bee_app, SIGNAL( aboutToQuit() ), &mw, SLOT( forceShutdown() ) );
  QObject::connect( &bee_app, SIGNAL( showUp() ), &mw, SLOT( showUp() ) );
  QObject::connect( &bee_app, SIGNAL( tickEvent( int ) ), &mw, SLOT( onTickEvent( int ) ) );
  QObject::connect( &bee_app, SIGNAL( commitDataRequest( QSessionManager& ) ), &mw, SLOT( saveSession( QSessionManager& ) ), Qt::DirectConnection );

  if( !Settings::instance().guiGeometry().isEmpty() )
  {
    mw.restoreGeometry( Settings::instance().guiGeometry() );
    if( !Settings::instance().guiState().isEmpty() )
      mw.restoreState( Settings::instance().guiState() );
  }
  else
  {
    QDesktopWidget* desktop_widget = bee_app.desktop();
    QRect desktop_size = desktop_widget->availableGeometry();
    int app_w = qMin( (int)(desktop_size.width()-100), BEE_MAIN_WINDOW_BASE_SIZE_WIDTH );
    int app_h = qMin( (int)(desktop_size.height()-80), BEE_MAIN_WINDOW_BASE_SIZE_HEIGHT );
    mw.resize( QSize( app_w, app_h ) );
    int m_w = qMax( 0, (int)((desktop_size.width() - app_w) / 2) );
    int m_h = qMax( 0, (int)((desktop_size.height() - app_h) / 2) );
    mw.move( m_w, m_h );
  }

  mw.checkWindowFlagsAndShow();

  /* Load saved session */
  mw.loadSession();

  if( Settings::instance().loadOnTrayAtStartup() && QSystemTrayIcon::isSystemTrayAvailable() )
    QTimer::singleShot( 100, &mw, SLOT( hideToTrayIcon() ) );

  /* Starting connection to BeeBEEP Network */
  QTimer::singleShot( 500, &mw, SLOT( startStopCore() ) );

  if( Settings::instance().autoUserAway() )
    bee_app.setIdleTimeout( Settings::instance().userAwayTimeout() );

  /* Event Loop */
  int iRet = bee_app.exec();

  /* Check Icon Provider */
  qDebug() << "IconProvider has load in cache" << GuiIconProvider::instance().cacheSize() << "icons";

  /* Save final session */
  Settings::instance().setRecentEmoticons( EmoticonManager::instance().saveRencentEmoticons() );
  Settings::instance().loadRcFile();
  Settings::instance().save();

#ifdef BEEBEEP_DEBUG
  //Settings::instance().createDefaultRcFile();
  //Settings::instance().createDefaultHostsFile();
#endif

  /* CleanUp */
  bee_app.cleanUp();
  Settings::instance().clearTemporaryFile();
  GuiIconProvider::close();
  FileShare::close();
  HistoryManager::close();
  ChatManager::close();
  MessageManager::close();
  UserManager::close();
  Protocol::close();
  PluginManager::close();
  NetworkManager::close();
  ColorManager::close();
  AudioManager::close();
  ShortcutManager::close();
#ifdef BEEBEEP_USE_HUNSPELL
  SpellChecker::close();
#endif
  Log::instance().closeFileStream();
  Log::instance().close();
  Settings::close();

  /* Exit */
  return iRet;
}
