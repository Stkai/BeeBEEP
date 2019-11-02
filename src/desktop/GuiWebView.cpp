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

#include "GuiWebView.h"
#include "IconManager.h"
#include "Settings.h"


GuiWebView::GuiWebView( QWidget* parent )
  : QMainWindow( parent )
{
  setObjectName( "GuiWebView" );
  setupUi( this );
  setWindowIcon( IconManager::instance().icon( "network.png" ) );
  setWindowTitle( QString( "%1 - %2" ).arg( tr( "News" ) ).arg( Settings::instance().programName() ) );
  mp_view = new QWebEngineView( mp_central );
  mp_view->setObjectName( "mp_view" );
  mp_layout->addWidget( mp_view, 0, 0, 1, 1 );
  connect( mp_view, SIGNAL( loadFinished( bool ) ), this, SIGNAL( newsLoadFinished( bool ) ) );
}

void GuiWebView::loadNews()
{
  mp_view->load( QUrl( "https://www.marcomastroddi.com" ) );
}
