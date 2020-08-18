#include <stdio.h>
#include <string.h>
#include <sodium.h>
#include <sodium/crypto_pwhash.h>
#include <sodium/randombytes.h>

#include "../include/common.h"
#include "../include/encrypt.h"
#include "../include/log.h"

typedef crypto_secretstream_xchacha20poly1305_state state;
typedef int32_t (*read_callback)(state* st,
                                 FILE* out_file,
                                 byte* buffer,
                                 byte* out,
                                 uint64_t read,
                                 int32_t tag,
                                 int32_t is_eof);

static int32_t
encrypt(state* st,
        FILE* out_file,
        byte* buffer,
        byte* out,
        uint64_t read,
        int32_t tag,
        int32_t is_eof)
{
  int32_t status = 0;
  unsigned long long out_len;
  status = crypto_secretstream_xchacha20poly1305_push(
    st, out, &out_len, buffer, read, NULL, 0, tag);
  if (0 != status) {
    return ERROR_XCHACHA20_ENCRYPTION;
  }
  fwrite(out, sizeof(byte), (size_t)out_len, out_file);
  fflush(out_file);
  return status;
}

static int32_t
decrypt(state* st,
        FILE* out_file,
        byte* buffer,
        byte* out,
        uint64_t read,
        int32_t tag,
        int32_t is_eof)
{
  int32_t status = 0;
  uint8_t t;
  unsigned long long out_len;
  status = crypto_secretstream_xchacha20poly1305_pull(
    st, out, &out_len, &t, buffer, read, NULL, 0);
  if (0 != status) {
    log_debug("Decryption failed, stream cipher failed, %d: INVALID HEADER",
              status);
    return ERROR_XCHACHA20_DECRYPTION;
  }

  if (t == crypto_secretstream_xchacha20poly1305_TAG_FINAL && !is_eof) {
    log_debug("Decryption failed, end of file not reached");
    return ERROR_PREMATURE_ENDING;
  }

  fwrite(out, sizeof(byte), (size_t)out_len, out_file);
  fflush(out_file);
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
  *in_file = fopen(file, "rb+");
  if (!*in_file) {
    log_debug("Error while opening: %s", file);
    return ERROR_FILE_OPEN;
  }

  *out_file_p = fopen(out_file, "wb+");
  if (!*out_file_p) {
    log_debug("Error while opening: %s", out_file);
    return ERROR_FILE_OPEN;
  }
  status = sodium_init();
  if (-1 == status) {
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
  fflush(*out_file_p);
  return status;
}

int32_t
encrypt_file_password(const char* const file,
                      const char* const out_file,
                      const char* const passphrase)
{
  FILE *in_file = NULL, *out_file_p = NULL;
  int32_t status = 0;
  state st;
  byte salt[FILE_SALT_BYTES_LENGTH], buffer[READ_BUFFER_SIZE],
    enc[ENCRYPTED_LENGTH_BUFFER];
  memset(buffer, 0, sizeof(buffer));
  memset(enc, 0, sizeof(enc));

  generate_salt(salt);
  status = init(&st, &in_file, &out_file_p, salt, passphrase, file, out_file);

  if (0 != status) {
    return status;
  }

  size_t read;
  int32_t is_eof = 0, tag = 0;
  int64_t enc_len = 0;
  do {
    read = fread(buffer, sizeof(byte), READ_BUFFER_SIZE, in_file);
    is_eof = feof(in_file);
    tag = is_eof ? crypto_secretstream_xchacha20poly1305_TAG_FINAL : 0;
    status = encrypt(&st, out_file_p, buffer, enc, read, tag, is_eof);
    if (status != 0) {
      return status;
    }
  } while (!is_eof);

  if (0 != status) {
    return ERROR_XCHACHA20_ENCRYPTION;
  }

  fflush(out_file_p);
  fclose(in_file);
  fclose(out_file_p);
  return ENCDEC_SUCCESS;
}

int32_t
decrypt_file_password(const char* const file,
                      const char* const out_file,
                      const char* const passphrase,
                      output o)
{
  FILE *in_file = NULL, *out_file_p = NULL;
  int32_t status = 0;
  state st;
  byte salt[FILE_SALT_BYTES_LENGTH], buffer[ENCRYPTED_LENGTH_BUFFER],
    dec[READ_BUFFER_SIZE],
    header[crypto_secretstream_xchacha20poly1305_HEADERBYTES],
    key[DERIVED_PASSPHRASE_LENGTH];
  char* print = malloc(READ_BUFFER_SIZE * 2 + 1);

  in_file = fopen(file, "rb+");
  if (!in_file) {
    log_debug("Error while opening: %s", file);
    return ERROR_FILE_OPEN;
  }

  out_file_p = fopen(out_file, "wb+");
  if (!out_file_p) {
    log_debug("Error while opening: %s", out_file);
    return ERROR_FILE_OPEN;
  }

  fread(salt, sizeof(byte), sizeof(salt), in_file);
  fread(header, sizeof(byte), sizeof(header), in_file);

  status = sodium_init();
  if (-1 == status) {
    log_debug("Error while setting up libsodium");
    return status;
  }

  status = password_derivation(key, salt, passphrase);
  if (0 != status) {
    log_debug("Error while generating key, %s");
    return ERROR_PASSWORD_DERIVATION_FAILED;
  }
  status = crypto_secretstream_xchacha20poly1305_init_pull(&st, header, key);

  if (0 != status) {
    log_debug("Error while initializing xchacha state");
    return ERROR_XCHACHA20_INVALID_HEADER;
  }

  output print_out = o;
  size_t read;
  int32_t is_eof = 0, tag = 0;
  int64_t enc_len = 0;
  do {
    read = fread(&buffer, sizeof(byte), ENCRYPTED_LENGTH_BUFFER, in_file);
    is_eof = feof(in_file);
    tag = is_eof ? crypto_secretstream_xchacha20poly1305_TAG_FINAL : 0;
    status = decrypt(&st, out_file_p, buffer, dec, read, tag, is_eof);
    if (status != 0) {
      return status;
    }
    if (o & HEX) {
      memset(print, 0, sizeof(print));
      bin2hex(print, sizeof(print), dec, read);
      hexdump(stdout, print);
    }

    if (o & ASCII) {
      printf("%s", dec);
    }

  } while (!is_eof);

  if (0 != status) {
    log_debug("Error while decrypting");
    return ERROR_XCHACHA20_INVALID_HEADER;
  }

  free(print);
  return status;
}