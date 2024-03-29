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

#ifndef BEEBEEP_GUIVCARD_H
#define BEEBEEP_GUIVCARD_H

#include "ui_GuiVCard.h"
#include "Config.h"
class User;


class GuiVCard : public QFrame, private Ui::GuiVCardWidget
{
  Q_OBJECT

public:
  GuiVCard( QWidget* parent = Q_NULLPTR );

  void setVCard( const User&, VNumber chat_id, bool );

signals:
  void showChat( VNumber );
  void sendFile( VNumber );
  void changeUserColor( VNumber, const QString& );
  void toggleFavorite( VNumber );
  void removeUser( VNumber );
  void buzzUser( VNumber );
  void sendHelpRequestToUser( VNumber );

private slots:
  void showPrivateChat();
  void sendFile();
  void changeColor();
  void favoriteClicked();
  void removeUserClicked();
  void sendBuzz();
  void sendHelp();

private:
  VNumber m_userId;
  VNumber m_chatId;
  QString m_userColor;

};

#endif // BEEBEEP_GUIVCARD_H
