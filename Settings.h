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

#ifndef BEEBEEP_SETTINGS_H
#define BEEBEEP_SETTINGS_H

#include "User.h"


class Settings
{
// Singleton Object
  static Settings* mp_instance;

public:
  QString version() const;
  QString programName() const;
  inline const User& localUser() const;
  inline void setLocalUserNickname( const QString& );
  inline QString defaultChatName() const;
  inline QString chatName( const User& ) const;
  inline void setListenerPort( int );
  inline int listenerPort() const;

  inline void setShowMenuBar( bool );
  inline bool showMenuBar() const;
  inline void setShowToolBar( bool );
  inline bool showToolBar() const;

  inline const QString& chatFontColor() const;
  inline void setChatFontColor( const QString& );
  inline const QFont& chatFont() const;
  inline void setChatFont( const QFont& );
  inline bool chatCompact() const;
  inline void setChatCompact( bool );
  inline bool chatAddNewLineToMessage() const;
  inline void setChatAddNewLineToMessage( bool );
  inline bool chatShowMessageTimestamp() const;
  inline void setChatShowMessageTimestamp( bool );
  inline const QString& chatSaveDirectory() const;
  inline void setChatSaveDirectory( const QString& );
  inline bool showUserIp() const;
  inline void setShowUserIp( bool new_value );
  inline bool showUserNickname() const;
  inline void setShowUserNickname( bool );

  inline int userListWidth() const;
  inline void setUserListWidth( int );
  inline const QSize& mainBarIconSize() const;
  inline void setGuiGeometry( const QByteArray& );
  inline const QByteArray& guiGeometry() const;

  QString hash( const QString& nick ) const;
  inline const QString& hash() const;
  void setPassword( const QString& );
  inline const QByteArray& password() const;
  inline QString defaultPassword() const;

  inline bool debugMode() const;
  inline void setDebugMode( bool );

  inline const QString& language() const;
  inline void setLanguage( const QString& );

  inline const QString& lastDirectorySelected() const;
  inline void setLastDirectorySelected( const QString& );

  inline bool useEncryption() const;
  inline void setUseEncryption( bool );

  void load( bool check_environment_also );
  void save();


  static Settings& instance()
  {
    if( !mp_instance )
      mp_instance = new Settings();
    return *mp_instance;
  }

  static void close()
  {
    if( mp_instance )
    {
      delete mp_instance;
      mp_instance = NULL;
    }
  }

protected:
  Settings();
  QString getUserName( const QProcessEnvironment& );
  void checkSystemEnvinroment();

private:
  User m_localUser;
  int m_listenerPort;

  bool m_showMenuBar;
  bool m_showToolBar;

  QString m_chatFontColor;
  QFont m_chatFont;
  bool m_chatCompact;
  bool m_chatAddNewLineToMessage;
  bool m_chatShowMessageTimestamp;
  QString m_chatSaveDirectory;
  bool m_showUserIp;
  bool m_showUserNickname;

  int m_userListWidth;
  QByteArray m_guiGeometry;
  QSize m_mainBarIconSize;

  QByteArray m_password;
  QString m_hash;

  bool m_debugMode;

  QString m_language;

  QString m_lastDirectorySelected;

  bool m_useEncryption;

};


// Inline Functions

inline const User& Settings::localUser() const { return m_localUser; }
inline void Settings::setLocalUserNickname( const QString& new_value ) { m_localUser.setNickname( new_value ); }
inline QString Settings::defaultChatName() const { return QObject::tr( "*All*" ); }
inline QString Settings::chatName( const User& u ) const { return u == m_localUser ? defaultChatName() : QString( "%1 (%2@%3)" ).arg( u.nickname() ).arg( u.name() ).arg( u.hostAddress().toString() ); }
inline void Settings::setListenerPort( int new_value ) { m_listenerPort = new_value; }
inline int Settings::listenerPort() const { return m_listenerPort; }
inline void Settings::setShowMenuBar( bool new_value) { m_showMenuBar = new_value; }
inline bool Settings::showMenuBar() const { return m_showMenuBar; }
inline void Settings::setShowToolBar( bool new_value ) { m_showToolBar = new_value; }
inline bool Settings::showToolBar() const { return m_showToolBar; }
inline const QSize& Settings::mainBarIconSize() const { return m_mainBarIconSize; }
inline const QString& Settings::chatFontColor() const { return m_chatFontColor; }
inline void Settings::setChatFontColor( const QString& new_value ) { m_chatFontColor = new_value; }
inline const QFont& Settings::chatFont() const { return m_chatFont; }
inline void Settings::setChatFont( const QFont& new_value ) { m_chatFont = new_value; }
inline bool Settings::chatCompact() const { return m_chatCompact; }
inline void Settings::setChatCompact( bool new_value ) { m_chatCompact = new_value; }
inline bool Settings::chatAddNewLineToMessage() const { return m_chatAddNewLineToMessage; }
inline void Settings::setChatAddNewLineToMessage( bool new_value ) { m_chatAddNewLineToMessage = new_value; }
inline bool Settings::chatShowMessageTimestamp() const { return m_chatShowMessageTimestamp; }
inline void Settings::setChatShowMessageTimestamp( bool new_value ) { m_chatShowMessageTimestamp = new_value; }
inline const QString& Settings::chatSaveDirectory() const { return m_chatSaveDirectory; }
inline void Settings::setChatSaveDirectory( const QString& new_value ) { m_chatSaveDirectory = new_value; }
inline bool Settings::showUserIp() const { return m_showUserIp; }
inline void Settings::setShowUserIp( bool new_value ) { m_showUserIp = new_value; }
inline bool Settings::showUserNickname() const { return m_showUserNickname; }
inline void Settings::setShowUserNickname( bool new_value ) { m_showUserNickname = new_value; }
inline int Settings::userListWidth() const { return m_userListWidth; }
inline void Settings::setGuiGeometry( const QByteArray& new_value ) { m_guiGeometry = new_value; }
inline const QByteArray& Settings::guiGeometry() const { return m_guiGeometry; }
inline void Settings::setUserListWidth( int new_value ) { m_userListWidth = new_value; }
inline const QString& Settings::hash() const { return m_hash; }
inline const QByteArray& Settings::password() const { return m_password; }
inline QString Settings::defaultPassword() const { return "*"; }
inline bool Settings::debugMode() const { return m_debugMode; }
inline void Settings::setDebugMode( bool new_value ) { m_debugMode = new_value; }
inline const QString& Settings::language() const { return m_language; }
inline void Settings::setLanguage( const QString& new_value ) { m_language = new_value; }
inline const QString& Settings::lastDirectorySelected() const { return m_lastDirectorySelected; }
inline void Settings::setLastDirectorySelected( const QString& new_value ) { m_lastDirectorySelected = new_value; }
inline bool Settings::useEncryption() const { return m_useEncryption; }
inline void Settings::setUseEncryption( bool new_value ) { m_useEncryption = new_value; }

#endif // BEEBEEP_SETTINGS_H
