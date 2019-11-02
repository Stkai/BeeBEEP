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
// $Id: rainbowtextmarker_global.h 346 2015-04-05 16:12:37Z mastroddi $
//
//////////////////////////////////////////////////////////////////////

#ifndef RAINBOWTEXTMAKER_GLOBAL_H
#define RAINBOWTEXTMAKER_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(RAINBOWTEXTMARKER_LIBRARY)
#  define RAINBOWTEXTMARKERSHARED_EXPORT Q_DECL_EXPORT
#else
#  define RAINBOWTEXTMARKERSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // RAINBOWTEXTMAKER_GLOBAL_H
