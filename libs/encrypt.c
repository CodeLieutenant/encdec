#include <stdio.h>
#include <string.h>
#include <sodium.h>
#include <sodium/crypto_pwhash.h>
#include <sodium/randombytes.h>

#include "../include/common.h"
#include "../include/encrypt.h"
#define LOG_USE_COLORS
#include "../include/log.h"

#define READ_FILE(read_len, out_len, cb)                                       \
  size_t read;                                                                 \
  int32_t is_eof = 0, tag = 0;                                                 \
  int64_t enc_len = 0;                                                         \
  do {                                                                         \
    memset(buffer, 0, read_len);                                               \
    memset(enc, 0, out_len);                                                   \
    read = fread(&buffer, sizeof(byte), read_len, in_file);                    \
    is_eof = feof(in_file);                                                    \
    tag = is_eof ? crypto_secretstream_xchacha20poly1305_TAG_FINAL : 0;        \
    status = cb(&st, out_file_p, buffer, enc, read, tag);                      \
    if (status != 0) {                                                         \
      return status;                                                           \
    }                                                                          \
  } while (!is_eof);

typedef crypto_secretstream_xchacha20poly1305_state state;
typedef int32_t (*read_callback)(state* st,
                                 FILE* out_file,
                                 byte* buffer,
                                 byte* out,
                                 uint32_t read,
                                 int32_t tag);

static int32_t
encrypt(state* st,
        FILE* out_file,
        byte* buffer,
        byte* out,
        uint32_t read,
        int32_t tag)
{
  int32_t status = 0;
  unsigned long long out_len;
  status = crypto_secretstream_xchacha20poly1305_push(
    st, out, &out_len, buffer, read, NULL, 0, tag);
  if (0 != status) {
    return ERROR_XCHACHA20_ENCRYPTION;
  }
  fwrite(out, sizeof(byte), (size_t)out_len, out_file);

  return status;
}

static int32_t
init(state* st,
     FILE** in_file,
     FILE** out_file_p,
     byte* salt,
     const char* const passphrase,
     const char* const file,
     const char* const out_file)
{
  int32_t status = 0;
  byte header[crypto_secretstream_xchacha20poly1305_HEADERBYTES],
    key[DERIVED_PASSPHRASE_LENGTH];
  *in_file = fopen(file, "r+");
  if (!*in_file) {
    log_debug("Error while opening: %s", file);
    return ERROR_FILE_OPEN;
  }

  *out_file_p = fopen(out_file, "w+");
  if (!*out_file_p) {
    log_debug("Error while opening: %s", out_file);
    return ERROR_FILE_OPEN;
  }
  status = sodium_init();
  if (0 != status) {
    log_debug("Error while setting up libsodium");
    return status;
  }

  status = password_derivation(key, salt, passphrase);
  if (0 != status) {
    log_debug("Error while generating key, %s");
    return ERROR_PASSWORD_DERIVATION_FAILED;
  }
  status = crypto_secretstream_xchacha20poly1305_init_push(st, header, key);

  if (0 != status) {
    log_debug("Error while encrypting");
    return ERROR_XCHACHA20_INVALID_HEADER;
  }
  // Metadata
  fwrite(salt, sizeof(char), FILE_SALT_BYTES_LENGTH, *out_file_p);
  fwrite(header, 1, sizeof header, *out_file_p);
  return status;
}

int32_t
encrypt_file_password(const char const* file,
                      const char const* out_file,
                      const char const* passphrase)
{
  FILE *in_file = NULL, *out_file_p = NULL;
  int32_t status = 0;
  state st;
  byte salt[FILE_SALT_BYTES_LENGTH], buffer[READ_BUFFER_SIZE],
    enc[ENCRYPTED_LENGTH_BUFFER];
  generate_salt(salt);
  status = init(&st, &in_file, &out_file_p, salt, passphrase, file, out_file);

  if (status != 0) {
    return status;
  }

  READ_FILE(READ_BUFFER_SIZE, ENCRYPTED_LENGTH_BUFFER, encrypt);

  if (0 != status) {
    return ERROR_XCHACHA20_ENCRYPTION;
  }

  fclose(in_file);
  fclose(out_file_p);
  return ENCDEC_SUCCESS;
}