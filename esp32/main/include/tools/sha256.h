#ifndef SHA256_F
#define SHA256_F

#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include <stddef.h>

#define MAX_DIGITS 8

#define DBL_INT_ADD(a, b, c)                                                   \
  if (a > 0xffffffff - (c))                                                    \
    ++b;                                                                       \
  a += c;
#define ROTLEFT(a, b) (((a) << (b)) | ((a) >> (32 - (b))))
#define ROTRIGHT(a, b) (((a) >> (b)) | ((a) << (32 - (b))))

#define CH(x, y, z) (((x) & (y)) ^ (~(x) & (z)))
#define MAJ(x, y, z) (((x) & (y)) ^ ((x) & (z)) ^ ((y) & (z)))
#define EP0(x) (ROTRIGHT(x, 2) ^ ROTRIGHT(x, 13) ^ ROTRIGHT(x, 22))
#define EP1(x) (ROTRIGHT(x, 6) ^ ROTRIGHT(x, 11) ^ ROTRIGHT(x, 25))
#define SIG0(x) (ROTRIGHT(x, 7) ^ ROTRIGHT(x, 18) ^ ((x) >> 3))
#define SIG1(x) (ROTRIGHT(x, 17) ^ ROTRIGHT(x, 19) ^ ((x) >> 10))

typedef struct {
  unsigned char data[64];
  unsigned int datalen;
  unsigned int bitlen[2];
  unsigned int state[8];
} SHA256_CTX;

char *RollingSHA256(char *data, size_t counter);

#endif
