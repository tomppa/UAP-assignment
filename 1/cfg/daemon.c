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
#include "../lib/fops.h"

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
      fprintf(stderr, "Received a signal (%d) that wasn't handled properly!\n",
              signo);
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

int hdl_data (int fd, struct cfg cf, int pid)
{
  int i = 0, retry = 0;
  char buf[_BUF_SIZE_];

  while (i <= cf.opt_amt)
  {
    switch (i)
    {
      case 1: if (cf.print)
                strcpy(buf, "print");
              break;

      case 2: if (cf.greet)
                strcpy(buf, "greet");
              break;

      case 3: if (cf.log_access)
                strcpy(buf, "log_access");
              break;

      case 4: if (cf.cls)
                strcpy(buf, "cls");
              break;

      case 5: if (cf.cla)
                strcpy(buf, "cla");
              break;

      case 6: if (cf.os_dtls)
                break;

      default: break;
    }

    if (alive == 0) {
      return -1;
    }

    if (buf[0] != '\0') {

      printf("Sending a command (%s) to main.\n", buf);
  
      // First write option to pipe.
      if (write(fd, buf, strlen(buf)) < strlen(buf)) {
        perror("Write buffer filled up before finishing write");
        // TODO: Free up buffer space.
      }

      // Signal main that there's data waiting to be read on the pipe.
      kill(pid, SIGUSR1);

      // Then wait for main to read it and send signal back.
      while (rd == 0)
        sleep(_SLEEP_LEN_);

      // Reset the flag.
      rd = 0;
      
      // Read reply from pipe and tell main pipe is free again.
      if (read(fd, buf, _BUF_SIZE_) == 0) {
        perror("Read buffer was empty");
        // TODO: Signal main to resend.
      }

      kill(pid, SIGUSR2);
    
      // If reply was a success, move on, else try again.
      if (strcmp(buf, "success")) {
        printf("Command delivered and performed!\n");
        i++;
      }

      else {
        if (retry == 1) {
          fprintf(stderr, "Delivery failure!\n");
          return -1;
        }

        else {
          retry = 1;
          if (strcmp(buf, "failure")) 
            printf("Delivery had some problems!\n");
          else
            printf("Received non-standard reply.\n");
        }
      }

      bzero(buf, _BUF_SIZE_);
    }
  }

  return 0;
}

int main(int argc, char *argv[])
{
  struct sigaction sig;
  int pid = 0;

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
    /* On first rotation, map config file to memory and open pipe.
     * Also open up lockfile and put a lock on it to show daemon is alive.
     */
    if (i == 0) {
      int lock = clck(&lck_fd, _LCK_FILE_);

      mopen = fmap(&cfg_fd, &ptr, &mmapsize);
      if (opipe(&fifo_fd, _DMN_FIFO_) < 0 ||
          lock == -1 || mopen < 0)
        i = -2;

      else if (lock == -2) {
        printf("Let's write PID to lockfile instead.\n");

        if (wr_pid(lck_fd, getpid()) < 0)
          i = -2;
      }
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

      hdl_data(cfg_fd, cf, pid);

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

