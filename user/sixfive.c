#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fcntl.h"

static int
is_separator(char c)
{
  return (c == ' '  || c == '-'  || c == '\r' || c == '\t' ||
          c == '\n' || c == '.'  || c == '/'  || c == ',');
}


static int is_digit(char c)
{
  return (c >= '0' && c <= '9');
}


void
process_fd(int fd)
{
  char buf[512];
  int n;

  int in_token = 0;       
  int is_valid = 1;       
  long long val = 0;      

  while ((n = read(fd, buf, sizeof(buf))) > 0) {
    for (int i = 0; i < n; i++) {
      char c = buf[i];

      if (is_separator(c)) {
        if (in_token) {
          
          if (is_valid && (val % 5 == 0 || val % 6 == 0)) {
            printf("%d\n", (int)val);
          }
          in_token = 0;
          is_valid = 1;
          val = 0;
        }
      } else if (is_digit(c)) {
        if (!in_token) {
          in_token = 1;
          is_valid = 1;
          val = c - '0';
        } else if (is_valid) {
          val = val * 10 + (c - '0');
        }
      } else {
        
        in_token = 1;
        is_valid = 0; 
      }
    }
  }


  if (in_token && is_valid && (val % 5 == 0 || val % 6 == 0)) {
    printf("%d\n", (int)val);
  }
}

int
main(int argc, char *argv[])
{
  if (argc <= 1) {
    process_fd(0);
    exit(0);
  }

  for (int i = 1; i < argc; i++) {
    int fd = open(argv[i], O_RDONLY);
    if (fd < 0) {
      fprintf(2, "sixfive: cannot open %s\n", argv[i]);
      continue;
    }
    process_fd(fd);
    close(fd);
  }

  exit(0);
}
