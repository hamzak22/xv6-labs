#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#define LEN 65536   // adjust based on how much secret.c allocates

int
isalnum_check(char c)
{
  return (c >= '0' && c <= '9') ||
         (c >= 'a' && c <= 'z') ||
         (c >= 'A' && c <= 'Z');
}

int
main(int argc, char *argv[])
{
  char *mem = sbrk(LEN);

  for (int i = 0; i < LEN; i++) {
    // Secrets are guaranteed to be only digits and letters.
    if (isalnum_check(mem[i])) {
      int j = i;
      while (j < LEN && isalnum_check(mem[j]))
        j++;
      if (j - i >= 4) {  // ignore tiny coincidental runs
        for (int k = i; k < j; k++)
          printf("%c", mem[k]);
        printf("\n");
      }
      i = j;
    }
  }

  exit(0);
}

