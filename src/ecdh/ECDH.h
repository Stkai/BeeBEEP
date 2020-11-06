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

#include "Config.h"

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

#define ECDH_CURVE_NIST_B163  1
#define ECDH_CURVE_NIST_K163  2
#define ECDH_CURVE_NIST_B233  3
#define ECDH_CURVE_NIST_K233  4
#define ECDH_CURVE_NIST_B283  5
#define ECDH_CURVE_NIST_K283  6
#define ECDH_CURVE_NIST_B409  7
#define ECDH_CURVE_NIST_K409  8
#define ECDH_CURVE_NIST_B571  9
#define ECDH_CURVE_NIST_K571 10

/* Curve selected (must be the same for all BeeBEEP in your network) */
#define ECDH_CURVE ECDH_CURVE_NIST_K163

#if defined(ECDH_CURVE) && (ECDH_CURVE != 0)
 #if   (ECDH_CURVE == ECDH_CURVE_NIST_K163) || (ECDH_CURVE == ECDH_CURVE_NIST_B163)
  #define ECDH_CURVE_DEGREE       163
  #define ECDH_PRIVATE_KEY_SIZE   24
 #elif (ECDH_CURVE == ECDH_CURVE_NIST_K233) || (ECDH_CURVE == ECDH_CURVE_NIST_B233)
  #define ECDH_CURVE_DEGREE       233
  #define ECDH_PRIVATE_KEY_SIZE   32
 #elif (ECDH_CURVE == ECDH_CURVE_NIST_K283) || (ECDH_CURVE == ECDH_CURVE_NIST_B283)
  #define ECDH_CURVE_DEGREE       283
  #define ECDH_PRIVATE_KEY_SIZE   36
 #elif (ECDH_CURVE == ECDH_CURVE_NIST_K409) || (ECDH_CURVE == ECDH_CURVE_NIST_B409)
  #define ECDH_CURVE_DEGREE       409
  #define ECDH_PRIVATE_KEY_SIZE   52
 #elif (ECDH_CURVE == ECDH_CURVE_NIST_K571) || (ECDH_CURVE == ECDH_CURVE_NIST_B571)
  #define ECDH_CURVE_DEGREE       571
  #define ECDH_PRIVATE_KEY_SIZE   72
 #endif
#endif

#define ECDH_PUBLIC_KEY_SIZE (2*ECDH_PRIVATE_KEY_SIZE)

namespace ECDH  {

  class Keys {
    public:
      Keys();
      virtual ~Keys();

      inline const QString& publicKey() const { return m_publicKey; }
      inline const QByteArray& sharedKey() const { return m_sharedKey; }

      void create();
      bool generateSharedKey( const QString& other_public_key );
      void reset();

    private:
      QString m_publicKey;
      QByteArray m_sharedKey;
      quint8 m_privateKey[ ECDH_PRIVATE_KEY_SIZE ];

  };

  void generatePrivateKey( quint8* private_key );
  void generatePublicKey( quint8* public_key, quint8* private_key );
  bool generateSharedKey( const quint8* private_key, const quint8* others_pub, quint8* output );
}

#endif // BEEBEEP_ECDH_H
