#include <stdint.h>
#ifndef ENCDEC_DECRYPTION

#ifdef __cplusplus
extern "C"
{
#endif
  int32_t decrypt(const char* input_file[],
                  const int32_t input_files_len,
                  const char* output_files[],
                  const char* const password,
                  const output o);

#ifdef __cplusplus
}
#endif
#endif
