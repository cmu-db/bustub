// This source file was originally from:
//   https://github.com/PeterScott/murmur3
//
// We've changed it for use with VoltDB:
//   - We changed the top-level functions defined below to return
//     their hash by value, rather than accept a pointer to storage
//     for the result

//-----------------------------------------------------------------------------
// MurmurHash3 was written by Austin Appleby, and is placed in the public
// domain. The author hereby disclaims copyright to this source code.

// Note - The x86 and x64 versions do _not_ produce the same results, as the
// algorithms are optimized for their respective platforms. You can still
// compile and run any of them on any platform, but your performance with the
// non-native version will be less than optimal.

#include "MurmurHash3.h"

namespace murmur3 {
//-----------------------------------------------------------------------------
// Platform-specific functions and macros

// Microsoft Visual Studio

#if defined(_MSC_VER)

#define FORCE_INLINE	__forceinline

#include <stdlib.h>

#define ROTL64(x,y)	_rotl64(x,y)
#define ROTL32(x,y)	_rotl32(x,y)

#define BIG_CONSTANT(x) (x)

// Other compilers

#else	// defined(_MSC_VER)

#define	FORCE_INLINE inline __attribute__((always_inline))

inline uint32_t rotl32 ( uint32_t x, int8_t r )
{
  return (x << r) | (x >> (32 - r));
}

inline uint64_t rotl64 ( uint64_t x, int8_t r )
{
  return (x << r) | (x >> (64 - r));
}

#define ROTL32(x,y)	rotl32(x,y)
#define ROTL64(x,y)	rotl64(x,y)

#define BIG_CONSTANT(x) (x##LLU)

#endif // !defined(_MSC_VER)

//-----------------------------------------------------------------------------
// Block read - if your platform needs to do endian-swapping or can only
// handle aligned reads, do the conversion here

static FORCE_INLINE uint64_t getblock ( const uint64_t * p, int i )
{
  return p[i];
}

static FORCE_INLINE uint32_t getblock32 ( const uint32_t * p, int i )
{
  return p[i];
}

static FORCE_INLINE uint64_t getblock64 ( const uint64_t * p, int i )
{
  return p[i];
}

//-----------------------------------------------------------------------------
// Finalization mix - force all bits of a hash block to avalanche

static FORCE_INLINE uint32_t fmix32 ( uint32_t h )
{
  h ^= h >> 16;
  h *= 0x85ebca6b;
  h ^= h >> 13;
  h *= 0xc2b2ae35;
  h ^= h >> 16;

  return h;
}

static FORCE_INLINE uint64_t fmix ( uint64_t k )
{
  k ^= k >> 33;
  k *= BIG_CONSTANT(0xff51afd7ed558ccd);
  k ^= k >> 33;
  k *= BIG_CONSTANT(0xc4ceb9fe1a85ec53);
  k ^= k >> 33;

  return k;
}

static FORCE_INLINE uint64_t fmix64 ( uint64_t k )
{
  k ^= k >> 33;
  k *= BIG_CONSTANT(0xff51afd7ed558ccd);
  k ^= k >> 33;
  k *= BIG_CONSTANT(0xc4ceb9fe1a85ec53);
  k ^= k >> 33;

  return k;
}

//-----------------------------------------------------------------------------

int32_t MurmurHash3_x64_128 ( const void * key, const int len,
                           const uint32_t seed )
{
  const uint8_t * data = (const uint8_t*)key;
  const int nblocks = len / 16;

  uint64_t h1 = seed;
  uint64_t h2 = seed;

  const uint64_t c1 = BIG_CONSTANT(0x87c37b91114253d5);
  const uint64_t c2 = BIG_CONSTANT(0x4cf5ad432745937f);

  //----------
  // body

  const uint64_t * blocks = (const uint64_t *)(data);

  for(int i = 0; i < nblocks; i++)
  {
    uint64_t k1 = getblock(blocks,i*2+0);
    uint64_t k2 = getblock(blocks,i*2+1);

    k1 *= c1; k1  = ROTL64(k1,31); k1 *= c2; h1 ^= k1;

    h1 = ROTL64(h1,27); h1 += h2; h1 = h1*5+0x52dce729;

    k2 *= c2; k2  = ROTL64(k2,33); k2 *= c1; h2 ^= k2;

    h2 = ROTL64(h2,31); h2 += h1; h2 = h2*5+0x38495ab5;
  }

  //----------
  // tail

  const uint8_t * tail = (const uint8_t*)(data + nblocks*16);

  uint64_t k1 = 0;
  uint64_t k2 = 0;

  switch(len & 15)
  {
  case 15: k2 ^= uint64_t(tail[14]) << 48;  // fallthrough
  case 14: k2 ^= uint64_t(tail[13]) << 40;  // fallthrough
  case 13: k2 ^= uint64_t(tail[12]) << 32;  // fallthrough
  case 12: k2 ^= uint64_t(tail[11]) << 24;  // fallthrough
  case 11: k2 ^= uint64_t(tail[10]) << 16;  // fallthrough
  case 10: k2 ^= uint64_t(tail[ 9]) << 8;  // fallthrough
  case  9: k2 ^= uint64_t(tail[ 8]) << 0;  // fallthrough
           k2 *= c2; k2  = ROTL64(k2,33); k2 *= c1; h2 ^= k2;  // fallthrough

  case  8: k1 ^= uint64_t(tail[ 7]) << 56;  // fallthrough
  case  7: k1 ^= uint64_t(tail[ 6]) << 48;  // fallthrough
  case  6: k1 ^= uint64_t(tail[ 5]) << 40;  // fallthrough
  case  5: k1 ^= uint64_t(tail[ 4]) << 32;  // fallthrough
  case  4: k1 ^= uint64_t(tail[ 3]) << 24;  // fallthrough
  case  3: k1 ^= uint64_t(tail[ 2]) << 16;  // fallthrough
  case  2: k1 ^= uint64_t(tail[ 1]) << 8;  // fallthrough
  case  1: k1 ^= uint64_t(tail[ 0]) << 0;  // fallthrough
           k1 *= c1; k1  = ROTL64(k1,31); k1 *= c2; h1 ^= k1;  // fallthrough
  };

  //----------
  // finalization

  h1 ^= len; h2 ^= len;

  h1 += h2;
  h2 += h1;

  h1 = fmix(h1);
  h2 = fmix(h2);

  h1 += h2;
  h2 += h1;

  //Shift so that we use the higher order bits in case we want to use the lower order ones later
  //Also use the h1 higher order bits because it provided much better performance in voter, consistent too
  return static_cast<int32_t>(h1 >> 32);
}

uint32_t MurmurHash3_x86_32 ( const void * key, uint32_t len,
                          uint32_t seed )
{
  const uint8_t * data = (const uint8_t*)key;
  const int nblocks = len / 4;
  int i;

  uint32_t h1 = seed;

  uint32_t c1 = 0xcc9e2d51;
  uint32_t c2 = 0x1b873593;

  //----------
  // body

  const uint32_t * blocks = (const uint32_t *)(data + nblocks*4);

  for(i = -nblocks; i; i++)
  {
    uint32_t k1 = getblock32(blocks,i);

    k1 *= c1;
    k1 = ROTL32(k1,15);
    k1 *= c2;

    h1 ^= k1;
    h1 = ROTL32(h1,13);
    h1 = h1*5+0xe6546b64;
  }

  //----------
  // tail

  const uint8_t * tail = (const uint8_t*)(data + nblocks*4);

  uint32_t k1 = 0;

  switch(len & 3)
  {
  case 3: k1 ^= tail[2] << 16;  // fallthrough
  case 2: k1 ^= tail[1] << 8;  // fallthrough
  case 1: k1 ^= tail[0];  // fallthrough
          k1 *= c1; k1 = ROTL32(k1,15); k1 *= c2; h1 ^= k1;
  };

  //----------
  // finalization

  h1 ^= len;

  h1 = fmix32(h1);

  return h1;
}


void MurmurHash3_x64_128 ( const void * key, const int len,
                           const uint32_t seed, void * out )
{
  const uint8_t * data = (const uint8_t*)key;
  const int nblocks = len / 16;

  uint64_t h1 = seed;
  uint64_t h2 = seed;

  const uint64_t c1 = BIG_CONSTANT(0x87c37b91114253d5);
  const uint64_t c2 = BIG_CONSTANT(0x4cf5ad432745937f);

  //----------
  // body

  const uint64_t * blocks = (const uint64_t *)(data);

  for(int i = 0; i < nblocks; i++)
  {
    uint64_t k1 = getblock64(blocks,i*2+0);
    uint64_t k2 = getblock64(blocks,i*2+1);

    k1 *= c1; k1  = ROTL64(k1,31); k1 *= c2; h1 ^= k1;

    h1 = ROTL64(h1,27); h1 += h2; h1 = h1*5+0x52dce729;

    k2 *= c2; k2  = ROTL64(k2,33); k2 *= c1; h2 ^= k2;

    h2 = ROTL64(h2,31); h2 += h1; h2 = h2*5+0x38495ab5;
  }

  //----------
  // tail

  const uint8_t * tail = (const uint8_t*)(data + nblocks*16);

  uint64_t k1 = 0;
  uint64_t k2 = 0;

  switch(len & 15)
  {
  case 15: k2 ^= ((uint64_t)tail[14]) << 48;  // fallthrough
  case 14: k2 ^= ((uint64_t)tail[13]) << 40;  // fallthrough
  case 13: k2 ^= ((uint64_t)tail[12]) << 32;  // fallthrough
  case 12: k2 ^= ((uint64_t)tail[11]) << 24;  // fallthrough
  case 11: k2 ^= ((uint64_t)tail[10]) << 16;  // fallthrough
  case 10: k2 ^= ((uint64_t)tail[ 9]) << 8;  // fallthrough
  case  9: k2 ^= ((uint64_t)tail[ 8]) << 0;  // fallthrough
           k2 *= c2; k2  = ROTL64(k2,33); k2 *= c1; h2 ^= k2;  // fallthrough

  case  8: k1 ^= ((uint64_t)tail[ 7]) << 56;  // fallthrough
  case  7: k1 ^= ((uint64_t)tail[ 6]) << 48;  // fallthrough
  case  6: k1 ^= ((uint64_t)tail[ 5]) << 40;  // fallthrough
  case  5: k1 ^= ((uint64_t)tail[ 4]) << 32;  // fallthrough
  case  4: k1 ^= ((uint64_t)tail[ 3]) << 24;  // fallthrough
  case  3: k1 ^= ((uint64_t)tail[ 2]) << 16;  // fallthrough
  case  2: k1 ^= ((uint64_t)tail[ 1]) << 8;  // fallthrough
  case  1: k1 ^= ((uint64_t)tail[ 0]) << 0;  // fallthrough
           k1 *= c1; k1  = ROTL64(k1,31); k1 *= c2; h1 ^= k1;
  };

  //----------
  // finalization

  h1 ^= len; h2 ^= len;

  h1 += h2;
  h2 += h1;

  h1 = fmix64(h1);
  h2 = fmix64(h2);

  h1 += h2;
  h2 += h1;

  ((uint64_t*)out)[0] = h1;
  ((uint64_t*)out)[1] = h2;
}

}
//-----------------------------------------------------------------------------
