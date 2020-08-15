#include <stdio.h>
#include "../../include/hrtime.h"
#include <sodium.h>

int
main()
{
  uint64_t start = hrtime();
  for (int i = 0; i < 10000; i++) {
  }

  uint64_t end = hrtime() - start;
  printf("Hello, World! %ld\n", start);

  return 0;
}
