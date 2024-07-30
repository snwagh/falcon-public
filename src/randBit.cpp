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

#include "randBit.h"


void prg_aes(uint8_t *dest, uint8_t *src, __m128i *ri) {
    __m128i rr, mr;
    __m128i *r = ri;

    rr = _mm_loadu_si128((__m128i *)src);
    mr = rr;

    mr = _mm_xor_si128(mr, r[0]);

    mr = _mm_aesenc_si128(mr, r[1]);
    mr = _mm_aesenc_si128(mr, r[2]);
    mr = _mm_aesenc_si128(mr, r[3]);
    mr = _mm_aesenc_si128(mr, r[4]);
    mr = _mm_aesenc_si128(mr, r[5]);
    mr = _mm_aesenc_si128(mr, r[6]);
    mr = _mm_aesenc_si128(mr, r[7]);
    mr = _mm_aesenc_si128(mr, r[8]);
    mr = _mm_aesenc_si128(mr, r[9]);
    mr = _mm_aesenclast_si128(mr, r[10]);
    mr = _mm_xor_si128(mr, rr);
    _mm_storeu_si128((__m128i *)dest, mr);
}

__m128i* prg_keyschedule(uint8_t *src) {
    __m128i *r = (__m128i *)malloc(11 * sizeof(__m128i));

    r[0] = _mm_load_si128((__m128i *)src);

    KE2(r[1], r[0], 0x01)
    KE2(r[2], r[1], 0x02)
    KE2(r[3], r[2], 0x04)
    KE2(r[4], r[3], 0x08)
    KE2(r[5], r[4], 0x10)
    KE2(r[6], r[5], 0x20)
    KE2(r[7], r[6], 0x40)
    KE2(r[8], r[7], 0x80)
    KE2(r[9], r[8], 0x1b)
    KE2(r[10], r[9], 0x36)

    return r;
}

void prg_getrandom(int keyID, uint size, uint length, uint8_t *dest) {
    // we assume container_size is 16, so all *container_size are replaced as <<4
    // this size means how many random bytes we need
    // uint8_t *buffer = new uint8_t [size];
    // its always size * length
    // printf("curent P is %d \n",P_container[keyID]);

   // setup
   uint8_t random_container[3][16];

   uint8_t RandomData[64] = {'1', '2', '3', '4', '1', '2', '3', '4',
                            '1', '2', '3', '4', '1', '2', '3', '4',
                            '1', '2', '3', '4', '1', '2', '3', '4',
                            '1', '2', '3', '4', '1', '2', '3', '4',
                            '4', '3', '2', '1', '4', '3', '2', '1',
                            '4', '3', '2', '1', '4', '3', '2', '1',
                            '4', '3', '2', '1', '4', '3', '2', '1',
                            '4', '3', '2', '1', '4', '3', '2', '1'};
   uint8_t tempKey_A[16];
   uint8_t tempKey_B[16];
   uint8_t tempKey_C[16];

   memcpy(random_container[0], RandomData, 16);
   memcpy(tempKey_A, RandomData + 16, 16);
   memcpy(tempKey_C, RandomData + 32, 16);
   memcpy(random_container[2], RandomData + 48, 16);

   // int pid = partyNum;
   // int map[2];
   // switch (pid) {
   //  case 0:
   //      map[0] = 2;
   //      map[1] = 1;
   //      break;
   //  case 1:
   //      map[0] = 0;
   //      map[1] = 2;
   //      break;
   //  case 2:
   //      map[0] = 1;
   //      map[1] = 0;
   //      break;
   //  }

   // sendDataToPeer(map[0], 32, RandomData);
   // getDataFromPeer(map[1], 32, RandomData);

   memcpy(random_container[1], RandomData, 16);
   memcpy(tempKey_B, RandomData + 16, 16);


   __m128i prg_key[3];
   prg_key[0] = *prg_keyschedule(tempKey_A);
   prg_key[1] = *prg_keyschedule(tempKey_B);
   prg_key[2] = *prg_keyschedule(tempKey_C);

   uint8_t res[16] = {};
   for (size_t i = 0; i < 3; i++) {
      prg_aes(res, random_container[i], &prg_key[i]);
      memcpy(&random_container[i], res, 16);
   }

   int P_container[3]= {0, 0, 0};
   int container_size = 16;
   
   uint rounds = ((size * length - container_size + P_container[keyID]) + 15) >> 4;
   // printf("rounds %u\n", rounds);
   if (rounds == 0) {
      memcpy(dest, random_container[keyID] + P_container[keyID], size * length);
      P_container[keyID] = P_container[keyID] + size * length;
   } else {
      memcpy(dest, &random_container[keyID] + P_container[keyID], container_size - P_container[keyID]);
      if (rounds >= 2) {
         prg_aes(dest + (container_size - P_container[keyID]), random_container[keyID], &prg_key[keyID]);
         for (int i = 1; i < rounds - 1; i++) {
               // segfault in this loop for "large" size
               // printf("i : %u\n", i);
               prg_aes(dest + (container_size - P_container[keyID]) + (i << 4), dest + (container_size - P_container[keyID]) + ((i - 1) << 4), &prg_key[keyID]);
         }
         prg_aes(random_container[keyID], dest + (container_size - P_container[keyID]) + ((rounds - 2) << 4), &prg_key[keyID]);
         memcpy(dest + container_size - P_container[keyID] + ((rounds - 1) << 4), &random_container[keyID], size * length - ((rounds - 1) << 4) - container_size + P_container[keyID]);
         P_container[keyID] = size * length - ((rounds - 1) << 4) - container_size + P_container[keyID];
      } else {
         prg_aes(random_container[keyID], random_container[keyID], &prg_key[keyID]);
         memcpy(dest + container_size - P_container[keyID], &random_container[keyID], size * length - container_size + P_container[keyID]);
         P_container[keyID] = size * length - container_size + P_container[keyID];
      }
   }

    // delete [] buffer;
}

