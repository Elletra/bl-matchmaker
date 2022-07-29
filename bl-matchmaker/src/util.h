#ifndef _UTIL_H
#define _UTIL_H

#include "types.h"

/* Endian conversion functions */

inline U16 to_le (U16 num)
{
	return (num >> 8) | (num << 8);
}

inline U32 to_le (U32 num)
{
	return ((num >> 24) & 0xFF) |
		((num << 8) & 0xFF0000) |
		((num >> 8) & 0xFF00) |
		((num << 24) & 0xFF000000);
}

inline S16 to_le (S16 num)
{
	return (S16) to_le ((U16) num);
}

inline S32 to_le (S32 num)
{
	return (S32) to_le ((U32) num);
}

#endif
