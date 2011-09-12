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

#ifndef BEEBEEP_RANDOM_H
#define BEEBEEP_RANDOM_H

namespace Random
{
  inline int __double2int( double d ) { int i = static_cast<int>(d); return d-i >= 0.5 ? ++i : i; } // never negative in random
  inline void init() { qsrand( QDateTime::currentDateTime().toTime_t() ); } 
  inline int number( int from, int to ) { return from >= to ? to : __double2int((qrand()/(1.0 + RAND_MAX))*( to - from )) + from; }
  inline int roll( int dices, int faces ) { int result = 0; for( int i = 0; i < dices; i++ ) result += number( 1, faces ); return result; }
  inline int d100() { return number( 1, 100 ); }
  inline int d20() { return number( 1, 20 ); }
  
} // namespace Random

#endif // BEEBEEP_RANDOM_H
