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
// $Id: Broadcaster.h 159 2012-01-24 19:18:37Z mastroddi $
//
//////////////////////////////////////////////////////////////////////

#ifndef BEEBEEP_BUILDFILESHARELIST_H
#define BEEBEEP_BUILDFILESHARELIST_H

#include "Config.h"
#include "FileInfo.h"


class BuildFileShareList : public QObject
{
  Q_OBJECT

public:
  explicit BuildFileShareList( QObject* parent = 0 );

  inline void setPath( const QString& );
  inline const QString& path() const;
  inline void setBroadcastList( bool );
  inline bool broadcastList() const;
  inline const QMultiMap<QString, FileInfo>& shareList() const;

signals:
  void listCompleted();

public slots:
  void buildList();
  void stopBuilding();

protected:
  void addPathToList( const QString&, const QString& );

private:
  QString m_path;
  bool m_broadcastList;
  QMultiMap<QString, FileInfo> m_shareList;
  bool m_stopBuilding;

};


// Inline Functions
inline void BuildFileShareList::setPath( const QString& new_value ) { m_path = new_value; }
inline const QString& BuildFileShareList::path() const { return m_path; }
inline void BuildFileShareList::setBroadcastList( bool new_value ) { m_broadcastList = new_value; }
inline bool BuildFileShareList::broadcastList() const { return m_broadcastList; }
inline const QMultiMap<QString, FileInfo>& BuildFileShareList::shareList() const { return m_shareList; }

#endif // BEEBEEP_BUILDFILESHARELIST_H
