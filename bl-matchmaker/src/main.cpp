#include <stdio.h>
#include <stdlib.h>

#include "bitReader/BitReader.h"
#include "bitWriter/BitWriter.h"

#include "types.h"

#define DATA_SIZE 4

int main (int argc, const char **argv)
{
	U8 data[DATA_SIZE] = { 0b11110001, 0b10110100, 0b10101010, 0b11111011 };
	BitReader reader = BitReader (data, DATA_SIZE, false);

	/*U8 bit = 0;

	while (reader.get_curr_bit () < reader.get_num_bits ())
	{
		reader.read_bit (bit);
		printf ("%u%s", bit, (reader.get_curr_bit () & 7) ? "" : "\n");
	}*/

	S8 num = 0;
	reader.read (num);

	printf ("%d\n", num);

	return 0;
}
