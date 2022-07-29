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
	BitWriter (size_t data_size, bool auto_free_data = true);
	BitWriter () : BitWriter (0, false) {}
	~BitWriter ();

	inline size_t get_num_bits () const { return num_bits; }
	inline size_t get_curr_bit () const { return curr_bit; }
	inline size_t get_data_size () const { return data_size; }

	inline U8 *get_data_buffer () { return data; }

	inline void reset () { curr_bit = 0; }

	inline void set_data_buffer (U8 *buffer)
	{
		data = buffer;
		reset ();
	}

	bool write_bit (bool value);
	bool write_flag (bool flag);
	//bool write_string (char buffer[256]);

	template<typename T>
	bool write (T value)
	{
		size_t size = sizeof (T) * 8;

		if (curr_bit + size > num_bits || data == nullptr)
		{
			return false;
		}

		for (size_t n = size - 1; n >= 0; n--)
		{
			_write_bit ((value >> n) & 1U);
		}

		return true;
	}

private:
	/// Dangerous method with no sanity checks!!
	///
	/// This method assumes you have already done the appropriate sanity checking.
	inline void _write_bit (bool value);
};

#endif
