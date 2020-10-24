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

#include "ECDH.h"

/* margin for overhead needed in intermediate calculations */
#define BITVEC_MARGIN     3
#define BITVEC_NBITS      (CURVE_DEGREE + BITVEC_MARGIN)
#define BITVEC_NWORDS     ((BITVEC_NBITS + 31) / 32)
#define BITVEC_NBYTES     (sizeof(uint32_t) * BITVEC_NWORDS)

/* Default to a (somewhat) constant-time mode?
   NOTE: The library is _not_ capable of operating in constant-time and leaks information via timing.
         Even if all operations are written const-time-style, it requires the hardware is able to multiply in constant time.
         Multiplication on ARM Cortex-M processors takes a variable number of cycles depending on the operands...
*/

/******************************************************************************/


/* the following type will represent bit vectors of length (CURVE_DEGREE+MARGIN) */
typedef uint32_t bitvec_t[BITVEC_NWORDS];
typedef bitvec_t gf2elem_t;           /* this type will represent field elements */
typedef bitvec_t scalar_t;


/******************************************************************************/

/* Here the curve parameters are defined. */
#define coeff_a  0
#define cofactor 4
/* NIST K-283 */
const gf2elem_t polynomial = { 0x000010a1, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x08000000 };
const gf2elem_t coeff_b    = { 0x00000001, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000 };
const gf2elem_t base_x     = { 0x58492836, 0xb0c2ac24, 0x16876913, 0x23c1567a, 0x53cd265f, 0x62f188e5, 0x3f1a3b81, 0x78ca4488, 0x0503213f };
const gf2elem_t base_y     = { 0x77dd2259, 0x4e341161, 0xe4596236, 0xe8184698, 0xe87e45c0, 0x07e5426f, 0x8d90f95d, 0x0f1c9e31, 0x01ccda38 };
const scalar_t  base_order = { 0x1e163c61, 0x94451e06, 0x265dff7f, 0x2ed07577, 0xffffe9ae, 0xffffffff, 0xffffffff, 0xffffffff, 0x01ffffff };

/*************************************************************************************************/

/* Private / static functions: */


/* some basic bit-manipulation routines that act on bit-vectors follow */
static int bitvec_get_bit(const bitvec_t x, const uint32_t idx)
{
  return ((x[idx / 32U] >> (idx & 31U) & 1U));
}

static void bitvec_clr_bit(bitvec_t x, const uint32_t idx)
{
  x[idx / 32U] &= ~(1U << (idx & 31U));
}

static void bitvec_copy(bitvec_t x, const bitvec_t y)
{
  int i;
  for (i = 0; i < BITVEC_NWORDS; ++i)
  {
    x[i] = y[i];
  }
}

static void bitvec_swap(bitvec_t x, bitvec_t y)
{
  bitvec_t tmp;
  bitvec_copy(tmp, x);
  bitvec_copy(x, y);
  bitvec_copy(y, tmp);
}

static int bitvec_equal(const bitvec_t x, const bitvec_t y)
{
  int i;
  for (i = 0; i < BITVEC_NWORDS; ++i)
  {
    if (x[i] != y[i])
    {
      return 0;
    }
  }
  return 1;
}

static void bitvec_set_zero(bitvec_t x)
{
  int i;
  for (i = 0; i < BITVEC_NWORDS; ++i)
  {
    x[i] = 0;
  }
}

static int bitvec_is_zero(const bitvec_t x)
{
  uint32_t i = 0;
  while (i < BITVEC_NWORDS)
  {
    if (x[i] != 0)
    {
      break;
    }
    i += 1;
  }
  return (i == BITVEC_NWORDS);
}

/* return the number of the highest one-bit + 1 */
static int bitvec_degree(const bitvec_t x)
{
  int i = BITVEC_NWORDS * 32;

  /* Start at the back of the vector (MSB) */
  x += BITVEC_NWORDS;

  /* Skip empty / zero words */
  while (    (i > 0)
          && (*(--x)) == 0)
  {
    i -= 32;
  }
  /* Run through rest if count is not multiple of bitsize of DTYPE */
  if (i != 0)
  {
    uint32_t u32mask = ((uint32_t)1 << 31);
    while (((*x) & u32mask) == 0)
    {
      u32mask >>= 1;
      i -= 1;
    }
  }
  return i;
}

/* left-shift by 'count' digits */
static void bitvec_lshift(bitvec_t x, const bitvec_t y, int nbits)
{
  int nwords = (nbits / 32);

  /* Shift whole words first if nwords > 0 */
  int i,j;
  for (i = 0; i < nwords; ++i)
  {
    /* Zero-initialize from least-significant word until offset reached */
    x[i] = 0;
  }
  j = 0;
  /* Copy to x output */
  while (i < BITVEC_NWORDS)
  {
    x[i] = y[j];
    i += 1;
    j += 1;
  }

  /* Shift the rest if count was not multiple of bitsize of DTYPE */
  nbits &= 31;
  if (nbits != 0)
  {
    /* Left shift rest */
    int i;
    for (i = (BITVEC_NWORDS - 1); i > 0; --i)
    {
      x[i]  = (x[i] << nbits) | (x[i - 1] >> (32 - nbits));
    }
    x[0] <<= nbits;
  }
}