void prg_getrandom(uint size, uint length, uint8_t *dest) {
    prg_getrandom(1, size, length, dest);
}

// void Rss_RandBit(RSSVectorSmallType &b, uint size, uint ring_size) {

//     int pid = partyNum;
//     uint i;
//     uint bytes = (ring_size + 9) >> 3;
//     // uint bytes = 1;
    
//     uint numShares = NUM_OF_PARTIES-1;

//     vector<smallType> u0(size, 0);
//     vector<smallType> u1(size, 0);

//     // RSSVectorSmallType _a(size, std::make_pair(0, 0));
//     RSSVectorMyType _a(size, std::make_pair(0, 0));
//     RSSVectorSmallType _d(size, std::make_pair(0, 0));

//     uint8_t *buffer = new uint8_t[bytes * size];

//     // RSSVectorSmallType _f(size, std::make_pair(0, 0));
//     RSSVectorMyType _f(size, std::make_pair(0, 0));
//     // vector<smallType> _e(size, 0);
//     // vector<smallType> _c(size, 0);
//     vector<myType> _e(size, 0);
//     vector<myType> _c(size, 0);

//     smallType ai[3];
//     memset(ai, 0, sizeof(smallType) * numShares);
//     if (pid == 0) {
//         ai[0] = 1;
//         ai[1] = 0;
//     } else if (pid == 1) {
//         ai[0] = 0;
//         ai[1] = 0;
//     } else { //(pid == 2)
//         ai[0] = 0;
//         ai[1] = 1;
//     }

//     prg_getrandom(0, bytes, size, buffer);
//     std::copy(buffer, buffer + bytes * size, u0.begin());
    
//     prg_getrandom(1, bytes, size, buffer);
//     std::copy(buffer, buffer + bytes * size, u1.begin());
    
//     for (i = 0; i < size; i++) {
//         _a[i].first = (u0[i] << smallType(1)) + ai[0];
//         _a[i].second = (u1[i] << smallType(1)) + ai[1];

//         // _a[i].first = (1 << (ring_size)) - 1;
//         // _a[i].second = (1 << (ring_size)) - 1;
//     }
//     // squaring a
//     funcDotProduct(_a, _a, _f, size, 0, 0);
//     funcReconstruct(_f, _e, size, "e", true);
//     rss_sqrt_inv(_c, _e, size, ring_size + 2);

//     // effectively combines the two loops into one, eliminates d variable
//     for (i = 0; i < size; i++) {
//         b[i].first = (_c[i] * _a[i].first + ai[0]) >> smallType(1);
//         b[i].second = (_c[i] * _a[i].second + ai[1]) >> smallType(1);
//     }

//     // freeing up
    
//     delete[] buffer;
    
// }

