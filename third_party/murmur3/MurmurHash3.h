// This source file was originally from:
//   https://github.com/PeterScott/murmur3
//
// We've changed it for use with VoltDB:
//   - We changed the functions declared below to return their hash by
//     value, rather than accept a pointer to storage for the result

//-----------------------------------------------------------------------------
// MurmurHash3 was written by Austin Appleby, and is placed in the public
// domain. The author hereby disclaims copyright to this source code.

#ifndef _MURMURHASH3_H_
#define _MURMURHASH3_H_

#include <string>

namespace murmur3 {

//-----------------------------------------------------------------------------
// Platform-specific functions and macros

// Microsoft Visual Studio

#if defined(_MSC_VER)

typedef unsigned char uint8_t;
typedef unsigned long uint32_t;
typedef unsigned __int64 uint64_t;

// Other compilers

#else	// defined(_MSC_VER)

#include <stdint.h>

#endif // !defined(_MSC_VER)

//-----------------------------------------------------------------------------

int32_t MurmurHash3_x64_128 (const void * key, int len, uint32_t seed );

inline int32_t MurmurHash3_x64_128(int32_t value, uint32_t seed) {
  return MurmurHash3_x64_128(&value, 4, seed);
}
inline int32_t MurmurHash3_x64_128(int32_t value) {
  return MurmurHash3_x64_128(&value, 4, 0);
}

inline int32_t MurmurHash3_x64_128(int64_t value, uint32_t seed) {
    return MurmurHash3_x64_128(&value, 8, seed);
}
inline int32_t MurmurHash3_x64_128(int64_t value) {
    return MurmurHash3_x64_128(&value, 8, 0);
}

inline int32_t MurmurHash3_x64_128(double value, uint32_t seed) {
  return MurmurHash3_x64_128(&value, 8, seed);
}

inline int32_t MurmurHash3_x64_128(std::string &value, uint32_t seed) {
  return MurmurHash3_x64_128(value.data(), static_cast<uint32_t >(value.size()), seed);
}

uint32_t MurmurHash3_x86_32(const void* key, uint32_t len, uint32_t seed);

void MurmurHash3_x64_128 ( const void * key, const int len,
                           const uint32_t seed, void * out );

//-----------------------------------------------------------------------------

}
#endif // _MURMURHASH3_H_
