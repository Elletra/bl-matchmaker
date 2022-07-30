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

bool BitReader::read_bits (S32 bit_count, void *out_ptr)
{
	if (curr_bit + bit_count > num_bits || data == nullptr)
	{
		return false;
	}

	U8 *ptr = (U8 *) out_ptr;

	for (S32 n = 0; n < bit_count; n++)
	{
		U8 byte = n >> 3;

		ptr[byte] = (ptr[byte] << 1) | _read_bit ();
	}

	out_ptr = ptr;

	return true;
}

bool BitReader::read (U8 &out)
{
	return read_bits (BITS_U8, &out);
}

bool BitReader::read (U16 &out)
{
	bool success = read_bits (BITS_U16, &out);

	if (success)
	{
		out = to_le (out);
	}

	return success;
}

bool BitReader::read (U32 &out)
{
	bool success = read_bits (BITS_U32, &out);

	if (success)
	{
		out = to_le (out);
	}

	return success;
}

bool BitReader::read (S8 &out)
{
	return read_bits (BITS_S8, &out);
}

bool BitReader::read (S16 &out)
{
	bool success = read_bits (BITS_S16, &out);

	if (success)
	{
		out = to_le (out);
	}

	return success;
}

bool BitReader::read (S32 &out)
{
	bool success = read_bits (BITS_S32, &out);

	if (success)
	{
		out = to_le (out);
	}

	return success;
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
/// This method assumes you have already done the appropriate sanity checking.
inline U8 BitReader::_read_bit ()
{
	size_t byte = curr_bit >> 3;
	U8 n = 7 - (curr_bit & 7);

	curr_bit++;

	return (data[byte] >> n) & 1U;
}
