#include <stdint.h>

#include <encrypt.h>

#ifndef ENCDEC_DECRYPTION

#ifdef __cplusplus
extern "C"
{
#endif
  int32_t decrypt(const char* input_file[],
                  int32_t input_files_len,
                  const char* output_files[],
                  const char* const password,
                  output o);

#ifdef __cplusplus
}
#endif
#endif