#include <stdio.h>

#include "assert.h"

void AssertFatal (bool test, const char *error)
{
	if (!test)
	{
		throw AssertException (error);
	}
}

void AssertWarn (bool test, const char *warn)
{
	if (!test)
	{
		printf (warn);
	}
}
