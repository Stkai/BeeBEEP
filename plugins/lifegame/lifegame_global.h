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
// $Id: lifegame_global.h 346 2015-04-05 16:12:37Z mastroddi $
//
//////////////////////////////////////////////////////////////////////

#ifndef BEE_LIFEGAME_GLOBAL_H
#define BEE_LIFEGAME_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(BEE_LIFEGAME_LIBRARY)
#  define BEELIFEGAMESHARED_EXPORT Q_DECL_EXPORT
#else
#  define BEELIFEGAMESHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // BEE_LIFEGAME_GLOBAL_H
