//////////////////////////////////////////////////////////////////////
//
// BeeBEEP Copyright (C) 2010-2021 Marco Mastroddi
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

#include "BeeUtils.h"
#include "PreviewFileDialog.h"
#include "Settings.h"


PreviewFileDialog::PreviewFileDialog( QWidget* parent, const QString& caption, const QString& directory, const QString& filter )
  : QFileDialog( parent, caption, directory, filter )
{
  setObjectName("PreviewFileDialog");
  setOption( QFileDialog::DontUseNativeDialog, true );

  mp_preview = new QLabel( this );
  mp_preview->setAlignment( Qt::AlignCenter );
  mp_preview->setObjectName( "LabelPreview" );
  mp_preview->setMinimumWidth( qMax( 100, Settings::instance().previewFileDialogImageSize() ) );
  mp_preview->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding );
  m_defaultPixmap = Bee::convertToGrayScale( QIcon( ":images/beebeep.png" ).pixmap( Settings::instance().previewFileDialogImageSize(), Settings::instance().previewFileDialogImageSize() ) );
  setPixmapInPreview( m_defaultPixmap );

  {
    QGridLayout *layout = (QGridLayout*)this->layout();
    layout->addWidget( mp_preview, 0, layout->columnCount()+1, layout->rowCount()-1, 1, 0 );
  }

  connect( this, SIGNAL( currentChanged( const QString& ) ), this, SLOT( onCurrentChanged( const QString& ) ) );
}

void PreviewFileDialog::onCurrentChanged( const QString& file_path )
{
  QString file_suffix = Bee::suffixFromFile( file_path );
  if( Bee::isFileTypeImage( file_suffix ) )
  {
    QPixmap pix = QPixmap( file_path );
    if( !pix.isNull() )
    {
      setPixmapInPreview( pix );
      return;
    }
  }

  setPixmapInPreview( m_defaultPixmap );
}

void PreviewFileDialog::setPixmapInPreview( const QPixmap& pix )
{
  int max_width = qMax( Settings::instance().previewFileDialogImageSize(), mp_preview->width() );
  if( pix.width() > max_width || pix.height() > mp_preview->height() )
  {
    mp_preview->setPixmap( pix.scaled( max_width, mp_preview->height(), Qt::KeepAspectRatio, Qt::SmoothTransformation ) );
  }
  else
    mp_preview->setPixmap( pix );
}
