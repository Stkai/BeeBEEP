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

#ifndef BEEBEEP_BUILDFILELIST_H
#define BEEBEEP_BUILDFILELIST_H

#include "Config.h"
#include "FileInfo.h"

// For ShareBox only
class BuildFileList : public QObject
{
  Q_OBJECT

public:
  explicit BuildFileList( QObject* parent = Q_NULLPTR );

  void init( const QString& folder_name, const QString folder_path, VNumber );

  inline const QString& folderPath() const;
  inline const QString& folderName() const;
  inline const QList<FileInfo>& fileList() const;
  inline int elapsedTime() const;
  inline VNumber toUserId() const;
  inline bool errorFound() const;

signals:
  void listCompleted();

public slots:
  void buildList();

private:
  QString m_folderPath;
  QString m_folderName;
  VNumber m_toUserId;
  QList<FileInfo> m_fileList;
  int m_elapsedTime;
  bool m_errorFound;

};


// Inline Functions
inline const QString& BuildFileList::folderPath() const { return m_folderPath; }
inline const QString& BuildFileList::folderName() const { return m_folderName; }
inline const QList<FileInfo>& BuildFileList::fileList() const { return m_fileList; }
inline int BuildFileList::elapsedTime() const { return m_elapsedTime; }
inline VNumber BuildFileList::toUserId() const { return m_toUserId; }
inline bool BuildFileList::errorFound() const { return m_errorFound; }

#endif // BEEBEEP_BUILDFILELIST_H
