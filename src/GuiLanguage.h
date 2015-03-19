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

#ifndef BEEBEEP_GUILANGUAGE_H
#define BEEBEEP_GUILANGUAGE_H

#include "ui_GuiLanguage.h"


class GuiLanguage : public QDialog, private Ui::GuiLanguageDialog
{
  Q_OBJECT

public:
  explicit GuiLanguage( QWidget *parent = 0 );

  void loadLanguages();
  inline const QString& languageSelected() const;
  inline const QString& folderSelected() const;

protected slots:
  void selectLanguage();
  void selectFolder();
  void restoreDefault();
  void checkItemClicked( QTreeWidgetItem*, int );

private:
  QString m_languageSelected;
  QString m_folderSelected;

};

// Inline Functions
inline const QString& GuiLanguage::languageSelected() const { return m_languageSelected; }
inline const QString& GuiLanguage::folderSelected() const { return m_folderSelected; }

#endif // BEEBEEP_GUILANGUAGE_H
