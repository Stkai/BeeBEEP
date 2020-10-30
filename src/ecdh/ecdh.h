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

/*
  Crypto using elliptic curves defined over the finite binary field GF(2^m) where m is prime.
  The curves used are the anomalous binary curves (ABC-curves) or also called Koblitz curves.
  This class of curves was chosen because it yields efficient implementation of operations.
  Curves available - their different NIST/SECG names and eqivalent symmetric security level:
      NIST      SEC Group     strength
    ------------------------------------
      K-163     sect163k1      80 bit
      B-163     sect163r2      80 bit
      K-233     sect233k1     112 bit
      B-233     sect233r1     112 bit
      K-283     sect283k1     128 bit
      B-283     sect283r1     128 bit
      K-409     sect409k1     192 bit
      B-409     sect409r1     192 bit
      K-571     sect571k1     256 bit
      B-571     sect571r1     256 bit
  Curve parameters from:
    http://www.secg.org/sec2-v2.pdf
    http://csrc.nist.gov/publications/fips/fips186-3/fips_186-3.pdf
  Reference:
    https://www.ietf.org/rfc/rfc4492.txt
*/

#ifndef BEEBEEP_ECDH_H
#define BEEBEEP_ECDH_H

/* for size-annotated integer types: uint8_t, uint32_t etc. */
#include <stdint.h>


namespace ECDH {

  int privateKeySize();
  int publicKeySize();

  /* NOTE: assumes private is filled with random data before calling */
  bool generatePublicKey( uint8_t* public_key, uint8_t* private_key );

  /* input: own private key + other party's public key, output: shared secret */
  bool generateSharedKey( const uint8_t* private_key, const uint8_t* others_pub, uint8_t* output );

}

#endif // BEEBEEP_ECDH_H
