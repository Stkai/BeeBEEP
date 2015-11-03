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

#include "GuiLanguage.h"
#include "Settings.h"


GuiLanguage::GuiLanguage( QWidget *parent )
  : QDialog( parent )
{
  setupUi( this );

  setWindowTitle( tr( "Select language" ) + QString( " - %1" ).arg( Settings::instance().programName() ) );

  QStringList labels;
  labels << tr( "Language" ) << tr( "File" );
  mp_twLanguage->setHeaderLabels( labels );
  mp_twLanguage->setAlternatingRowColors( true );
  mp_twLanguage->setSortingEnabled( true );
  mp_twLanguage->setRootIsDecorated( false );

  m_languageSelected = Settings::instance().language();
  m_folderSelected = Settings::instance().languagePath();

  mp_leLanguage->setText( m_languageSelected );
  mp_lePath->setText( m_folderSelected );

  connect( mp_pbSelect, SIGNAL( clicked() ), this, SLOT( selectLanguage() ) );
  connect( mp_pbCancel, SIGNAL( clicked() ), this, SLOT( reject() ) );
  connect( mp_pbSelectFolder, SIGNAL( clicked() ), this, SLOT( selectFolder() ) );
  connect( mp_pbDefault, SIGNAL( clicked() ), this, SLOT( restoreDefault() ) );
  connect( mp_twLanguage, SIGNAL( itemClicked( QTreeWidgetItem*, int ) ), this, SLOT( checkItemClicked( QTreeWidgetItem*, int ) ) );
}

void GuiLanguage::loadLanguages()
{
  if( mp_twLanguage->topLevelItemCount() > 0 )
    mp_twLanguage->clear();

  QDir language_path( mp_lePath->text().simplified() );
  if( !language_path.exists() )
  {
    qWarning() << "Language path not found:" << language_path.dirName();
    return;
  }

  QTreeWidgetItem* item;
  QStringList filter_list;
  filter_list << QString( "*.qm" );
  QList<QFileInfo> language_files = language_path.entryInfoList( filter_list );
  foreach( QFileInfo language_file, language_files )
  {
    QStringList sl = language_file.completeBaseName().split( "_" );
    QString lang = "en";
    if( sl.size() > 1 )
      lang = sl.at( 1 );
    item = new QTreeWidgetItem( mp_twLanguage );
    item->setText( 0, lang );
    item->setText( 1, language_file.fileName() );
    item->setToolTip( 1, language_file.absoluteFilePath() );
  }
}

void GuiLanguage::selectFolder()
{
  QString folder_path = QFileDialog::getExistingDirectory( this, tr( "Select a language folder" ), mp_lePath->text().simplified() );
  if( folder_path.isEmpty() )
    return;
  mp_lePath->setText( folder_path );
  loadLanguages();
}

void GuiLanguage::selectLanguage()
{
  QString folder_selected = mp_lePath->text().simplified();
  QString language_selected = mp_leLanguage->text().trimmed();

  if( !language_selected.isEmpty() )
  {
    QString language_file_path = Settings::instance().languageFilePath( folder_selected, language_selected );

    QFileInfo file_info( language_file_path );
    if( !file_info.exists() )
    {
      QMessageBox::warning( this, Settings::instance().programName(), tr( "Language '%1'' not found." ).arg( language_file_path ) );
      return;
    }
  }

  if( folder_selected == QApplication::applicationDirPath() || folder_selected.isEmpty() )
  {
#ifdef BEEBEEP_DEBUG
    qDebug() << "The language folder is the same of application folder. Change it to '.'";
#endif
    folder_selected = ".";
  }

  m_folderSelected = folder_selected;
  m_languageSelected = language_selected;

  accept();
}

void GuiLanguage::restoreDefault()
{
  m_folderSelected = ".";
  m_languageSelected = "";
  accept();
}

void GuiLanguage::checkItemClicked( QTreeWidgetItem* item, int )
{
  if( !item )
    return;

  mp_leLanguage->setText( item->text( 0 ) );
}
