/*
 * High resolution timing API
 */
#ifndef ENCDEC_HRTIME

#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif
#define ENCDEC_HRTIME
  /**
   * Gets current time in nanoseconds
   * If platform is not supported 0 is returned
   */
  uint64_t hrtime();

#ifdef __cplusplus
}
#endif
#endif