#ifndef _UTIL_ASSERT_H
#define _UTIL_ASSERT_H

#include <exception>

struct AssertException : public std::exception
{
	AssertException (const char *what) : std::exception (what) {}
};

void AssertFatal (bool test, const char *error);
void AssertWarn (bool test, const char *warn);

#endif