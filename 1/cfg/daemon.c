// Daemonized configuration reader
#include <stdio.h>
#include <stdlib.h>

#include <fcntl.h>
#include <signal.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/stat.h>

#include "daemon.h"
#include "config.h"

int alive = 1;
time_t last_read = 0;
struct cfg cf;

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

  if (setsid() < 0 || chdir(_DMN_DIR_) == -1)
    return -1; // Session creation or directory change failed.

  umask(027);

/*
  for (i = sysconf(_SC_OPEN_MAX); i >= 0; i--)
    close(i); // Close descriptors one by one.

  open("/dev/null", O_RDWR);
  dup(0); // Direct STDOUT to /dev/null.
  dup(0); // Direct STDERR to /dev/null.
*/

  return 0;
}

// Open the file lock to show this process has terminated.
int olck()
{

  return 0;
}

// Close the file (lock it down) indicating this daemon is operational.
int clck()
{
  int pid = getpid();

  fopen(_LOCK_FILE_, "a+");

  return 0;
}

int main(void)
{
  struct sigaction sig;
//  struct stat st;
  FILE *file;

/*
  if (stat("./../logs/", &st) != 0) {
    fprintf(stdout, "<%d> Directory 'logs' not found, creating it ...",
            getpid());
    if (mkdir("./../logs", S_IRWXU | S_IRGRP | S_IROTH) < 0) {
      perror("Directory creation failed");
      return 1;
    }

    else
      fprintf(stdout, " creation successfull.\n");
  }
*/

  if (daemonize() < 0) {
    perror("daemon init");
    exit(2);
  }

  /* Assign stdout and stderr to their respective files and set up line
     buffering. */
  file = freopen(_DMN_OUT_, "a+", stdout);
  setvbuf(file, NULL, _IOLBF, (size_t) 512);
  file = freopen(_DMN_ERR_, "a+", stderr);
  setvbuf(file, NULL, _IOLBF, (size_t) 512);

  time_t tim=time(NULL);
  struct tm *now = localtime(&tim);

  fprintf(stdout, "[%02d:%02d] New round of execution.\n",
          now->tm_hour, now->tm_min);
  fprintf(stderr, "[%02d:%02d] New round of execution.\n",
          now->tm_hour, now->tm_min);

  memset(&sig, 0x00, sizeof(sig));
  sigemptyset(&sig.sa_mask);
  sig.sa_handler = int_handler;
  sigaction(SIGINT, &sig, NULL);

  int i = 0, mopen, fd = 0;
  char *ptr = NULL;
  size_t mmapsize = 0;

  while (!csd())
  {
    /* TODO:
     * Lock down a file with pid of this process.
     * Open up FIFOs.
     * Sleep until receiving a signal to check pipe.
     * Process command, write to pipe and go back to sleep.
     * Idle around until receiving SIGINT.
     */
    if (i == 0)
      mopen = fmap(fd, ptr, mmapsize);

    if (i == 3) {
      process_cfg(&cf, fd);
      prt_opt(&cf);
    }

    if (i == 2)
      kill(getpid(), SIGINT);

    printf("Sleeping for 15 seconds...\n");
    sleep(15);
    printf("... woke up!\n");

    i++;
  }
  
  if (mopen)
    fumap(fd, ptr, mmapsize);

  /* TODO:
   * Open up the file lock and delete the file.
   * Close down pipes.
   * Exit.
   */


  printf("Program terminating.\n");

  return(0);
}

