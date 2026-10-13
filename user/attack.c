#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#define LEN (256 * 1024)
#define PREFIX "Here it is: "
#define PREFIX_LEN (sizeof(PREFIX) - 1)

static int
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

  for (int i = 0; i + PREFIX_LEN < LEN; i++) {
    int found = 1;
    for (int j = 0; j < PREFIX_LEN; j++) {
      if (mem[i + j] != PREFIX[j]) {
        found = 0;
        break;
      }
    }
    if (found) {
      int j = i + PREFIX_LEN;
      int start = j;
      while (j < LEN && isalnum_check(mem[j]))
        j++;
      if (j - start >= 4) {
        for (int k = start; k < j; k++)
          printf("%c", mem[k]);
        printf("\n");
        break;
      }
    }
  }

  exit(0);
}
