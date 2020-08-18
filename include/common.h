#include <stdint.h>
#include <stdio.h>

#ifndef ENCDEC_COMMON

#ifdef __cplusplus
extern "C" {
#endif
#define ENCDEC_COMMON
#if defined(__WIN32) || defined(_WIN32) || defined(_WIN64)
#define DIRECTORY_SEPARATOR '\\'
#define ENCDEC_PLATFORM_WINDOWS 1
#elif __linux__
#define DIRECTORY_SEPARATOR '/'
#define ENCDEC_PLATFORM_LINUX 1
#endif

#ifdef _MSC_VER
#define ENCDEC_INLINE __forceinline
#define stricmp _stricmp
#define fread(buffer, _ElementSize, _ElementCount, _Stream)                    \
  fread_s(buffer, _ElementCount, _ElementSize, _ElementCount, _Stream)

#elif defined(__GCC__)
#define ENCDEC_INLINE __attribute__((always_inline))
#else
#define ENCDEC_INLINE 
#endif
#define MIBI_BYTE (double)1048576

#define FILE_SALT_BYTES_START 0
#define FILE_SALT_BYTES_LENGTH crypto_pwhash_SALTBYTES
#define DERIVED_PASSPHRASE_LENGTH crypto_secretstream_xchacha20poly1305_KEYBYTES
#define READ_BUFFER_SIZE 4098 // One page of memory
#define ENCRYPTED_LENGTH_BUFFER                                                \
  (READ_BUFFER_SIZE + crypto_secretstream_xchacha20poly1305_ABYTES)

#define ENCDEC_SUCCESS 0
#define ERROR_NOT_ENOUGH_BYTES (-1)
#define ERROR_INVALID_CHARACTER (-2)
#define ERROR_FILE_OPEN (-3)
#define ERROR_PASSWORD_DERIVATION_FAILED (-4)
#define ERROR_PASSWORD_DERIVATION_MEM_FAILED (-5)
#define ERROR_XCHACHA20_INVALID_HEADER (-6)
#define ERROR_XCHACHA20_ENCRYPTION (-7)
#define ERROR_PREMATURE_ENDING (-8)
#define ERROR_XCHACHA20_DECRYPTION (-9)
#define ERROR_MEMORY_ALLOCATION (-10)

#define HEX2BIN_OUTPUT_BUFFER_LENGTH(n) (((n)-1) >> 1)
#define BIN2HEX_OUTPUT_BUFFER_LENGTH(n) (((n) << 1) + 1)

typedef uint8_t byte;

typedef enum { NONE = 0, ASCII = 1, HEX = 1 << 2 } output;

int32_t bin2hex(char *const dst, const uint32_t dst_len, const byte *const src,
                const uint32_t src_len);

int32_t hex2bin(char *dst, uint32_t dst_len, const byte *const src,
                uint32_t src_len);

uint32_t hexdump(const FILE *const output, const char *const hex);

int32_t generate_salt(byte *const out_salt);

int32_t password_derivation(byte *out, byte *out_salt,
                            const char *const password);

#ifndef _MSC_VER
ENCDEC_INLINE int32_t stricmp(const char *const p1, const char *const p2);
#endif
#ifdef __cplusplus
}
#endif
#endif