void Rss_RandBit(RSSVectorMyType &b, uint size, uint ring_size) {

    // int pid = nodeNet->getID();
    int pid = partyNum;
    uint i;
    uint bytes = (ring_size + 9) >> 3;
    // printf("bytes : %llu\n", bytes );
    // uint numShares = nodeNet->getNumShares();
    uint numShares = NUM_OF_PARTIES-1;

    Lint **u = new Lint *[numShares];
    Lint **a = new Lint *[numShares];
    Lint **d = new Lint *[numShares];

    for (i = 0; i < numShares; i++) {
        u[i] = new Lint[size];
        a[i] = new Lint[size];
        d[i] = new Lint[size];
    }
    Lint *e = new Lint[size];
    Lint *c = new Lint[size];
    uint8_t *buffer = new uint8_t[bytes * size];

    // falcon init
    RSSVectorMyType _a(size, std::make_pair(0, 0));
    RSSVectorMyType _f(size, std::make_pair(0, 0));
    // vector<myType> _f(size, myType(0));
    vector<myType> _e(size, myType(0));
    vector<myType> _c(size, myType(0));

    Lint *ai = new Lint[numShares];
    memset(ai, 0, sizeof(Lint) * numShares);
    if (pid == 0) {
        ai[0] = 1;
    } else if (pid == 2) {
        ai[numShares - 1] = 1;
    }

    // nodeNet->prg_getrandom(0, bytes, size, buffer);
    prg_getrandom(0, bytes, size, buffer);
    for (i = 0; i < size; i++) {
        memcpy(u[0] + i, buffer + i * bytes, bytes);
    }
    // nodeNet->prg_getrandom(1, bytes, size, buffer);
    prg_getrandom(1, bytes, size, buffer);
    for (i = 0; i < size; i++) {
        memcpy(u[1] + i, buffer + i * bytes, bytes);
    }

    for (i = 0; i < size; i++) {
        // // ensuring [a] is odd
        // for (size_t s = 0; s < numShares; s++)
        //     a[s][i] = (u[s][i] << Lint(1)) + ai[s];
        // // a[1][i] = (u[1][i] << Lint(1)) + a2;
        _a[i].first = (u[0][i] << myType(1)) + ai[0];
        _a[i].second = (u[1][i] << myType(1)) + ai[1];

    }
    // squaring a
    // Rss_MultPub(e, a, a, size, ring_size + 2, nodeNet); // ringsize+2

    // rss_sqrt_inv(c, e, size, ring_size + 2);

    funcDotProduct(_a, _a, _f, size, 0, 0);
    funcReconstruct(_f, _e, size, "e", false);
    // printf("mult:\n");
    // for (i = 0; i < size; i++) {
    //     _e[i] &= ((1 << (ring_size+2)) - 1);
    //     printf("%u, ", _e[i]);
    // }
    // printf("\n");
    rss_sqrt_inv(_c, _e, size, ring_size + 2);
    // printf("inverse:\n");
    // for (i = 0; i < size; i++) {
    //     _e[i] &= ((1 << (ring_size+2)) - 1);
    //     printf("%u, ", _c[i]);
    // }
    // printf("\n");

    // effectively combines the two loops into one, eliminates d variable
    for (i = 0; i < size; i++) {
        // for (size_t s = 0; s < numShares; s++)
        //     b[s][i] = (c[i] * a[s][i] + ai[s]) >> Lint(1);
        // b[1][i] = (c[i] * a[1][i] + a2) >> (1);
        b[i].first = (_c[i] * _a[i].first + myType(ai[0])) >> myType(1);
        b[i].second = (_c[i] * _a[i].second + myType(ai[1])) >> myType(1);

    }

    // freeing up
    delete[] c;
    delete[] buffer;
    delete[] e;
    for (i = 0; i < numShares; i++) {
        delete[] d[i];
        delete[] a[i];
        delete[] u[i];
    }
    delete[] d;
    delete[] a;
    delete[] ai;
    delete[] u;
}


void rss_sqrt_inv(vector<myType> &c, vector<myType> &e, uint size, uint ring_size) {

    Lint c1, c2, temp, d_;
    uint i, j;

    for (i = 0; i < size; i++) {
        c1 = Lint(1);
        c2 = Lint(1);
        d_ = Lint(4); // 100 - the first mask

        for (j = 2; j < ring_size - 1; j++) {
            temp = e[i] - (c1) * (c1);
            if (temp != Lint(0)) {
                // get the jth+1 bit of temp, place it in jth position, and add to c1
                c1 += (temp & (d_ << Lint(1))) >> Lint(1);
            }

            temp = Lint(1) - c1 * c2;
            // get the jth bit of temp and add it to c2
            c2 += temp & d_;
            d_ = d_ << Lint(1);
        }
        // last round for the inv portion
        temp = Lint(1) - c1 * c2;
        c[i] = c2 + (temp & d_);
    }
}
