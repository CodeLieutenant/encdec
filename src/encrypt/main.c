#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <hrtime.h>
#include <log.h>
#include <common.h>
#include <encrypt.h>

#include <argtable3.h>

#include "encryption.h"
#include "decryption.h"

#define NAME "encdec"
#define VERSION_MAJOR 1
#define VERSION_MINOR 0

struct arg_lit *help, *version, *hex, *ascii;
struct arg_file *o, *file;
struct arg_str *type, *log_level, *password;
struct arg_end* end;

int32_t
print_usage(void** argtable)
{
  printf("Usage: %s", NAME);
  arg_print_syntax(stdout, argtable, "\n");
  printf("EncDec Software.\n\n");
  arg_print_glossary(stdout, argtable, "  %-25s %s\n");
  return 0;
}

int32_t
print_version()
{
  printf("Version: %d.%d\n", VERSION_MAJOR, VERSION_MINOR);
  return 0;
}

int32_t
print_error()
{
  arg_print_errors(stdout, end, NAME);
  printf("Try '%s --help' for more information.\n", NAME);
  return 1;
}

void
set_log_level()
{
  const char* log;
  if (log_level->count == 0) {
    log = "info";
  } else {
    log = log_level->sval[0];
  }

  // Set LOG_INFO first as it will be most common
  if (stricmp(log, "info") == 0) {
    log_set_level(LOG_INFO);
  } else if (stricmp(log, "debug") == 0) {
    log_set_level(LOG_DEBUG);
  } else if (stricmp(log, "trace") == 0) {
    log_set_level(LOG_TRACE);
  } else if (stricmp(log, "error") == 0) {
    log_set_level(LOG_ERROR);
  } else if (stricmp(log, "fatal") == 0) {
    log_set_level(LOG_FATAL);
  }
}

int
main(int32_t argc, char* argv[])
{
  uint64_t start = hrtime();
  void* argtable[] = {
    help = arg_litn("h", "help", 0, 1, "display this help and exit"),
    version = arg_litn("v", "version", 0, 1, "display version info and exit"),
    hex = arg_litn(NULL, "dump", 0, 1, "Dump decrypted file as hex"),
    ascii = arg_litn(NULL, "print", 0, 1, "Dump decrypted file as ascii text"),
    type = arg_str1(NULL, NULL, "<str>", "Encrypt or Decrypt"),
    password =
      arg_str1("p", "password", "<password>", "Password to encrypt all files"),
    file = arg_filen(NULL, NULL, "<files>", 1, 100, "input files"),
    o = arg_filen("o", "output", "<output_files>", 0, 100, "output file"),
    log_level =
      arg_strn("l",
               "log",
               "<level>",
               0,
               1,
               "Application Log level (DEBUG, INFO, TRACE, ERROR, FATAL)"),
    end = arg_end(20),
  };

  int32_t status = arg_parse(argc, argv, argtable);

  if (help->count > 0) {
    status = print_usage(argtable);
    goto exit;
  }

  if (version->count > 0) {
    status = print_version();
    goto exit;
  }

  if (0 != status) {
    status = print_error();
    goto exit;
  }

  set_log_level();

  if (stricmp(type->sval[0], "encrypt") == 0) {
    if (file->count != o->count) {
      log_error("Number of input files has to be same as number of output "
                "files, %d - %d",
                file->count,
                o->count);
      status = 1;
      goto exit;
    }
    log_debug("Starting the file encryption");
    status =
      encrypt(file->filename, file->count, o->filename, password->sval[0]);
    log_debug("Encryption of file completed");
  } else if (stricmp(type->sval[0], "decrypt") == 0) {
    output output = NONE;
    if (file->count != o->count) {
      log_error("Number of input files has to be same as number of output "
                "files, %d - %d",
                file->count,
                o->count);
      status = 1;
      goto exit;
    }
    log_debug("Starting the file decryption");

    if (ascii->count) {
      output |= ASCII;
    }

    if (hex->count) {
      output |= HEX;
    }

    decrypt(
      file->filename, file->count, o->filename, password->sval[0], output);
  } else if (stricmp(type->sval[0], "hexdump") == 0) {
    printf("HexDUMP file\n");
  }
exit:
  arg_freetable(argtable, sizeof(argtable) / sizeof(argtable[0]));
  log_info("Total execution time: %ld ns", hrtime() - start);
  return status;
}
