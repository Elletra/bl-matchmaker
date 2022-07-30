#include "BitReader.h"

#include "util.h"

// -----------------------------------------------------------------------------

BitReader::BitReader (U8 *data, size_t data_size, bool auto_free_data)
	: data (data),
	data_size (data_size),
	auto_free_data (auto_free_data)
{
	num_bits = data_size * 8;
	curr_bit = 0;
}

BitReader::~BitReader ()
{
	if (data && auto_free_data)
	{
		free (data);
		data = nullptr;
	}
}

bool BitReader::read_bit (U8 &out_value)
{
	if (curr_bit >= num_bits || data == nullptr)
	{
		return false;
	}

	out_value = _read_bit ();

	return true;
}

bool BitReader::read_flag ()
{
	U8 flag = 0;
	return read_bit (flag) && flag;
}

bool BitReader::read_bits (U32 bit_count, void *out_ptr)
{
	if (curr_bit + bit_count > num_bits || data == nullptr)
	{
		return false;
	}

	U8 *ptr = (U8 *) out_ptr;

	for (U32 n = 0; n < bit_count; n++)
	{
		if (_read_bit ())
		{
			ptr[n >> 3] += 1 << (n & 7);
		}
	}

	out_ptr = ptr;

	return true;
}

S32 BitReader::read_int (U8 bit_count)
{
	S32 num = 0;
	read_bits (bit_count, &num);
	return num;
}

U8 BitReader::read_u8 (U8 bit_count)
{
	return read_int (bit_count);
}

U16 BitReader::read_u16 (U8 bit_count)
{
	return read_int (bit_count);
}

U32 BitReader::read_u32 (U8 bit_count)
{
	return read_int (bit_count);
}

S8 BitReader::read_s8 (U8 bit_count)
{
	return read_int (bit_count);
}

S16 BitReader::read_s16 (U8 bit_count)
{
	return read_int (bit_count);
}

S32 BitReader::read_s32 (U8 bit_count)
{
	return read_int (bit_count);
}

void BitReader::read_string (char buffer[256])
{
	buffer[255] = '\0';

	if (read_flag ())
	{
		printf ("String len > 2\n");
	}
	else
	{
		printf ("String len <= 2\n");
	}
}

// -----------------------------------------------------------------------------

/// Dangerous method with no sanity checks!!
///
/// This method assumes you have already done the appropriate sanity checking with `curr_bit`.
inline U8 BitReader::_read_bit ()
{
	size_t byte = curr_bit >> 3;
	U8 n = curr_bit & 7;

	curr_bit++;

	return (data[byte] >> n) & 1U;
}