/*************************************************************************************************/
/*
  Code that does arithmetic on bit-vectors in the Galois Field GF(2^CURVE_DEGREE).
*/
/*************************************************************************************************/


static void gf2field_set_one(gf2elem_t x)
{
  /* Set first word to one */
  x[0] = 1;
  /* .. and the rest to zero */
  int i;
  for (i = 1; i < BITVEC_NWORDS; ++i)
  {
    x[i] = 0;
  }
}

/* fastest check if x == 1 */
static int gf2field_is_one(const gf2elem_t x)
{
  /* Check if first word == 1 */
  if (x[0] != 1)
  {
    return 0;
  }
  /* ...and if rest of words == 0 */
  int i;
  for (i = 1; i < BITVEC_NWORDS; ++i)
  {
    if (x[i] != 0)
    {
      break;
    }
  }
  return (i == BITVEC_NWORDS);
}

/* galois field(2^m) addition is modulo 2, so XOR is used instead - 'z := a + b' */
static void gf2field_add(gf2elem_t z, const gf2elem_t x, const gf2elem_t y)
{
  int i;
  for (i = 0; i < BITVEC_NWORDS; ++i)
  {
    z[i] = (x[i] ^ y[i]);
  }
}

/* field multiplication 'z := (x * y)' */
static void gf2field_mul(gf2elem_t z, const gf2elem_t x, const gf2elem_t y)
{
  int i;
  gf2elem_t tmp;

  bitvec_copy(tmp, x);

  /* LSB set? Then start with x */
  if (bitvec_get_bit(y, 0) != 0)
  {
    bitvec_copy(z, x);
  }
  else /* .. or else start with zero */
  {
    bitvec_set_zero(z);
  }

  /* Then add 2^i * x for the rest */
  for (i = 1; i < CURVE_DEGREE; ++i)
  {
    /* lshift 1 - doubling the value of tmp */
    bitvec_lshift(tmp, tmp, 1);

    /* Modulo reduction polynomial if degree(tmp) > CURVE_DEGREE */
    if (bitvec_get_bit(tmp, CURVE_DEGREE))
    {
      gf2field_add(tmp, tmp, polynomial);
    }

    /* Add 2^i * tmp if this factor in y is non-zero */
    if (bitvec_get_bit(y, i))
    {
      gf2field_add(z, z, tmp);
    }
  }
}

/* field inversion 'z := 1/x' */
static void gf2field_inv(gf2elem_t z, const gf2elem_t x)
{
  gf2elem_t u, v, g, h;
  int i;

  bitvec_copy(u, x);
  bitvec_copy(v, polynomial);
  bitvec_set_zero(g);
  gf2field_set_one(z);

  while (!gf2field_is_one(u))
  {
    i = (bitvec_degree(u) - bitvec_degree(v));

    if (i < 0)
    {
      bitvec_swap(u, v);
      bitvec_swap(g, z);
      i = -i;
    }

    bitvec_lshift(h, v, i);
    gf2field_add(u, u, h);
    bitvec_lshift(h, g, i);
    gf2field_add(z, z, h);
  }
}

/*************************************************************************************************/
/*
   The following code takes care of Galois-Field arithmetic.
   Elliptic curve points are represented  by pairs (x,y) of bitvec_t.
   It is assumed that curve coefficient 'a' is {0,1}
   This is the case for all NIST binary curves.
   Coefficient 'b' is given in 'coeff_b'.
   '(base_x, base_y)' is a point that generates a large prime order group.
*/
/*************************************************************************************************/


static void gf2point_copy(gf2elem_t x1, gf2elem_t y1, const gf2elem_t x2, const gf2elem_t y2)
{
  bitvec_copy(x1, x2);
  bitvec_copy(y1, y2);
}

static void gf2point_set_zero(gf2elem_t x, gf2elem_t y)
{
  bitvec_set_zero(x);
  bitvec_set_zero(y);
}

static int gf2point_is_zero(const gf2elem_t x, const gf2elem_t y)
{
  return (    bitvec_is_zero(x)
           && bitvec_is_zero(y));
}

