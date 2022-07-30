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

	bool read_bits (U32 bit_count, void *out_ptr);
	S32 read_int (U8 bit_count);

	U8 read_u8 (U8 bit_count = BITS_U8);
	U16 read_u16 (U8 bit_count = BITS_U16);
	U32 read_u32 (U8 bit_count = BITS_U32);
	S8 read_s8 (U8 bit_count = BITS_S8);
	S16 read_s16 (U8 bit_count = BITS_S16);
	S32 read_s32 (U8 bit_count = BITS_S32);

	void read_string (char buffer[256]);

private:
	/// Dangerous method with no sanity checks!!
	///
	/// This method assumes you have already done the appropriate sanity checking.
	inline U8 _read_bit ();
};

#endif
