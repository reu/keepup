#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>

void monitor(char *process) {
  pid_t pid = fork();
  int status;

  switch (pid) {
    case -1:
      exit(EXIT_FAILURE);
    case 0:
      execl("/bin/sh", "sh", "-c", process, NULL);
      exit(EXIT_FAILURE);
    default:
      waitpid(pid, &status, 0);

      if (WIFEXITED(status) || WIFSIGNALED(status)) {
        monitor(process);
      }
  }
}

int daemonize() {
  if (fork()) {
    exit(EXIT_SUCCESS);
  }

  if (setsid() < 0) {
    exit(EXIT_FAILURE);
  }

  close(STDIN_FILENO);
  close(STDOUT_FILENO);
  close(STDERR_FILENO);
}

int main(int argc, char **argv) {
  char *process;
  int daemon = 0;

  int i;
  for (i = 1; i < argc; i++) {
    char *arg = argv[i];

    if (strcmp("-d", arg) == 0 || strcmp("--daemon", arg) == 0) {
      daemon = 1;
      continue;
    }

    if (strcmp("-h", arg) == 0 || strcmp("--help", arg) == 0) {
      puts("usage: keepup [options] [command]");
      puts("-d --daemonize");
      puts("-h --help");
      continue;
    }

    process = arg;
  }

  if (daemon) {
    daemonize();
  }

  monitor(process);

  return 0;
}
