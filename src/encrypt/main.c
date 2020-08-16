#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <hrtime.h>
#include <log.h>
#include <common.h>
#include <encrypt.h>

int
main()
{
  log_set_level(LOG_DEBUG);
  int32_t status =
    encrypt_file_password("test.txt", "text.enc", "MySuperSecretPassword");
  log_info("File %s successfully encrypted into %s", "test.txt", "text.enc");
  return 0;
}
