#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>

pid_t pid;

char *error_command;

char *pidfile_path;
char *keepup_pidfile_path;

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

void monitor(char *process, int max_retries) {
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

      // In case we have an error callback, execute it
      if (error_command) {
        system(error_command);
      }

      // Check if we can retry the process if informed a maximum number of retries
      if (max_retries) {
        if (--max_retries == 0) {
          exit(EXIT_SUCCESS);
        }
      }

      monitor(process, max_retries);
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

void safe_exit(int sig) {
  kill(pid, sig);

  if (pidfile_path) {
    remove_pidfile(pidfile_path);
  }

  if (keepup_pidfile_path) {
    remove_pidfile(keepup_pidfile_path);
  }

  exit(EXIT_FAILURE);
}

int main(int argc, char **argv) {
  char *process;
  int daemon = 0;
  int max_retries = 0;

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

    if (strcmp("-k", arg) == 0 || strcmp("--keepup-pidfile", arg) == 0) {
      keepup_pidfile_path = argv[++i];
      continue;
    }

    if (strcmp("-r", arg) == 0 || strcmp("--max-retries", arg) == 0) {
      max_retries = atoi(argv[++i]);
      continue;
    }

    if (strcmp("-e", arg) == 0 || strcmp("--error-command", arg) == 0) {
      error_command = argv[++i];
      continue;
    }

    if (strcmp("-h", arg) == 0 || strcmp("--help", arg) == 0) {
      puts("usage: keepup [options] [command]");
      puts("-d --daemonize");
      puts("-p --pidfile <path>");
      puts("-k --keepup-pidfile <path>");
      puts("-r --max-retries <number>");
      puts("-e --error-command <command>");
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

  if (keepup_pidfile_path) {
    write_pidfile(keepup_pidfile_path, getpid());
  }

  monitor(process, max_retries);

  return 0;
}
