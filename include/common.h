#ifndef ENCDEC_COMMON

#include <stdint.h>
#include <stdio.h>
#include <malloc.h>
#include <sodium/crypto_secretstream_xchacha20poly1305.h>
#ifdef __cplusplus
extern "C"
{
#endif
#define ENCDEC_COMMON
#ifdef __WIN32
#define DIRECTORY_SEPARATOR '\\'
#define ENCDEC_PLATFORM_WINDOWS 1
#elif __linux__
#define DIRECTORY_SEPARATOR '/'
#define ENCDEC_PLATFORM_LINUX 1
#endif

#define FILE_SALT_BYTES_START 0
#define FILE_SALT_BYTES_LENGTH crypto_pwhash_SALTBYTES
#define DERIVED_PASSPHRASE_LENGTH crypto_secretstream_xchacha20poly1305_KEYBYTES
#define READ_BUFFER_SIZE 4098 // One page of memory
#define ENCRYPTED_LENGTH_BUFFER                                                \
  (READ_BUFFER_SIZE + crypto_secretstream_xchacha20poly1305_ABYTES)

#define ENCDEC_SUCCESS 0

#define ERROR_NOT_ENOUGH_BYTES -1
#define ERROR_INVALID_CHARACTER -2
#define ERROR_FILE_OPEN -1
#define ERROR_PASSWORD_DERIVATION_FAILED -2
#define ERROR_PASSWORD_DERIVATION_MEM_FAILED -3
#define ERROR_XCHACHA20_INVALID_HEADER -4
#define ERROR_XCHACHA20_ENCRYPTION -5
#define ERROR_PREMATURE_ENDING -6

#define HEX2BIN_OUTPUT_BUFFER_LENGTH(n) ((n - 1) >> 1)
#define BIN2HEX_OUTPUT_BUFFER_LENGTH(n) (n << 1) + 1
  typedef uint8_t byte;

  int32_t bin2hex(char* dst,
                  uint32_t dst_len,
                  const byte* const src,
                  uint32_t src_len);

  int32_t hex2bin(char* dst,
                  uint32_t dst_len,
                  const byte* const src,
                  uint32_t src_len);

  int32_t hexdump(FILE* output, const char* const hex);

  int32_t generate_salt(byte* out_salt);

  int32_t password_derivation(byte* out,
                              byte* out_salt,
                              const char* const password);
  void basename(char* out, uint32_t out_len, const char* const path);
#ifdef __cplusplus
}
#endif
#endif
