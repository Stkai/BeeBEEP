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

#ifndef BEEBEEP_GUICONFIG_H
#define BEEBEEP_GUICONFIG_H

namespace {

const int BEE_MAIN_WINDOW_BASE_SIZE_WIDTH = 940;
const int BEE_MAIN_WINDOW_BASE_SIZE_HEIGHT = 600;
const int BEE_DOCK_WIDGET_SIZE_HINT_WIDTH = 200;
const int BEE_DOCK_WIDGET_SIZE_HINT_HEIGHT = 300;
const int BEE_DOCK_WIDGET_SIZE_HINT_HEIGHT_SMALL = 60;

const char* BEE_GUICUSTOMLIST_STYLESHEET = "#GuiCustomList { background: %1 url(%2);"
                                           "background-repeat: no-repeat;"
                                           "background-position: bottom center;"
                                           "background-attachment: fixed;"
                                           "padding-bottom: 32px;"
                                           "color: black; }";

}

#endif // BEEBEEP_GUICONFIG_H
