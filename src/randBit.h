/*   
   Multi-Party Replicated Secret Sharing over a Ring 
   ** Copyright (C) 2022 Alessandro Baccarini, Marina Blanton, and Chen Yuan
   ** Department of Computer Science and Engineering, University of Buffalo (SUNY)

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef RANDBIT_H_
#define RANDBIT_H_

// #include "NodeNetwork.h"
// #include "Mult.h"
#include <cstdlib>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <errno.h>
#include <fcntl.h>
#include <inttypes.h>
#include <iostream>
#include <map>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <openssl/aes.h>
#include <openssl/bio.h>
#include <openssl/evp.h>
#include <openssl/pem.h>
#include <openssl/rand.h>
#include <openssl/rsa.h>
#include <stdint.h> //for int8_t
#include <stdio.h>
#include <string.h> //for memcmp
#include <string>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <time.h>
#include <tmmintrin.h>
#include <unistd.h>
#include <vector>
#include <wmmintrin.h> //for intrinsics for AES-NI
#include <x86intrin.h>

#include "globals.h"
#include "connect.h"
#include "Functionalities.h"

#define KE2(NK, OK, RND)                           \
    NK = OK;                                       \
    NK = _mm_xor_si128(NK, _mm_slli_si128(NK, 4)); \
    NK = _mm_xor_si128(NK, _mm_slli_si128(NK, 4)); \
    NK = _mm_xor_si128(NK, _mm_slli_si128(NK, 4)); \
    NK = _mm_xor_si128(NK, _mm_shuffle_epi32(_mm_aeskeygenassist_si128(OK, RND), 0xff));

#define ANSI_COLOR_RED "\x1b[31m"
#define ANSI_COLOR_RESET "\x1b[0m"
#define ANSI_COLOR_GREEN "\x1b[32m"
#define ANSI_COLOR_YELLOW "\033[33m"

void prg_aes(uint8_t *dest, uint8_t *src, __m128i *ri);
__m128i* prg_keyschedule(uint8_t *src);

void prg_getrandom(int keyID, uint size, uint length, uint8_t *dest);
void prg_getrandom(uint size, uint length, uint8_t *dest);

void Rss_RandBit(RSSVectorMyType &b, uint size, uint ring_size = 8);
// void Rss_RandBit(const RSSVectorMyType &b, uint size, uint ring_size = 32);

void rss_sqrt_inv(vector<myType> &c, vector<myType> &e, uint size, uint ring_size = 8);

#endif