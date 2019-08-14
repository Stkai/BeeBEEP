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

#ifndef BEEBEEP_GUIHOME_H
#define BEEBEEP_GUIHOME_H

#include "Config.h"
#include "ui_GuiHome.h"
class ChatMessage;


class GuiHome : public QWidget, private Ui::GuiHomeWidget
{
  Q_OBJECT

public:
  explicit GuiHome( QWidget *parent = Q_NULLPTR );

  inline void setMainToolTip( const QString& );
  inline QString mainToolTip() const;

  bool addSystemMessage( const ChatMessage& );
  int loadSystemMessages();

  void setNews( const QString& );

  void updateBackground();

signals:
  void openUrlRequest( const QUrl& );

public slots:
  void resetNews();

protected slots:
  void checkAnchorClicked( const QUrl& );
  void customContextMenu( const QPoint& );
  void onAddTimestampClicked();
  void reloadMessages();
  void printActivities();
  void selectBackgroundColor();

private:
  QString m_prev_sys_mess;
  QMenu* mp_menuContext;

};


// Inline Functions
inline void GuiHome::setMainToolTip( const QString& new_value ) { mp_teSystem->setToolTip( new_value ); }
inline QString GuiHome::mainToolTip() const { return mp_teSystem->toolTip(); }

#endif // BEEBEEP_GUIHOME_H
