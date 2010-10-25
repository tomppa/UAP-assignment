// Daemonized configuration reader
#include <stdio.h>
#include <stdlib.h>

#include <fcntl.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/stat.h>

#include "cnfdmn.h"

int alive = 1;

// Handle SIGINTs.
static void int_handler(int signo)
{
  if (signo == SIGINT)
    alive = 0;
}

// Check for shutdown conditions, i.e., whether to live or die.
int csd()
{
  if (!alive) {
    fprintf(stdout, "Shutdown flag set, closing files and exiting.\n");
    cpipes();
    return 1;
  }

  return 0;
}

// Open up the pipes required for communicating with other processes.
int opipes()
{

  return 0;
}

// Close down pipes used to communicate with the rest of the world.
int cpipes()
{

  return 0;
}

/* Remove the process from the rest of the program, to work as an independent
   unit. */
int daemonize()
{
  int i;

  i = fork();
  if (i > 0)
    _exit(0); // Parent process exits.

  if (i < 0)
    return -1; // Failed to fork.

  if (setsid() < 0 || chdir(_CFG_DIR_) == -1)
    return -1; // Session creation or directory change failed.

  umask(027);

  for (i = sysconf(_SC_OPEN_MAX); i >= 0; i--)
    close(i); // Close descriptors one by one.

  open("/dev/null", O_RDWR);
  dup(0); // Direct STDOUT to /dev/null.
  dup(0); // Direct STDERR to /dev/null.

  return 0;
}

// Open the file lock to show this process has terminated.
int olock()
{

  return 0;
}

// Close the file (lock it down) indicating this daemon is operational.
int clock()
{

  return 0;
}

int main(void)
{
  struct sigaction sig;

  if (daemonize() < 0) {
    perror("daemon init");
    exit(2);
  }

  memset(&sig, 0x00, sizeof(sig));
  sigemptyset(&sig.sa_mask);
  sig.sa_handler = int_handler;
  sigaction(SIGINT, &sig, NULL);

  sleep(60);

  return(0);
}

