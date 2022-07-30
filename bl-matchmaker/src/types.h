#ifndef _T3D_TYPES_H
#define _T3D_TYPES_H

#include <stdint.h>

typedef uint8_t U8;
typedef uint16_t U16;
typedef uint32_t U32;
typedef uint64_t U64;

typedef int8_t S8;
typedef int16_t S16;
typedef int32_t S32;
typedef int64_t S64;

typedef unsigned int uint;

typedef float F32;
typedef double F64;

#define BITS_U8 (sizeof (U8) << 3)
#define BITS_U16 (sizeof (U16) << 3)
#define BITS_U32 (sizeof (U32) << 3)

#define BITS_S8 (sizeof (S8) << 3)
#define BITS_S16 (sizeof (S16) << 3)
#define BITS_S32 (sizeof (S32) << 3)

#endif
