#include <memory.h>
#include <stdlib.h>

#include "BitWriter.h"

BitWriter::BitWriter (size_t data_size, bool auto_free_data)
	: data_size (data_size),
	auto_free_data (auto_free_data)
{
	data = nullptr;
	num_bits = data_size * 8;
	curr_bit = 0;

	if (data_size > 0)
	{
		data = (U8 *) malloc (sizeof (U8) * data_size);

		if (data)
		{
			memset (data, 0, sizeof (U8) * data_size);
		}
	}
}

BitWriter::~BitWriter ()
{
	if (data && auto_free_data)
	{
		free (data);
		data = nullptr;
	}
}

bool BitWriter::write_bit (bool value)
{
	if (curr_bit >= num_bits || data == nullptr)
	{
		return false;
	}

	_write_bit (value);

	return true;
}

bool BitWriter::write_flag (bool flag)
{
	if (!write_bit (flag))
	{
		return false;
	}

	return flag;
}

bool BitWriter::write_bits (U32 bit_count, const void *bit_ptr)
{
	if (curr_bit + bit_count > num_bits || data == nullptr)
	{
		return false;
	}

	U8 *ptr = (U8 *) bit_ptr;

	for (U32 n = 0; n < bit_count; n++)
	{
		_write_bit ((ptr[curr_bit >> 3] >> (n & 7)) & 1U);
	}

	return true;
}

/* write_int() overloads */

bool BitWriter::write_int (U8 num, S32 bit_count)
{
	return write_bits (bit_count, &num);
}

bool BitWriter::write_int (U16 num, S32 bit_count)
{
	return write_bits (bit_count, &num);
}

bool BitWriter::write_int (U32 num, S32 bit_count)
{
	return write_bits (bit_count, &num);
}

bool BitWriter::write_int (S8 num, S32 bit_count)
{
	return write_bits (bit_count, &num);
}

bool BitWriter::write_int (S16 num, S32 bit_count)
{
	return write_bits (bit_count, &num);
}

bool BitWriter::write_int (S32 num, S32 bit_count)
{
	return write_bits (bit_count, &num);
}

// -----------------------------------------------------------------------------

/// Dangerous method with no sanity checks!!
///
/// This method assumes you have already done the appropriate sanity checking.
void BitWriter::_write_bit (bool value)
{
	U8 n = (curr_bit & 7);

	if (value)
	{
		data[curr_bit >> 3] |= 1U << n;
	}
	else
	{
		data[curr_bit >> 3] &= ~(1U << n);
	}

	curr_bit++;
}
