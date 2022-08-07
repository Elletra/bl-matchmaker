#include <chrono>
#include <random>

#include "rng.h"

#define RNG_DIST_MIN 0x10000000
#define RNG_DIST_MAX 0xFFFFFFFF

namespace RNG
{
	typedef std::uniform_int_distribution<std::mt19937::result_type> Distribution;

	static std::mt19937 rng;
	static Distribution dist;

	void init ()
	{
		/* I realize this may not the most cryptographically secure, but it's a fucking
		   Blockland matchmaker server... not a bank. */

		std::random_device dev;

		rng = (std::mt19937 (dev ()));
		dist = Distribution (RNG_DIST_MIN, RNG_DIST_MAX);
	}

	U32 generate ()
	{
		return dist (rng);
	}
};
