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

#ifndef BEEBEEP_GUIEDITVCARD_H
#define BEEBEEP_GUIEDITVCARD_H

#include "ui_GuiEditVCard.h"
#include "User.h"


class GuiEditVCard : public QDialog, private Ui::GuiVCardDialog
{
  Q_OBJECT

public:
  GuiEditVCard( QWidget* parent = 0 );

  void setUser( const User& );

  inline const VCard& vCard() const;
  inline const QString& userColor() const;

protected:
  void loadVCard();
  void setUserColor( const QString& );
  bool checkLineEdit( QLineEdit*, const QString& );

private slots:
  void changePhoto();
  void removePhoto();
  void checkData();
  void changeUserColor();

private:
  VCard m_vCard;
  QString m_userColor;

};


// Inline Functions
inline const VCard& GuiEditVCard::vCard() const { return m_vCard; }
inline const QString& GuiEditVCard::userColor() const { return m_userColor; }

#endif // BEEBEEP_GUIEDITVCARD_H
