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

#include "FileDialog.h"
#include "PreviewFileDialog.h"
#include "Settings.h"


QString FileDialog::getExistingDirectory( QWidget* parent, const QString& caption, const QString& dir )
{
  if( Settings::instance().useNativeDialogs() )
    return QFileDialog::getExistingDirectory( parent, caption, dir, QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks );
  else
    return QFileDialog::getExistingDirectory( parent, caption, dir, QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks | QFileDialog::DontUseNativeDialog );
}

QString FileDialog::getSaveFileName( QWidget* parent, const QString& caption, const QString& dir,
                                     const QString& filter, QString* selectedFilter )
{
  if( Settings::instance().useNativeDialogs() )
    return QFileDialog::getSaveFileName( parent, caption, dir, filter, selectedFilter );
  else
    return QFileDialog::getSaveFileName( parent, caption, dir, filter, selectedFilter, QFileDialog::DontUseNativeDialog );
}

QString FileDialog::getOpenFileName( bool with_image_preview, QWidget* parent, const QString& caption,
                                     const QString& dir, const QString& filter, QString* selectedFilter )
{
  if( Settings::instance().useNativeDialogs() )
    return QFileDialog::getOpenFileName( parent, caption, dir, filter, selectedFilter );

  if( with_image_preview && Settings::instance().usePreviewFileDialog() )
  {
    PreviewFileDialog pfd( parent, caption, dir, filter );
    pfd.setAcceptMode( QFileDialog::AcceptOpen );
    pfd.setFileMode( QFileDialog::ExistingFile );
    pfd.show();
    if( !Settings::instance().previewFileDialogGeometry().isEmpty() )
      pfd.restoreGeometry( Settings::instance().previewFileDialogGeometry() );

    if( pfd.exec() == QFileDialog::Accepted )
    {
      QStringList sl = pfd.selectedFiles();
      Settings::instance().setPreviewFileDialogGeometry( pfd.saveGeometry() );
      return sl.first();
    }
    else
      return "";
  }
  else
    return QFileDialog::getOpenFileName( parent, caption, dir, filter, selectedFilter, QFileDialog::DontUseNativeDialog );
}

QStringList FileDialog::getOpenFileNames( bool with_image_preview, QWidget* parent, const QString& caption,
                                          const QString& dir, const QString& filter, QString* selectedFilter )
{
  if( Settings::instance().useNativeDialogs() )
    return QFileDialog::getOpenFileNames( parent, caption, dir, filter, selectedFilter );

  if( with_image_preview && Settings::instance().usePreviewFileDialog() )
  {
    PreviewFileDialog pfd( parent, caption, dir, filter );
    pfd.setAcceptMode( QFileDialog::AcceptOpen );
    pfd.setFileMode( QFileDialog::ExistingFiles );
    pfd.show();
    if( !Settings::instance().previewFileDialogGeometry().isEmpty() )
      pfd.restoreGeometry( Settings::instance().previewFileDialogGeometry() );
    if( pfd.exec() == QFileDialog::Accepted )
    {
      Settings::instance().setPreviewFileDialogGeometry( pfd.saveGeometry() );
      return pfd.selectedFiles();
    }
    else
      return QStringList();
  }
  else
    return QFileDialog::getOpenFileNames( parent, caption, dir, filter, selectedFilter, QFileDialog::DontUseNativeDialog );
}
