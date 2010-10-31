// Daemonized configuration reader
#include <stdio.h>
#include <stdlib.h>

#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <signal.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/stat.h>

#include "daemon.h"
#include "config.h"

int alive = 1, cfg = 0, rd = 0, mn = 0;
struct cfg cf;

// Handle signals.
static void sig_hdlr(int signo)
{
  switch (signo) {
    case SIGINT:
      printf("SIGINT received, setting shutdown flag.\n");
      alive = 0;
      break;

    case SIGPIPE:
      printf("SIGPIPE received, main wants to start processing config.\n");
      cfg = 1;
      break;

    case SIGUSR1:
      printf("SIGUSR1 received, main has written something on pipe.\n");
      rd = 1;
      break;

    case SIGUSR2:
      printf("SIGUSR2 received, main has finished reading command.\n");
      mn = 1;
      break;

    default:
      fprintf(stderr, "Received a signal (%d) that wasn't handled properly!\n", signo);
  }
}

// Check for shutdown conditions, i.e., whether to live or die.
int csd()
{
  if (!alive) {
    printf("Shutdown flag is set.\n");
    return -1;
  }

  return 0;
}

// Open up the pipe required for communicating with other processes.
int opipe(int *fd)
{
  if (mkfifo(_DMN_FIFO_, 0600) == -1) {
    if (errno == EEXIST)
      printf("Warning: FIFO already exists, it could be in use.\n");

    else {
      perror("Failed to create FIFO");
      return -1;
    }
  }
  
  if (errno != EEXIST)
    printf("FIFO created.\n");

  if ((*fd = open(_DMN_FIFO_, O_RDWR|O_NONBLOCK)) == -1) {
    perror("Failed to open FIFO");
    return -1;
  }


  printf("FIFO opened.\n");

  return 0;
}

// Close down the pipe used to communicate with the rest of the world.
int cpipe(int *fd)
{
  if (close(*fd) == -1) {
    perror("Failed to close down FIFO");
    return -1;
  }

  printf("FIFO closed.\n");

  return 0;
}

/* Remove the process from the rest of the program, to work as an independent
 * unit.
 */
int daemonize()
{
  int i;
  FILE *file;

  printf("Daemonize starting up.\n");

  i = fork();

  if (i > 0) {
    printf("\t- parent of daemon exits.\n");
    _exit(0); // Parent process exits.
  }

  if (i < 0) {
    perror("Failed to fork");
    return -1;
  }

  printf("\t- fork successfull, child up and running.\n");

  if (setsid() < 0 || chdir(_DMN_DIR_) == -1) {
    perror("Session creation or directory change failed");
    return -1;
  }

  printf("\t- new session created and working directory changed.\n");

  umask(027);

  printf("\t- mask changed.\n");
  printf("\t- moving stdout & stderr to logfiles for further reading...\n");

  /* Assign stdout and stderr to their respective files and set up line
   * buffering.
   */
  file = freopen(_DMN_OUT_, "a+", stdout);

  if (file == NULL) {
    perror("Associating stdout failed");
    return -1;
  }

  setvbuf(file, NULL, _IOLBF, (size_t) 512);
  file = freopen(_DMN_ERR_, "a+", stderr);

  if (file == NULL) {
    /* Have to use printf here and print an error to standard out, since
     * strerr might be assigned to some unknown location and moving it again
     * would need even more error checking.
     */
    printf("Associating stderr failed: %s.\n", strerror(errno));
    return -1;
  }

  setvbuf(file, NULL, _IOLBF, (size_t) 512);

  printf("Daemonize finished successfully.\n");

  time_t tim=time(NULL);
  struct tm *now = localtime(&tim);

  /* Just to facilitate reading log files a bit, since all log entries
   * don't have a timestamp yet.
   */
  fprintf(stdout, "[%02d:%02d] New round of execution.\n",
          now->tm_hour, now->tm_min);
  fprintf(stderr, "[%02d:%02d] New round of execution.\n",
          now->tm_hour, now->tm_min);

  return 0;
}

