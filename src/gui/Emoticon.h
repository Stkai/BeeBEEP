//////////////////////////////////////////////////////////////////////
//
// BeeBEEP Copyright (C) 2010-2020 Marco Mastroddi
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

#ifndef BEEBEEP_EMOTICON_H
#define BEEBEEP_EMOTICON_H

#include "Config.h"


class Emoticon
{
public:
  enum Group { Unknown, Text, People, Objects, Nature, Places, Symbols, NumGroups };

  Emoticon();
  Emoticon( const QString&, const QString&, int emoticon_group, int sort_order );
  Emoticon( const Emoticon& );

  Emoticon& operator=( const Emoticon& );
  inline bool operator==( const Emoticon& ) const;

  inline bool isValid() const;

  inline const QString& textToMatch() const;
  inline const QString& name() const;
  inline void setGroup( int );
  inline int group() const;
  inline bool isInGroup() const;
  inline int sortOrder() const;
  inline QString fileName() const;

  inline QString toHtml( int icon_size ) const;
  inline QIcon icon() const;

  static QString groupFolder( int );
  static QIcon groupIcon( int );
  static QString sourceFolder();

private:
  QString m_textToMatch;
  QString m_name;
  int m_group;
  int m_sortOrder;

};


// Inline Functions
inline bool Emoticon::operator==( const Emoticon& e ) const { return m_textToMatch == e.m_textToMatch; }
inline bool Emoticon::isValid() const { return m_textToMatch.size() > 0 && m_name.size() > 0; }
inline const QString& Emoticon::textToMatch() const { return m_textToMatch; }
inline const QString& Emoticon::name() const { return m_name; }
inline void Emoticon::setGroup( int new_value ) { m_group = new_value; }
inline int Emoticon::group() const { return m_group; }
inline bool Emoticon::isInGroup() const { return m_group > Emoticon::Text && m_group < Emoticon::NumGroups; }
inline int Emoticon::sortOrder() const { return m_sortOrder; }
inline QString Emoticon::fileName() const { return QString( "%1/%2.png" ).arg( groupFolder( m_group ) ).arg( m_name ); }
inline QString Emoticon::toHtml( int icon_size ) const { return QString( "<img src=\"%1\"%2 />").arg( fileName() ).arg( QString( " height=\"%1\"" ).arg( icon_size ) ); }
inline QIcon Emoticon::icon() const { return QIcon( fileName() ); }

#endif // BEEBEEP_EMOTICON_H
