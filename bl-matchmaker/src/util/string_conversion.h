#ifndef _UTIL_STRING_CONVERSION_H
#define _UTIL_STRING_CONVERSION_H

#include <charconv>
#include <string>

#include "types.h"

/// We have to do this because we only want to let people convert to integers and there's
/// no way to constraint template types.
#define DECLARE_STR_TO_X(suffix, T)\
	size_t str_to_##suffix (const std::string &str, size_t start_index, T &out_num);

DECLARE_STR_TO_X (u8, U8)
DECLARE_STR_TO_X (u16, U16)
DECLARE_STR_TO_X (u32, U32)
DECLARE_STR_TO_X (s8, S8)
DECLARE_STR_TO_X (s16, S16)
DECLARE_STR_TO_X (s32, S32)

#endif