/* double the point (x,y) */
static void gf2point_double(gf2elem_t x, gf2elem_t y)
{
  /* iff P = O (zero or infinity): 2 * P = P */
  if (bitvec_is_zero(x))
  {
    bitvec_set_zero(y);
  }
  else
  {
    gf2elem_t l;

    gf2field_inv(l, x);
    gf2field_mul(l, l, y);
    gf2field_add(l, l, x);
    gf2field_mul(y, x, x);
    gf2field_mul(x, l, l);
    gf2field_add(x, x, l);
    gf2field_mul(l, l, x);
    gf2field_add(y, y, l);
  }
}


/* add two points together (x1, y1) := (x1, y1) + (x2, y2) */
static void gf2point_add(gf2elem_t x1, gf2elem_t y1, const gf2elem_t x2, const gf2elem_t y2)
{
  if (!gf2point_is_zero(x2, y2))
  {
    if (gf2point_is_zero(x1, y1))
    {
      gf2point_copy(x1, y1, x2, y2);
    }
    else
    {
      if (bitvec_equal(x1, x2))
      {
        if (bitvec_equal(y1, y2))
        {
          gf2point_double(x1, y1);
        }
        else
        {
          gf2point_set_zero(x1, y1);
        }
      }
      else
      {
        /* Arithmetic with temporary variables */
        gf2elem_t a, b, c, d;

        gf2field_add(a, y1, y2);
        gf2field_add(b, x1, x2);
        gf2field_inv(c, b);
        gf2field_mul(c, c, a);
        gf2field_mul(d, c, c);
        gf2field_add(d, d, c);
        gf2field_add(d, d, b);
        gf2field_add(x1, x1, d);
        gf2field_mul(a, x1, c);
        gf2field_add(a, a, d);
        gf2field_add(y1, y1, a);
        bitvec_copy(x1, d);
      }
    }
  }
}

static void gf2point_mul(gf2elem_t x, gf2elem_t y, const scalar_t exp)
{
  gf2elem_t tmpx, tmpy;
  int i;
  int nbits = bitvec_degree(exp);

  gf2point_set_zero(tmpx, tmpy);

  for (i = (nbits - 1); i >= 0; --i)
  {
    gf2point_double(tmpx, tmpy);
    if (bitvec_get_bit(exp, i))
    {
      gf2point_add(tmpx, tmpy, x, y);
    }
  }
  gf2point_copy(x, y, tmpx, tmpy);
}

/* check if y^2 + x*y = x^3 + a*x^2 + coeff_b holds */
static int gf2point_on_curve(const gf2elem_t x, const gf2elem_t y)
{
  gf2elem_t a, b;

  if (gf2point_is_zero(x, y))
  {
    return 1;
  }
  else
  {
    gf2field_mul(a, x, x);
    gf2field_mul(a, a, x);
    gf2field_add(a, a, coeff_b);
    gf2field_mul(b, y, y);
    gf2field_add(a, a, b);
    gf2field_mul(b, x, y);

    return bitvec_equal(a, b);
  }
}


/*************************************************************************************************/
/*
  Elliptic Curve Diffie-Hellman key exchange protocol.
*/
/*************************************************************************************************/

int ecdh_generate_keys( uint8_t* public_key, uint8_t* private_key )
{
  /* Get copy of "base" point 'G' */
  gf2point_copy((uint32_t*)public_key, (uint32_t*)(public_key + BITVEC_NBYTES), base_x, base_y);

  /* Abort key generation if random number is too small */
  if( bitvec_degree((uint32_t*)private_key) < (CURVE_DEGREE / 2))
  {
    return 0;
  }
  else
  {
    /* Clear bits > CURVE_DEGREE in highest word to satisfy constraint 1 <= exp < n. */
    int nbits = bitvec_degree(base_order);
    int i;

    for (i = (nbits - 1); i < (BITVEC_NWORDS * 32); ++i)
    {
      bitvec_clr_bit((uint32_t*)private_key, i);
    }

    /* Multiply base-point with scalar (private-key) */
    gf2point_mul((uint32_t*)public_key, (uint32_t*)(public_key + BITVEC_NBYTES), (uint32_t*)private_key);

    return 1;
  }
}

int ecdh_shared_secret(const uint8_t* private_key, const uint8_t* others_pub, uint8_t* output )
{
  /* Do some basic validation of other party's public key */
  if (    !gf2point_is_zero ((uint32_t*)others_pub, (uint32_t*)(others_pub + BITVEC_NBYTES))
       &&  gf2point_on_curve((uint32_t*)others_pub, (uint32_t*)(others_pub + BITVEC_NBYTES)) )
  {
    /* Copy other side's public key to output */
    unsigned int i;
    for (i = 0; i < (BITVEC_NBYTES * 2); ++i)
    {
      output[i] = others_pub[i];
    }

    /* Multiply other side's public key with own private key */
    gf2point_mul((uint32_t*)output,(uint32_t*)(output + BITVEC_NBYTES), (const uint32_t*)private_key);

    return 1;
  }
  else
  {
    return 0;
  }
}
