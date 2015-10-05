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
  explicit GuiHome( QWidget *parent = 0 );

  void loadDefaultChat();
  void addSystemMessage( const ChatMessage& );

signals:
  void openDefaultChatRequest();
  void openUrlRequest( const QUrl& );

protected slots:
  void openDefaultChat();
  void checkAnchorClicked( const QUrl& );
  void customContextMenu( const QPoint& );

};


#endif // BEEBEEP_GUIHOME_H
