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

#include "GuiShareLocal.h"
#include "FileShare.h"
#include "Settings.h"


GuiShareLocal::GuiShareLocal( QWidget *parent )
  : QWidget(parent)
{
  setupUi( this );

  connect( mp_pbAddFile, SIGNAL( clicked() ), this, SLOT( addFilePath() ) );
  connect( mp_pbAddFolder, SIGNAL( clicked() ), this, SLOT( addFolderPath() ) );
  connect( mp_pbRemove, SIGNAL( clicked() ), this, SLOT( removePath() ) );
}

void GuiShareLocal::loadSettings()
{
  updateMyShares();
  updateShareList();
}

void GuiShareLocal::addFilePath()
{
  QStringList files = QFileDialog::getOpenFileNames( this, tr( "Select one or more files to share" ),
                                                     Settings::instance().lastDirectorySelected(),
                                                     "", 0, QFileDialog::DontResolveSymlinks );
  if( files.isEmpty() )
    return;

  foreach( QString file_path, files )
    FileShare::instance().addPath( file_path );

  loadSettings();
}

void GuiShareLocal::addFolderPath()
{
  QString folder_path = QFileDialog::getExistingDirectory( this, tr( "Select a folder to share" ),
                                                           Settings::instance().lastDirectorySelected(),
                                                           QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks );
  if( folder_path.isEmpty() )
    return;

  FileShare::instance().addPath( folder_path );

  loadSettings();
}

void GuiShareLocal::removePath()
{

}

void GuiShareLocal::updateMyShares()
{
  mp_twMyShares->clear();
  QTreeWidgetItem *item;
  foreach( QString share_path, Settings::instance().localShare() )
  {
    item = new QTreeWidgetItem( mp_twMyShares );
    item->setText( 0, share_path );
  }
}

void GuiShareLocal::updateShareList()
{
  mp_twLocalShares->clear();
  QTreeWidgetItem *item;
  foreach( FileInfo fi, FileShare::instance().local() )
  {
    item = new QTreeWidgetItem( mp_twMyShares );
    item->setText( 0, fi.path() );
  }
}
