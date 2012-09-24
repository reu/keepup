#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>

pid_t pid;
char *pidfile_path;

void write_pidfile(char *path, pid_t pid) {
  int size = sizeof(pid) + 1;

  char buffer[size];
  snprintf(buffer, size, "%d", pid);

  int fd = open(path, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
  write(fd, buffer, size);

  close(fd);
}

void remove_pidfile(char *path) {
  remove(path);
}

void monitor(char *process) {
  int status;

  switch (pid = fork()) {
    case -1:
      // We could't fork
      exit(EXIT_FAILURE);
    case 0:
      // Child process: exec the process command
      execl("/bin/sh", "sh", "-c", process, NULL);
      exit(EXIT_FAILURE);
    default:
      // Parent process

      // Writes the pidfile
      if (pidfile_path) {
        write_pidfile(pidfile_path, pid);
      }

      // Waits the child and then restarts it
      waitpid(pid, &status, 0);
      monitor(process);
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

void safe_exit() {
  kill(pid, SIGTERM);

  if (pidfile_path) {
    remove_pidfile(pidfile_path);
  }

  exit(EXIT_FAILURE);
}

int main(int argc, char **argv) {
  char *process;
  int daemon = 0;

  int i;
  for (i = 1; i < argc; i++) {
    char *arg = argv[i];

    if (strcmp("-d", arg) == 0 || strcmp("--daemonize", arg) == 0) {
      daemon = 1;
      continue;
    }

    if (strcmp("-p", arg) == 0 || strcmp("--pidfile", arg) == 0) {
      pidfile_path = argv[++i];
      continue;
    }

    if (strcmp("-h", arg) == 0 || strcmp("--help", arg) == 0) {
      puts("usage: keepup [options] [command]");
      puts("-d --daemonize");
      puts("-p --pidfile <path>");
      puts("-h --help");
      exit(EXIT_SUCCESS);
    }

    process = arg;
  }

  signal(SIGTERM, &safe_exit);
  signal(SIGQUIT, &safe_exit);

  if (daemon) {
    daemonize();
  }

  monitor(process);

  return 0;
}
