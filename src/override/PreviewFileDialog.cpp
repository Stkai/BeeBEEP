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

#include "BeeUtils.h"
#include "PreviewFileDialog.h"


PreviewFileDialog::PreviewFileDialog( QWidget* parent, const QString& caption, const QString& directory, const QString& filter )
  : QFileDialog( parent, caption, directory, filter )
{
  setObjectName("PreviewFileDialog");
  setOption( QFileDialog::DontUseNativeDialog, true );

  QVBoxLayout* box = new QVBoxLayout(this);

  mp_preview = new QLabel(tr("Preview"), this);
  mp_preview->setAlignment(Qt::AlignCenter);
  mp_preview->setObjectName("labelPreview");
  mp_preview->setGeometry( 0, 0, 200, 100 );
    box->addWidget(mp_preview);

    box->addStretch();

    // add to QFileDialog layout
    {
        QGridLayout *layout = (QGridLayout*)this->layout();
        layout->addLayout(box, 1, 3, 3, 1);
    }
    connect(this, SIGNAL(currentChanged(const QString&)), this, SLOT(OnCurrentChanged(const QString&)));
}

void PreviewFileDialog::OnCurrentChanged(const QString & path)
{
    QPixmap pixmap = QPixmap(path);
    if (pixmap.isNull()) {
        mp_preview->setText("not an image");
    } else {
        mp_preview->setPixmap(pixmap.scaled(mp_preview->width(), mp_preview->height(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    }
}
