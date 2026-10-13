#include "kernel/types.h"
#include "user/user.h"
#include "kernel/fcntl.h"

void memdump(char *fmt, char *data, int len);

int
main(int argc, char *argv[])
{
  if (argc == 1) {
    printf("Example 1:\n");
    int a[2] = {61810, 2026};
    memdump("ii", (char *)a, sizeof(a));

    printf("Example 2:\n");
    memdump("S", "a string", sizeof("a string"));

    printf("Example 3:\n");
    char *s = "another";
    memdump("s", (char *)&s, sizeof(s));

    struct sss {
      char *ptr;
      int num1;
      short num2;
      char byte;
      char bytes[8];
    } example;

    example.ptr = "hello";
    example.num1 = 1819438967;
    example.num2 = 100;
    example.byte = 'z';
    strcpy(example.bytes, "xyzzy");

    printf("Example 4:\n");
    memdump("pihcS", (char *)&example, sizeof(example));

    printf("Example 5:\n");
    memdump("sccccc", (char *)&example, sizeof(example));
  } else if (argc == 2) {
    // format in argv[1], up to 512 bytes of data from standard input.
    char data[512];
    int n = 0;
    memset(data, '\0', sizeof(data));
    while (n < sizeof(data)) {
      int nn = read(0, data + n, sizeof(data) - n);
      if (nn <= 0)
        break;
      n += nn;
    }
    memdump(argv[1], data, n);
  } else {
    printf("Usage: memdump [format]\n");
    exit(1);
  }
  exit(0);
}

void
memdump(char *fmt, char *data, int len)
{
  int offset = 0;

  for (int i = 0; fmt[i] != '\0'; i++) {
    char spec = fmt[i];

    switch (spec) {
    case 'i': {
      if (len - offset < 4) {
        printf("memdump: not enough data for '%c'\n", spec);
        return;
      }
      int val = *(int *)(data + offset);
      printf("%d\n", val);
      offset += 4;
      break;
    }

    case 'p': {
      if (len - offset < 8) {
        printf("memdump: not enough data for '%c'\n", spec);
        return;
      }
      uint64 val = *(uint64 *)(data + offset);
      printf("%p\n", (void *)val);
      offset += 8;
      break;
    }

    case 'h': {
      if (len - offset < 2) {
        printf("memdump: not enough data for '%c'\n", spec);
        return;
      }
      short val = *(short *)(data + offset);
      printf("%d\n", val);
      offset += 2;
      break;
    }

    case 'c': {
      if (len - offset < 1) {
        printf("memdump: not enough data for '%c'\n", spec);
        return;
      }
      char val = *(data + offset);
      printf("%c\n", val);
      offset += 1;
      break;
    }

    case 's': {
      if (len - offset < 8) {
        printf("memdump: not enough data for '%c'\n", spec);
        return;
      }
      char *str_ptr = *(char **)(data + offset);
      printf("%s\n", str_ptr);
      offset += 8;
      break;
    }

    case 'S': {
      // Print until '\0' or end of valid buffer (len)
      while (offset < len && data[offset] != '\0') {
        printf("%c", data[offset]);
        offset++;
      }
      printf("\n");
      // If stopped at a null terminator, consume it
      if (offset < len && data[offset] == '\0') {
        offset++;
      }
      break;
    }

    default:
      // Ignore or handle unknown format characters
      break;
    }
  }
}
