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

#ifndef BEEBEEP_GUIWEBVIEW_H
#define BEEBEEP_GUIWEBVIEW_H

#include "Config.h"
#include "ui_GuiWebView.h"


class GuiWebView : public QMainWindow, private Ui::GuiWebViewWidget
{
  Q_OBJECT

public:
  explicit GuiWebView( QWidget *parent = Q_NULLPTR );

signals:
  void newsLoadFinished( bool );

public slots:
  void loadNews();

private:
  QWebEngineView* mp_view;

};


#endif // BEEBEEP_GUIWEBVIEW_H
