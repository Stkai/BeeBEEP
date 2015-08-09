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

#ifndef BEEBEEP_BUILDFILESHARELIST_H
#define BEEBEEP_BUILDFILESHARELIST_H

#include "Config.h"
#include "FileInfo.h"


class BuildFileShareList : public QObject
{
  Q_OBJECT

public:
  explicit BuildFileShareList( QObject* parent = 0 );

  void setPath( const QString& );
  inline const QString& path() const;
  inline const QString& shareFolder() const;
  inline void setBroadcastList( bool );
  inline bool broadcastList() const;
  inline const QList<FileInfo>& shareList() const;
  inline FileSizeType shareSize() const;
  inline int elapsedTime() const;
  inline void setUserId( VNumber );
  inline VNumber userId() const;

signals:
  void listCompleted();

public slots:
  void buildList();

protected:
  FileSizeType addPathToList( const QString& );

private:
  QString m_path;
  QString m_shareFolder;
  bool m_broadcastList;
  QList<FileInfo> m_shareList;
  FileSizeType m_shareSize;
  int m_elapsedTime;
  VNumber m_userId;

};


// Inline Functions
inline const QString& BuildFileShareList::path() const { return m_path; }
inline const QString& BuildFileShareList::shareFolder() const { return m_shareFolder; }
inline void BuildFileShareList::setBroadcastList( bool new_value ) { m_broadcastList = new_value; }
inline bool BuildFileShareList::broadcastList() const { return m_broadcastList; }
inline const QList<FileInfo>& BuildFileShareList::shareList() const { return m_shareList; }
inline FileSizeType BuildFileShareList::shareSize() const { return m_shareSize; }
inline int BuildFileShareList::elapsedTime() const { return m_elapsedTime; }
inline void BuildFileShareList::setUserId( VNumber new_value ) { m_userId = new_value; }
inline VNumber BuildFileShareList::userId() const { return m_userId; }

#endif // BEEBEEP_BUILDFILESHARELIST_H
