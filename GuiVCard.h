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

#ifndef BEEBEEP_GUIVCARD_H
#define BEEBEEP_GUIVCARD_H

#include "ui_GuiVCard.h"
#include "VCard.h"


class GuiVCard : public QDialog, private Ui::GuiVCardDialog
{
  Q_OBJECT

public:
  GuiVCard( QWidget* parent = 0 );

  void setVCard( const VCard& );
  inline const VCard& vCard() const;

protected:
  void loadVCard();

private slots:
  void changePhoto();
  void checkData();

private:
  VCard m_vCard;
  QButtonGroup m_bgGender;

};


// Inline Functions
inline const VCard& GuiVCard::vCard() const { return m_vCard; }


#endif // BEEBEEP_GUIVCARD_H
