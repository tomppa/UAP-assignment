// Main class for the assignment.
#include <stdio.h>
#include <stdlib.h>

#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>

#include "main.h"
#include "lib/fops.h"
#include "lib/pops.h"

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

int main (void)
{
  int i, chd_fail = 0, pid = getpid();

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

  int lck = chk4lck(_DMN_LCK_FILE_, &pid);

  if (lck == 0) {
    printf("File isn't locked, so cannot get pid this way.\n");
    // TODO: make a plan B for acquiring the pid of daemon.
    return EXIT_FAILURE;
  }

  else if (lck == -1) {
    printf("Cannot even query file for locks, so terminating.\n");
    return EXIT_FAILURE;
  }

  else if (lck == -2) {
    printf("Let's read the PID from the file instead.\n");

    if (chk4pid(_DMN_LCK_FILE_, &pid) < 0) {
      printf("Reading failed too, so cannot get PID and terminating.\n");
      return EXIT_FAILURE;
    }
    
    printf("Pid %d found!", pid);
  }

  int fifo_fd = 0;

  // Try opening the pipe, if that fails, kill daemon and exit.
  if (opipe(&fifo_fd, _MN_FIFO_) < 0) {
    kill(pid, SIGINT);
    return EXIT_FAILURE;
  }

  // Request a config update from daemon.
  kill(pid, SIGPIPE);

  while (alive == 1)
  {
    if (rd == 1) {
      char *buf;

      bzero(buf, _MN_BUF_SIZE_);
      rd = 0;
      if (read(fifo_fd, buf, _MN_BUF_SIZE_) == 0) {
        //TODO: nothing to read, request new submission.
      }

      else {
        kill(pid, SIGUSR2);

        printf("Read command '%s' from pipe.\n", buf);

        int ret = pcom(&buf, _MN_BUF_SIZE_);

        if (ret == -2)
          fprintf(stderr, "Unrecognized command given.\n");
        else if (ret == -1)
          printf("Command failed.\n");
        else
          printf("Command successfull.\n");

        printf("Writing reply '%s' back to pipe.\n", buf);

        if (write(fifo_fd, buf, _MN_BUF_SIZE_) < 0) {
          // TODO: Buffer is full, need to empty it.
          fprintf(stderr, "Write buffer full!\n");
        }

        else
          printf("Reply written.\n");

        kill(pid, SIGUSR1);
      }
    }

    sleep(_MN_SLP_LEN_);
  }

  kill(pid, SIGINT);

  if (cpipe(&fifo_fd) < 0) {
    fprintf(stderr, "Problem closing the pipe.\n");
    return EXIT_FAILURE;
  }

  printf("Program terminating.\n");

  return EXIT_SUCCESS;
}

