#include "dump.h"
#include <common.h>
#include <log.h>
#include <string.h>
#define HEX_BUFFER_SIZE (READ_BUFFER_SIZE * 2 + 1)

#include "hrtime.h"

ENCDEC_INLINE static inline int32_t open_file(FILE **out, const char *const file,
                                const char *const mode) {
#ifdef _MSC_VER
  if (0 != fopen_s(out, file, mode)) {
    log_debug("Error while opening %s", file);
    return ERROR_FILE_OPEN;
  }
#elif
  *out = fopen(file, mode);
  if (!*out) {
    log_debug("Error while opening %s", file);
    return ERROR_FILE_OPEN;
  }
#endif
  return ENCDEC_SUCCESS;
}

ENCDEC_INLINE static inline int32_t
output_file(FILE *file, byte *const buffer,
                                  byte *const hex, const output o) {
  int32_t eof = 0;
  do {
    const size_t read = fread(buffer, sizeof(byte), READ_BUFFER_SIZE, file);
    eof = feof(file);

    if (o & ASCII) {
      printf("%s", buffer);
    } else if (o & HEX) {
      memset(hex, 0, HEX_BUFFER_SIZE);
      bin2hex(hex, HEX_BUFFER_SIZE, buffer, read);
      hexdump(stdout, hex);
    }
  } while (!eof);
  return ENCDEC_SUCCESS;
}

int32_t dump(const char *files[], const int32_t files_len, const output o) {
  byte buffer[READ_BUFFER_SIZE], hex[HEX_BUFFER_SIZE];
  int32_t status;
  const uint64_t global_time = hrtime();
  for (int32_t i = 0; i < files_len; i++) {
    const uint64_t time_start = hrtime();
    log_info("PRINTING %s", files[i]);
    printf("-------------------------------------------------\n");
    FILE *file = NULL;
    if (o & ASCII) {
      status = open_file(&file, files[i], "r+");
      if (ENCDEC_SUCCESS != status) {
        return status;
      }
    } else if (o & HEX) {
      status = open_file(&file, files[i], "rb+");
      if (ENCDEC_SUCCESS != status) {
        return status;
      }
    }

    if (!file) {
      return ERROR_FILE_OPEN;
    }

    output_file(file, buffer, hex, o);
    fclose(file);
    printf("-------------------------------------------------\n");
    log_info("Printing finished (%s), Time: %llu ns", files[i],
             hrtime() - time_start);
    printf("\n\n");
  }

  log_info("Priting of files took %llu", hrtime() - global_time);
  return ENCDEC_SUCCESS;
}

#undef HEX_BUFFER_SIZE
