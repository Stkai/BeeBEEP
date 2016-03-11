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

#ifndef BEEBEEP_FILEDIALOG_H
#define BEEBEEP_FILEDIALOG_H

#include "Config.h"

class FileDialog
{
public:
  static QString getExistingDirectory( QWidget* parent = 0, const QString& caption = QString(),
                                       const QString& dir = QString() );

  static QString getSaveFileName( QWidget* parent = 0, const QString& caption = QString(),
                                  const QString& dir = QString(), const QString& filter = QString(),
                                  QString* selectedFilter = 0 );

  static QString getOpenFileName( bool with_image_preview, QWidget* parent = 0, const QString& caption = QString(),
                                       const QString& dir = QString(), const QString& filter = QString(),
                                       QString* selectedFilter = 0 );

  static QStringList getOpenFileNames( bool with_image_preview, QWidget* parent = 0, const QString& caption = QString(),
                                       const QString& dir = QString(), const QString& filter = QString(),
                                       QString* selectedFilter = 0 );

protected:
  FileDialog() {}

};

#endif // BEEBEEP_FILEDIALOG_H
