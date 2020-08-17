/**
 * The inspiration for this function is taken from PHP source code
 * https://github.com/php/php-src/blob/master/ext/standard/hrtime.c
 */
#include <time.h>
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
  LARGE_INTEGER performanceCounter = { 0 };
  LARGE_INTEGER performanceFrequency = { 0 };


  if (!QueryPerformanceCounter(&performanceCounter) || 0 == performanceCounter.QuadPart) {
    return 0;
  }

  if (!QueryPerformanceFrequency(&performanceFrequency) || 0 == performanceFrequency.QuadPart) {
      return 0;
  }

  double time_scale = (double)performanceCounter.QuadPart / (double)performanceFrequency.QuadPart;

  uint64_t time =  (NANO_IN_SEC * time_scale);
  return time;
#elif ENCDEC_PLATFORM_LINUX
#if !_POSIX_MONOTONIC_CLOCK
  struct timespec spec;
  clock_gettime(CLOCK_MONOTONIC, &spec);

  return (uint64_t)spec.tv_nsec;
#endif
#endif
  return 0;
}

uint64_t
memory_usage()
{
  return 0;
}