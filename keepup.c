#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>

void monitor(char *process) {
  pid_t pid = fork();
  int status;

  switch (pid) {
    case -1:
      exit(1);
    case 0:
      execl("/bin/sh", "sh", "-c", process, NULL);
      exit(1);
    default:
      waitpid(pid, &status, 0);

      if (WIFEXITED(status) || WIFSIGNALED(status)) {
        monitor(process);
      }
  }
}

int main(int argc, char **argv) {
  char *process = argv[1];
  monitor(process);

  return 0;
}
