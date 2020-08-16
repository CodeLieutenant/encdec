/*
 * High resolution timing API
 */
#ifndef ENCDEC_ENCRYPT

#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif
#define ENCDEC_ENCRYPT
  int32_t encrypt_file_password(const char const* file,
                                const char const* out_file,
                                const char const* passphrase);

#ifdef __cplusplus
}
#endif
#endif