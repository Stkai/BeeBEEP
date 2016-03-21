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
// $Id$
//
//////////////////////////////////////////////////////////////////////

#ifndef BEEBEEP_RIJNDAEL_H
#define BEEBEEP_RIJNDAEL_H

int rijndaelSetupEncrypt(unsigned long *rk, const unsigned char *key, int keybits);
int rijndaelSetupDecrypt(unsigned long *rk, const unsigned char *key, int keybits);
void rijndaelEncrypt(const unsigned long *rk, int nrounds, const unsigned char plaintext[16], unsigned char ciphertext[16]);
void rijndaelDecrypt(const unsigned long *rk, int nrounds, const unsigned char ciphertext[16], unsigned char plaintext[16]);

#define KEYLENGTH(keybits) ((keybits)/8)
#define RKLENGTH(keybits)  ((keybits)/8+28)
#define NROUNDS(keybits)   ((keybits)/32+6)

#endif // BEEBEEP_RIJNDAEL_H
