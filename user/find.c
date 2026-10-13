#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"
#include "kernel/fcntl.h"
#include "kernel/param.h"

static char*
fmtname(char *path)
{
  char *p;

  for (p = path + strlen(path); p >= path && *p != '/'; p--)
    ;
  p++;

  return p;
}


static void
match_action(char *path, char **exec_argv, int exec_argc)
{
  if (exec_argv == 0) {
    printf("%s\n", path);
    return;
  }

  int pid = fork();
  if (pid < 0) {
    fprintf(2, "find: fork failed\n");
    return;
  }

  if (pid == 0) {

    char *cmd_argv[MAXARG];
    for (int i = 0; i < exec_argc; i++) {
      cmd_argv[i] = exec_argv[i];
    }
    cmd_argv[exec_argc] = path;
    cmd_argv[exec_argc + 1] = 0;

    exec(cmd_argv[0], cmd_argv);
    fprintf(2, "find: exec %s failed\n", cmd_argv[0]);
    exit(1);
  } else {
    wait(0);
  }
}

void
find(char *path, char *target, char **exec_argv, int exec_argc)
{
  char buf[512], *p;
  int fd;
  struct dirent de;
  struct stat st;

  if ((fd = open(path, O_RDONLY)) < 0) {
    fprintf(2, "find: cannot open %s\n", path);
    return;
  }

  if (fstat(fd, &st) < 0) {
    fprintf(2, "find: cannot stat %s\n", path);
    close(fd);
    return;
  }

  switch (st.type) {
  case T_DEVICE:
  case T_FILE:
    if (strcmp(fmtname(path), target) == 0) {
      match_action(path, exec_argv, exec_argc);
    }
    break;

  case T_DIR:
    if (strlen(path) + 1 + DIRSIZ + 1 > sizeof(buf)) {
      fprintf(2, "find: path too long\n");
      break;
    }

    strcpy(buf, path);
    p = buf + strlen(buf);
    *p++ = '/';

    while (read(fd, &de, sizeof(de)) == sizeof(de)) {
      if (de.inum == 0)
        continue;

      if (strcmp(de.name, ".") == 0 || strcmp(de.name, "..") == 0)
        continue;

      memmove(p, de.name, DIRSIZ);
      p[DIRSIZ] = 0;

      if (strcmp(de.name, target) == 0) {
        match_action(buf, exec_argv, exec_argc);
      }

      struct stat entry_st;
      if (stat(buf, &entry_st) < 0) {
        fprintf(2, "find: cannot stat %s\n", buf);
        continue;
      }

      if (entry_st.type == T_DIR) {
        find(buf, target, exec_argv, exec_argc);
      }
    }
    break;
  }

  close(fd);
}

int
main(int argc, char *argv[])
{
  if (argc == 3) {

    find(argv[1], argv[2], 0, 0);
  } else if (argc >= 5 && strcmp(argv[3], "-exec") == 0) {
    
    char **exec_argv = &argv[4];
    int exec_argc = argc - 4;
    find(argv[1], argv[2], exec_argv, exec_argc);
  } else {
    fprintf(2, "Usage: find <path> <filename> [-exec <cmd> ...]\n");
    exit(1);
  }

  exit(0);
}