// Open the file lock to show this process has terminated.
int olck(int *fd)
{
  struct flock fl;

  memset(&fl, 0, sizeof(fl));

  if (fcntl(*fd, F_GETLK, &fl) < 0) {
    perror("Problem probing lockfile for locks");
    return -1;
  }

  if (fl.l_type == F_UNLCK) {
    fl.l_type = F_UNLCK;

    if (fcntl(*fd, F_SETLK, &fl) < 0) {
      perror("Problem unlocking lockfile");
      return -1;
    }

    printf("Lockfile unlocked.\n");
  }

  else
    printf("Lock encountered by pid %d, so cannot unlock the file.\n", fl.l_pid);

  close(*fd);
  printf("Lockfile descriptor closed.\n");

  return 0;
}

// Close the file (lock it down) indicating this daemon is operational.
int clck(int *fd)
{
  *fd = open(_LCK_FILE_, O_WRONLY|O_CREAT|O_TRUNC, 0600);

  if (*fd < 0) {
    perror("Problem opening lockfile");
    return -1;
  }

  printf("Lockfile opened/created.\n");

  struct flock fl;

  memset(&fl, 0, sizeof(fl)); // initialize fl

  fl.l_type = F_WRLCK;

  /*TODO: This for some reason always returns ENOLCK on school computers.
          NFS issue perhaps? */
  if (fcntl(*fd, F_SETLKW, &fl) < 0) {
    perror("Problem locking lockfile");
    return -1;
  }

  printf("Lock on lockfile acquired.\n");

  return 0;
}

int main(int argc, char *argv[])
{
  struct sigaction sig;
  int pid = 0;
/*
  TODO: move to a common library, where all modules can use it.

  struct stat st;

  if (stat("./../logs/", &st) != 0) {
    fprintf(stdout, "<%d> Directory 'logs' not found, creating it ...",
            getpid());
    if (mkdir("./../logs", S_IRWXU | S_IRGRP | S_IROTH) < 0) {
      perror("Directory creation failed");
      return EXIT_FAILURE;
    }

    else
      fprintf(stdout, " creation successfull.\n");
  }
*/
  if (argc == 2) {
    pid = atoi(argv[1]);
    printf("Process ID of calling process is %d.\n", pid);
  }

  else {
    printf("Tsk tsk! Give one and only one parameter as parameter!\n");
    return EXIT_FAILURE;
  }

  if (daemonize() < 0)
    exit(2);

  memset(&sig, 0x00, sizeof(sig));
  sigemptyset(&sig.sa_mask);
  sig.sa_handler = sig_hdlr;
  sigaction(SIGINT, &sig, NULL);
  sigaction(SIGPIPE, &sig, NULL);
  sigaction(SIGUSR1, &sig, NULL);
  sigaction(SIGUSR2, &sig, NULL);

  int i = 0, mopen = 0, cfg_fd = 0, lck_fd = 0, fifo_fd = 0;
  char *ptr = NULL;
  size_t mmapsize = (size_t) 0;

  while (csd() == 0)
  {
    /* TODO:
     * Process command, write to pipe and go back to sleep.
     */

    /* On first rotation, map config file to memory and open pipe.
     * Also open up lockfile and put a lock on it to show daemon is alive.
     */
    if (i == 0) {
      mopen = fmap(&cfg_fd, &ptr, &mmapsize);
      if (opipe(&fifo_fd) < 0 || clck(&lck_fd) < 0 || mopen < 0)
        i = -2;
    }

    // Tell main program that daemon is up and operational.
    if (i == 1)
      kill(pid, SIGPIPE);

    /* When main has requested a configuration update,
     * read up config file and process its contents.
     */
    if (cfg == 1) {
      process_cfg(&cf, cfg_fd);
      prt_opt(&cf);

      cfg = 0;
    }

    /* When daemon has been left on a loop for too long
     * shut it down and kill main, if it's still alive.
     */
    if (i == 7) {
      i = -2;
      kill(pid, SIGINT);
    }

    printf("Sleeping for a while...\n");
    sleep(_SLEEP_LEN_);
    printf("... woke up!\n");

    // Terminate program, only come here if something went wrong.
    if (i == -2)
      kill(getpid(), SIGINT);

    i++;
  }

  // If config file was memorymapped earlier unmap it.
  if (mopen == 0) {
    if (fumap(&cfg_fd, &ptr, &mmapsize) < 0)
      printf("Problem unmapping config file, see error log.\n");
  }

  if (cpipe(&fifo_fd) < 0)
    printf("Problem closing pipe, see error log.\n");

  if (olck(&lck_fd) < 0)
    printf("Problem opening lock, see error log.\n");

  printf("Program terminating.\n");

  return EXIT_SUCCESS;
}

