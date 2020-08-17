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
  int32_t status = 0;
  log_set_level(LOG_DEBUG);
  status =
    encrypt_file_password("test.txt", "text.enc", "MySuperSecretPassword");

  if (status != ENCDEC_SUCCESS) {
    log_error("Error in encryption %d", status);
    return 1;
  }

  log_info("File %s successfully encrypted into %s", "test.txt", "text.enc");

  status =
    decrytpt_file_password("text.enc", "dec.txt", "MySuperSecretPassword");

  if (status != ENCDEC_SUCCESS) {
    log_error("Error in decryption %d", status);
    return 2;
  }

  return 0;
}
