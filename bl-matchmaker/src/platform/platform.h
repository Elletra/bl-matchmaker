#ifndef _PLATFORM_PLATFORM_H
#define _PLATFORM_PLATFORM_H

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
#define OS_WIN32
#elif defined(__APPLE__) || defined(__MACH__)
#define OS_MACOSX
#elif defined(linux)
#define OS_LINUX
#else
#define OS_UNKNOWN
#endif

#if defined(OS_WIN32)
#include "win32/platform.h"
#elif defined(OS_LINUX)
#include "linux/platform.h"
#elif defined(OS_MACOSX)
// TODO
#error Mac OS X platform definitions not implemented!
#else
#error Unknown operating system!
#endif

#endif
