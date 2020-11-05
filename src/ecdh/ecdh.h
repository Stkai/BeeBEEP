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

#ifndef BEEBEEP_ECDH_H
#define BEEBEEP_ECDH_H

/* for size-annotated integer types: uint8_t, uint32_t etc. */
#include <stdint.h>

namespace ECDH
{
  bool generatePrivateKey( uint8_t* private_key );

  /* NOTE: assumes private is filled with random data before calling */
  bool generatePublicKey( uint8_t* public_key, uint8_t* private_key );

  /* input: own private key + other party's public key, output: shared secret */
  bool generateSharedKey( const uint8_t* private_key, const uint8_t* others_pub, uint8_t* output );

}

#endif // BEEBEEP_ECDH_H
