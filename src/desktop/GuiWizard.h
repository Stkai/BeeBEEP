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

#ifndef BEEBEEP_GUIWIZARD_H
#define BEEBEEP_GUIWIZARD_H

#include "Config.h"
#include "ui_GuiWizard.h"


class GuiWizard : public QDialog, private Ui::GuiWizardDialog
{
  Q_OBJECT

public:
  GuiWizard( QWidget* parent = Q_NULLPTR );
  void loadSettings();

  inline const QString& userName() const;

protected slots:
  void saveSettings();

private:
  QString m_userName;

};

// Inline Functions
inline const QString& GuiWizard::userName() const { return m_userName; }

#endif // BEEBEEP_GUIWIZARD_H
