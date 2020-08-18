#include <common.h>
#include <log.h>
#include <hrtime.h>
#include <encrypt.h>

static void
handle_encryption_error(const int32_t error_code)
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
  default:
      log_fatal("Unknow error");
      break;
  }
}

int32_t
decrypt(const char* input_files[],
        const int32_t input_files_len,
        const char* output_files[],
        const char* const password,
        const output o)
{
  int32_t status = 0;
  uint64_t sum = 0, memory_sum = 0;

  for (int32_t i = 0; i < input_files_len; i++) {
	const uint64_t start = hrtime();
    status =
      decrypt_file_password(input_files[i], output_files[i], password, o);
    if (ENCDEC_SUCCESS != status) {
      handle_encryption_error(status);
      return status;
    }
	const uint64_t diff = hrtime() - start;
    log_info("Decrypting file: %s took %llu ns and %llu Bytes", input_files[i], diff, memory_usage());
    sum += diff;
    memory_sum += memory_usage();
  }

  log_info("Average decryption time: %llu ns", sum / (uint64_t)input_files_len);
  const uint64_t memory_avg = memory_sum / input_files_len;
  log_info("Average memory usage for decryption: %llu Bytes (%lf MiB)", memory_avg, (double)memory_avg / MIBI_BYTE);

  return status;
}