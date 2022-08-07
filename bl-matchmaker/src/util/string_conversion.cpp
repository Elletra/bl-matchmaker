#include <stdio.h>

#include "string_conversion.h"

template<typename T>
static inline size_t str_to_int (const std::string &str, size_t start_index, T &out_num)
{
	if (start_index >= str.length ())
	{
		return start_index;
	}

	const char *start_ptr = str.data () + start_index;
	char *end_ptr = (char *) start_ptr;

	out_num = (T) strtoll (start_ptr, &end_ptr, 10);

	return end_ptr - str.data ();
}

/// We have to do this because we only want to let people convert to integers and there's
/// no way to constraint template types.
#define IMPLEMENT_STR_TO_X(suffix, T)\
	size_t str_to_##suffix (const std::string &str, size_t start_index, T &out_num)\
	{\
		return str_to_int (str, start_index, out_num);\
	}

IMPLEMENT_STR_TO_X (u8, U8)
IMPLEMENT_STR_TO_X (u16, U16)
IMPLEMENT_STR_TO_X (u32, U32)
IMPLEMENT_STR_TO_X (s8, S8)
IMPLEMENT_STR_TO_X (s16, S16)
IMPLEMENT_STR_TO_X (s32, S32)
