#include <stdint.h>

#ifndef ENCDEC_ENCRYPTION

#ifdef __cplusplus
extern "C"
{
#endif

  int32_t encrypt(const char* input_file[],
                  int32_t input_files_len,
                  const char* output_files[],
                  const char* const password);

#ifdef __cplusplus
}
#endif
#endif