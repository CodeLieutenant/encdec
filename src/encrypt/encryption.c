#include <common.h>
#include <encrypt.h>
#include <hrtime.h>
#include <log.h>

#include "encrypt.h"

static void
handle_encryption_error(int32_t error_code)
{
  switch (error_code) {
    case ERROR_XCHACHA20_ENCRYPTION:
      log_error("Error while encrypting file content");
      break;
    case ERROR_FILE_OPEN:
      log_error("Error while opening files");
      break;
    case ERROR_PASSWORD_DERIVATION_FAILED:
      log_error("Error while generating encryption key!");
      break;
    case ERROR_XCHACHA20_INVALID_HEADER:
      log_error("Error while generating XChaCha20Poly1305 Header!");
      break;
  }
}

int32_t
encrypt(const char* input_files[],
        int32_t input_files_len,
        const char* output_files[],
        const char* const password)
{
  int32_t status = 0;
  uint64_t start, diff, sum = 0;

  for (int32_t i = 0; i < input_files_len; i++) {
    start = hrtime();
    status = encrypt_file_password(input_files[i], output_files[i], password);
    if (0 != status) {
      handle_encryption_error(status);
      return status;
    }
    diff = hrtime() - start;
    log_info("Encrypting file: %s took %ld ns", input_files[i], diff);
    sum += diff;
  }

  log_info("Average encryption time: %ld ns", sum / (uint64_t)input_files_len);

  return status;
}