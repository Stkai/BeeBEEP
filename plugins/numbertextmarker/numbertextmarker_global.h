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
// $Id: rainbowtextmarker_global.h 96 2011-09-13 17:47:13Z mastroddi $
//
//////////////////////////////////////////////////////////////////////

#ifndef NUMBERTEXTMAKER_GLOBAL_H
#define NUMBERTEXTMAKER_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(NUMBERTEXTMARKER_LIBRARY)
#  define NUMBERTEXTMARKERSHARED_EXPORT Q_DECL_EXPORT
#else
#  define NUMBERTEXTMARKERSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // NUMBERTEXTMAKER_GLOBAL_H
