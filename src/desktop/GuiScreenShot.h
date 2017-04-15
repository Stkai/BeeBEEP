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

#ifndef BEEBEEP_GUISCREENSHOT_H
#define BEEBEEP_GUISCREENSHOT_H

#include "Config.h"


class GuiScreenShot : public QMainWindow
{
  Q_OBJECT

public:
  explicit GuiScreenShot( QWidget *parent = 0 );

  inline const QPixmap& screenShot() const;
  void showUp();

signals:
  void screenShotToSend( const QString& );

protected slots:
  void doScreenShot();
  void captureScreen();
  void doSave();
  void doSend();
  void doDelete();

protected:
  void resizeEvent( QResizeEvent* );
  void updateScreenShot();
  void setupToolBar( QToolBar* );

private:
  QPixmap m_screenShot;
  QSpinBox* mp_sbDelay;
  QCheckBox* mp_cbHide;
#if QT_VERSION < 0x050000
  QCheckBox* mp_cbRetina;
#endif
  QAction* mp_actShot;
  QAction* mp_actSave;
  QAction* mp_actSend;
  QAction* mp_actDelete;

  QToolBar* mp_barScreenShot;
  QLabel* mp_labelScreenShot;

};


// Inline Functions
inline const QPixmap& GuiScreenShot::screenShot() const { return m_screenShot; }

#endif // BEEBEEP_GUISCREENSHOT_H
