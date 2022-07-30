#ifndef _BITWRITER_BITWRITER_H
#define _BITWRITER_BITWRITER_H

#include <stdio.h>
#include <stdlib.h>

#include "types.h"

class BitWriter
{
private:
	U8 *data;
	size_t data_size;
	size_t num_bits;
	size_t curr_bit;

	bool auto_free_data;

public:
	BitWriter (void *data, size_t data_size);
	BitWriter (size_t data_size, bool auto_free_data = true);
	BitWriter () : BitWriter (0, false) {}
	~BitWriter ();

	inline size_t get_num_bits () const { return num_bits; }
	inline size_t get_bit_pos () const { return curr_bit; }
	inline size_t get_data_size () const { return data_size; }

	inline U8 *get_data_buffer () { return data; }

	inline void reset () { curr_bit = 0; }

	inline void set_bit_pos (size_t bit_pos)
	{
		if (bit_pos < num_bits)
		{
			curr_bit = bit_pos;
		}
		else if (num_bits > 0)
		{
			curr_bit = num_bits - 1;
		}
	}

	inline void set_data_buffer (U8 *buffer, size_t size)
	{
		data = buffer;
		data_size = size;
		num_bits = data_size * 8;

		reset ();
	}

	bool write_bit (bool value);
	bool write_flag (bool flag);

	bool write_bits (U32 bit_count, const void *bit_ptr);

	bool write_int (U8 num, S32 bit_count = sizeof (U8) << 3);
	bool write_int (U16 num, S32 bit_count = sizeof (U16) << 3);
	bool write_int (U32 num, S32 bit_count = sizeof (U32) << 3);
	bool write_int (S8 num, S32 bit_count = sizeof (S8) << 3);
	bool write_int (S16 num, S32 bit_count = sizeof (S16) << 3);
	bool write_int (S32 num, S32 bit_count = sizeof (S32) << 3);

	//bool write_string (char buffer[256]);

private:
	/// Dangerous method with no sanity checks!!
	///
	/// This method assumes you have already done the appropriate sanity checking with `curr_bit`.
	inline void _write_bit (bool value);
};

#endif
