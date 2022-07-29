#ifndef _BITREADER_BITREADER_H
#define _BITREADER_BITREADER_H

#include <stdio.h>
#include <stdlib.h>

#include "types.h"

class BitReader
{
private:
	U8 *data;
	size_t data_size;
	size_t num_bits;
	size_t curr_bit;

	bool auto_free_data;

public:
	BitReader (U8 *data, size_t data_size, bool auto_free_data = false);
	BitReader () : BitReader (nullptr, 0) {}
	~BitReader ();

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

	bool read_bit (U8 &out_value);
	bool read_flag ();

	bool read (U8 &out);
	bool read (U16 &out);
	bool read (U32 &out);
	bool read (S8 &out);
	bool read (S16 &out);
	bool read (S32 &out);

	void read_string (char buffer[256]);

private:
	/// Dangerous method with no sanity checks!!
	///
	/// This method assumes you have already done the appropriate sanity checking.
	inline U8 _read_bit ();

	template<typename T>
	bool _read (T &out_num)
	{
		size_t size = sizeof (T) * 8;

		if (curr_bit + size > num_bits || data == nullptr)
		{
			return false;
		}

		T num = 0;

		for (U32 i = 0; i < size; i++)
		{
			num = (num << 1) | _read_bit ();
		}

		out_num = num;

		return true;
	}
};

#endif
