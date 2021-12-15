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
// $Id: GuiWorkgroups.h 1455 2020-12-23 10:17:53Z mastroddi $
//
//////////////////////////////////////////////////////////////////////

#ifndef BEEBEEP_GUICREATETEXTFILE_H
#define BEEBEEP_GUICREATETEXTFILE_H

#include "Config.h"
#include "ui_GuiCreateTextFile.h"


class GuiCreateTextFile : public QDialog, private Ui::GuiCreateTextFileDialog
{
  Q_OBJECT

public:
  explicit GuiCreateTextFile( QWidget *parent = Q_NULLPTR );

  QString text() const;
  QString fileSuffix() const;
  inline bool sendAsFile() const;

protected slots:
  void sendTextAsMessage();
  void sendTextAsFile();

protected:
  void loadSettings();
  void saveSettings();
  void sendText();

private:
  bool m_sendAsFile;

};

// Inline Functions
bool GuiCreateTextFile::sendAsFile() const { return m_sendAsFile; }

#endif // BEEBEEP_GUICREATETEXTFILE_H
