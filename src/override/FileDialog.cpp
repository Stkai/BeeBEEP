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

#include "FileDialog.h"
#include "PreviewFileDialog.h"
#include "Settings.h"


QString FileDialog::getExistingDirectory( QWidget* parent, const QString& caption, const QString& dir )
{
  QFileDialog::Options file_dialog_options = QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks;
  if( !Settings::instance().useNativeDialogs() )
    file_dialog_options |= QFileDialog::DontUseNativeDialog;
#if QT_VERSION >= 0x040806
  file_dialog_options |= QFileDialog::DontUseCustomDirectoryIcons;
#endif
  return QFileDialog::getExistingDirectory( parent, caption, dir, file_dialog_options );
}

QString FileDialog::getSaveFileName( QWidget* parent, const QString& caption, const QString& dir,
                                     const QString& filter, QString* selectedFilter )
{
  QFileDialog::Options file_dialog_options = 0;
  if( !Settings::instance().useNativeDialogs() )
    file_dialog_options |= QFileDialog::DontUseNativeDialog;
#if QT_VERSION >= 0x040806
  file_dialog_options |= QFileDialog::DontUseCustomDirectoryIcons;
#endif
  return QFileDialog::getSaveFileName( parent, caption, dir, filter, selectedFilter, file_dialog_options );
}

QString FileDialog::getOpenFileName( bool with_image_preview, QWidget* parent, const QString& caption,
                                     const QString& dir, const QString& filter, QString* selectedFilter )
{
  if( Settings::instance().useNativeDialogs() )
#if QT_VERSION >= 0x040806
    return QFileDialog::getOpenFileName( parent, caption, dir, filter, selectedFilter, QFileDialog::DontUseCustomDirectoryIcons );
#else
    return QFileDialog::getOpenFileName( parent, caption, dir, filter, selectedFilter );
#endif

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
#if QT_VERSION >= 0x040806
    return QFileDialog::getOpenFileName( parent, caption, dir, filter, selectedFilter, QFileDialog::DontUseNativeDialog | QFileDialog::DontUseCustomDirectoryIcons );
#else
    return QFileDialog::getOpenFileName( parent, caption, dir, filter, selectedFilter, QFileDialog::DontUseNativeDialog );
#endif
}

QStringList FileDialog::getOpenFileNames( bool with_image_preview, QWidget* parent, const QString& caption,
                                          const QString& dir, const QString& filter, QString* selectedFilter )
{
  if( Settings::instance().useNativeDialogs() )
#if QT_VERSION >= 0x040806
    return QFileDialog::getOpenFileNames( parent, caption, dir, filter, selectedFilter, QFileDialog::DontUseCustomDirectoryIcons );
#else
    return QFileDialog::getOpenFileNames( parent, caption, dir, filter, selectedFilter );
#endif

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
#if QT_VERSION >= 0x040806
    return QFileDialog::getOpenFileNames( parent, caption, dir, filter, selectedFilter, QFileDialog::DontUseNativeDialog | QFileDialog::DontUseCustomDirectoryIcons );
#else
    return QFileDialog::getOpenFileNames( parent, caption, dir, filter, selectedFilter, QFileDialog::DontUseNativeDialog );
#endif
}
