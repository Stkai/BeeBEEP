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


TEST:
#ifdef BEEBEEP_DEBUG
  if( 1 )
  {
    int num_ecdh_key_generated = 0;
    QElapsedTimer test_ecdh_time;
    test_ecdh_time.start();
    while( test_ecdh_time.elapsed() < 2000 )
    {
      ECDH::Keys keys_1;
      keys_1.create();
      ECDH::Keys keys_2;
      keys_2.create();
      keys_1.generateSharedKey( keys_2.publicKey() );
      keys_2.generateSharedKey( keys_1.publicKey() );

      QByteArray test_shr_1 = Protocol::instance().createCipherKey( keys_1.sharedKey(), 15 );
      num_ecdh_key_generated++;
      QByteArray test_shr_2 = Protocol::instance().createCipherKey( keys_2.sharedKey(), 15 );
      num_ecdh_key_generated++;
      if( test_shr_1 == test_shr_2 )
        qDebug() << "User A and User B generate the same key";
      else
        qWarning() << "User A and User B FAIL to generate the same key";
    }
    qDebug() << "Test ECDH completed in" << test_ecdh_time.elapsed() << "ms with" << num_ecdh_key_generated << "keys";
  }
#endif

#ifdef BEEBEEP_DEBUG
  QElapsedTimer test_ecdh_time;
  test_ecdh_time.start();
  for( int i = 0; i < 30; i++ )
  {
    QByteArray test_pvt_1 = Protocol::instance().generatePrivateKey();
    QByteArray test_pub_1 = Protocol::instance().generatePublicKey( test_pvt_1 );
    qDebug() << "User A generates pvt" << test_pvt_1.toHex( ':' ) << "and pub" << test_pub_1.toHex( ':' );
    QByteArray test_pvt_2 = Protocol::instance().generatePrivateKey();
    QByteArray test_pub_2 = Protocol::instance().generatePublicKey( test_pvt_2 );
    qDebug() << "User B generates pvt" << test_pvt_2.toHex( ':' ) << "and pub" << test_pub_2.toHex( ':' );
    QByteArray test_shr_1 = Protocol::instance().generateSharedKey( test_pvt_1, test_pub_2, 90, 15 );
    QByteArray test_shr_2 = Protocol::instance().generateSharedKey( test_pvt_2, test_pub_1, 90, 15 );
    if( test_shr_1 == test_shr_2 )
      qDebug() << "User A and User B generate the same key";
    else
      qWarning() << "User A and User B FAIL to generate the same key";
  }
  qDebug() << "Test ECDH completed in" << test_ecdh_time.elapsed() << "ms";
#endif

*/

#ifndef BEEBEEP_ECDH_H
#define BEEBEEP_ECDH_H

/* for size-annotated integer types: uint8_t, uint32_t etc. */
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define NIST_B163  1
#define NIST_K163  2
#define NIST_B233  3
#define NIST_K233  4
#define NIST_B283  5
#define NIST_K283  6
#define NIST_B409  7
#define NIST_K409  8
#define NIST_B571  9
#define NIST_K571 10

/* What is the default curve to use? */
#ifndef ECC_CURVE
 #define ECC_CURVE NIST_K283
#endif

#if defined(ECC_CURVE) && (ECC_CURVE != 0)
 #if   (ECC_CURVE == NIST_K163) || (ECC_CURVE == NIST_B163)
  #define CURVE_DEGREE       163
  #define ECC_PRV_KEY_SIZE   24
 #elif (ECC_CURVE == NIST_K233) || (ECC_CURVE == NIST_B233)
  #define CURVE_DEGREE       233
  #define ECC_PRV_KEY_SIZE   32
 #elif (ECC_CURVE == NIST_K283) || (ECC_CURVE == NIST_B283)
  #define CURVE_DEGREE       283
  #define ECC_PRV_KEY_SIZE   36
 #elif (ECC_CURVE == NIST_K409) || (ECC_CURVE == NIST_B409)
  #define CURVE_DEGREE       409
  #define ECC_PRV_KEY_SIZE   52
 #elif (ECC_CURVE == NIST_K571) || (ECC_CURVE == NIST_B571)
  #define CURVE_DEGREE       571
  #define ECC_PRV_KEY_SIZE   72
 #endif
#else
 #error Must define a curve to use
#endif

#define ECC_PUB_KEY_SIZE     (2 * ECC_PRV_KEY_SIZE)


/******************************************************************************/

/* NOTE: assumes private is filled with random data before calling */
int ecdh_generate_keys( uint8_t* public_key, uint8_t* private_key );

/* input: own private key + other party's public key, output: shared secret */
int ecdh_shared_secret( const uint8_t* private_key, const uint8_t* others_pub, uint8_t* output );

/******************************************************************************/

#ifdef __cplusplus
}

#endif /* __cplusplus */

#endif // BEEBEEP_ECDH_H
