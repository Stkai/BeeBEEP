//////////////////////////////////////////////////////////////////////
//
// BeeBEEP Copyright (C) 2010-2020 Marco Mastroddi
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

#ifndef BEEBEEP_RANDOM_H
#define BEEBEEP_RANDOM_H

#include <QDateTime>
#if QT_VERSION > 0x050909
  #include <QRandomGenerator64>
#else
  #include <stdlib.h>
#endif


namespace Random
{
  inline qint32 __double2int32( double d ) { int i = static_cast<qint32>(d); return d-i >= 0.5 ? ++i : i; } // never negative in random
  inline qint64 __double2int64( double d ) { int i = static_cast<qint64>(d); return d-i >= 0.5 ? ++i : i; }

#if QT_VERSION > 0x050909
  inline void init() {}
  inline qint32 number32( qint32 from, qint32 to ) { return from >= to ? to : __double2int32( QRandomGenerator::global()->generateDouble() * ( to - from ) ) + from; }
  inline qint64 number64( qint64 from, qint64 to ) { return from >= to ? to : __double2int64( QRandomGenerator::global()->generateDouble() * ( to - from ) ) + from; }
#else
  inline void init() { qsrand( QTime( 0, 0, 0 ).secsTo( QTime::currentTime() ) ); }
  inline qint32 number32( int from, int to ) { return from >= to ? to : __double2int32((qrand()/(1.0 + RAND_MAX)) * ( to - from )) + from; }
  inline qint64 number64( int from, int to ) { return from >= to ? to : __double2int64((qrand()/(1.0 + RAND_MAX)) * ( to - from )) + from; }
#endif
  inline int roll( int dices, int faces ) { int result = 0; for( int i = 0; i < dices; i++ ) result += number32( 1, faces ); return result; }
  inline int d100() { return number32( 1, 100 ); }
  inline int d20() { return number32( 1, 20 ); }

} // namespace Random

#endif // BEEBEEP_RANDOM_H
