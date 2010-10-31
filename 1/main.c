// Main class for the assignment.
#include <stdio.h>
#include <stdlib.h>

#include <fcntl.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>

#include "main.h"

// Some flags to help with various loops and signals.
int alive = 1, dmn = 0, rd = 0;

static void sig_hdlr(int signo)
{
  switch (signo) {
    case SIGINT:
      printf("SIGINT received, setting shutdown flag.\n");
      alive = 0;
      break;

    case SIGPIPE:
      printf("SIGPIPE received, assuming daemon is ready now.\n");
      dmn = 1;
      break;

    case SIGUSR1:
      printf("SIGUSR1 received, daemon has written something on pipe.\n");
      rd = 1;
      break;

    case SIGUSR2:
      printf("SIGUSR2 received, daemon has finished processing command.\n");
      dmn = 1;
      break;

    default:
      fprintf(stderr, "Received a signal (%d) that wasn't handled properly!\n", signo);
  }
}

int chk4lck(int *fd, char *name, int *pid)
{
  struct flock fl;
  int success = 0;
  
  *fd = open(name, O_RDONLY);

  printf("Lockfile (%s) opened.\n", name);

  if (*fd < 0) {
    perror("Problem opening lockfile");
    return -1;
  }

  memset(&fl, 0, sizeof(fl));

  if (fcntl(*fd, F_GETLK, &fl) < 0) {
    perror("Problem probing lockfile for locks");
    return -1;
  }

  if (fl.l_type == F_UNLCK) {
    printf("No lock found.\n");
    success = 0;
  }

  else {
    printf("File locked by process with pid: %d.\n", fl.l_pid);
    *pid = fl.l_pid;
    success = 1;
  }

  close(*fd);

  printf("Closed lockfile.\n");

  return success;
}

int main (void)
{
  int i, chd_fail = 0, fd = 0, pid = getpid();

  printf("Starting up the daemon.\n");

  i = fork();
  
  if (i < 0) {
    perror("Failed to fork");
    return EXIT_FAILURE;
  }

  if (i == 0) { //Child process.
    // Change working directory to config directory first.
    if (chdir(_CFG_DIR_) == -1) {
      perror("Failed to change directory");
      chd_fail = 1;
      return EXIT_FAILURE;
    }

    /* TODO: a more reasonable size for the char array.
     * For now 10 should be suitable for pids.
     */
    char tmp[10];

    if (sprintf(tmp, "%d", pid) < 0) {
      fprintf(stderr, "Failed to convert pid to string.\n");
      return EXIT_FAILURE;
    }

    /* Then use exec to swap into the daemon.
     * Note that we're giving the executable name as first parameter here,
     * so that it behaves the same way as when executed from command
     * line.
     */
    if (execl(_DMN_PTH_, _DMN_PTH_, tmp, NULL) < 0) {
      perror("Failed to exec");
      chd_fail = 1;
      return EXIT_FAILURE;
    }
    
    /* Exec doesn't come back, unless it fails, so no need to worry about
     * child anymore.
     */
  }

  // Only main process comes this far.
  if (chd_fail < 0) {
    printf("Child failed, so exiting parent too.\n");
    return EXIT_FAILURE;
  }

  printf("Daemon started.\n");

  // Set up signal handling.
  struct sigaction sig;
  memset(&sig, 0x00, sizeof(sig));
  sigemptyset(&sig.sa_mask);
  sig.sa_handler = sig_hdlr;
  sigaction(SIGINT, &sig, NULL);
  sigaction(SIGPIPE, &sig, NULL);
  sigaction(SIGUSR1, &sig, NULL);
  sigaction(SIGUSR2, &sig, NULL);

  while (dmn == 0 && alive == 1)
  {
    printf("Sleeping while waiting for daemon to start up ...\n");
    sleep(_MN_SLP_LEN_);
    printf("... woke up!\n");
  }

  printf("Daemon is finished starting up, so can proceed now.\n");

  int lck = chk4lck(&fd, "./cfg/daemon.lck", &pid);

  if (lck == 0) {
    printf("File isn't locked, so cannot get pid this way.\n");
    // TODO: make a plan B for acquiring the pid of daemon.
    return EXIT_FAILURE;
  }

  else if (lck < 0) {
    printf("Cannot even query file for locks, so terminating.\n");
    return EXIT_FAILURE;
  }

  // Request a config update from daemon.
  kill(pid, SIGPIPE);

  while (alive == 1)
  {

    sleep(_MN_SLP_LEN_);
  }

  kill(pid, SIGINT);

  printf("Program terminating.\n");

  return EXIT_SUCCESS;
  /* TODO:
   * Unblocking FIFO writes/reads with daemon.
   */
}

