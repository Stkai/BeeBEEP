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

#ifndef BEEBEEP_GUISHAREDESKTOP_H
#define BEEBEEP_GUISHAREDESKTOP_H

#include "Config.h"
#include "ui_GuiShareDesktop.h"
class User;


class GuiShareDesktop : public QMainWindow, private Ui::GuiShareDesktopWidget
{
  Q_OBJECT

public:
  GuiShareDesktop( QWidget* parent = Q_NULLPTR );

  void setUser( const User& );
  inline VNumber userId() const;

  void updateImage( const QImage&, const QString& image_type, QRgb diff_color );
  inline void resetLastImage();

  void onTickEvent( int );

public slots:
  void onUserChanged( const User& );

signals:
  void shareDesktopClosed( VNumber user_id );
  void shareDesktopDeleteRequest( VNumber user_id );

protected:
  void closeEvent( QCloseEvent* );

private:
  VNumber m_userId;
  QImage m_lastImage;
  QDateTime m_lastUpdate;
  bool m_toDelete;

};


// Inline Functions
inline VNumber GuiShareDesktop::userId() const { return m_userId; }
inline void GuiShareDesktop::resetLastImage() { m_lastImage = QImage(); }

#endif // BEEBEEP_GUISHAREDESKTOP_H
