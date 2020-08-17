/**
 * The inspiration for this function is taken from PHP source code
 * https://github.com/php/php-src/blob/master/ext/standard/hrtime.c
 */
#include "../include/hrtime.h"
#include "../include/common.h"

#ifdef ENCDEC_PLATFORM_WINDOWS
#include <windows.h>
#include <profileapi.h>
#elif ENCDEC_PLATFORM_LINUX
#include <unistd.h>
#include <time.h>
#endif

#define NANO_IN_SEC 1000000000
uint64_t
hrtime()
{
#ifdef ENCDEC_PLATFORM_WINDOWS
  LARGE_INTEGER lt = { 0 };
  if (!QueryPerformanceFrequency(&lt) || 0 == lt.QuadPart) {
    return 0;
  }
  double time_scale = (double)NANO_IN_SEC / (uint64_t)lt.QuadPart;

  return (uint64_t)((uint64_t)lt.QuadPart * time_scale);
#elif ENCDEC_PLATFORM_LINUX
#if !_POSIX_MONOTONIC_CLOCK
#ifdef _SC_MONOTONIC_CLOCK
  return sysconf(_SC_MONOTONIC_CLOCK);
#endif
#endif
#endif
  return 0;
}

uint64_t
memory_usage()
{
  return 0;
}