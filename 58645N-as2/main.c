// Main class for 2nd assignment.
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <string.h>

#include <sys/select.h>

#include "main.h"

// Uncomment this to enable DEBUG print to stdout.
//#define _DEBUG_
#define _TST_FILE_ "test.tst"

int alive = 1, fd = -1, fd_wr = -1;

static void sig_hdlr(int signo)
{
  switch (signo) {
    case SIGINT:
      printf("SIGINT received, setting shutdown flag.\n");
      alive = 0;
      break;

    case SIGTSTP:
      printf("SIGTSTP received, see how the select loop works.\n");
      write(fd_wr, "hello.\0", 6);
      break;

    default:
      fprintf(stderr, "Asked to handle an unknown signal (%d).\n",
              signo);
      break;
  }
}

int handle_fds(fd_set tbr, struct arli list)
{
  int *fds = list.values, i;
  char *tmp;

  tmp = (char*) malloc(_BUF_SIZE_ * sizeof(char));
  //bzero(&tmp, _BUF_SIZE_);

  for (i = 0; i < list.size; i++) {
    if (FD_ISSET(fds[i], &tbr)) {
      // Reset errno, so you don't get old errors.
      errno = 0;
      int res = read(fds[i], tmp, _BUF_SIZE_);

      if (res) {
        perror("Reading the data from FD");
        return -1;
      }

      else
        printf("Read '%s' from FD.\n", tmp);
    }
  }

  free(tmp);

  return 0;
}

int otest(int *fd_tst)
{
  if (mkfifo(_TST_FILE_, S_IRWXU) < 0) {
    if (errno == EEXIST)
      printf("Warning: file already exists.\n");

    else {
      perror("Creating fifo");
      return -1;
    }
  }

  if ((*fd_tst = open(_TST_FILE_, O_RDONLY|O_NONBLOCK, S_IRWXU)) < 0) {
    perror("Opening the test file");
    return -1;
  }

  printf("Testfile opened. \n");

  return 0;
}

int ctest(int *fd_tst)
{
  if (close(fd) < 0) {
    perror("Closing the testfile");
    return -1;
  }

  printf("Testfile closed.\n");
  return 0;
}

void init_tbr(fd_set *tbr, struct arli list)
{
  FD_ZERO(tbr);

  for (int i = 0; i < list.size; i++)
    FD_SET(list.values[i], tbr);
}

void set_tv(struct timeval *tv)
{
  tv->tv_sec = _SEC_;
  tv->tv_usec = _USEC_;
}

int main(void)
{
  fd_set tbr;
  struct timeval tv;
  struct arli list;
  struct sigaction sig;
  int value, highfd, reset = 1;

  printf("Setting up signal handling.\n");

  memset(&sig, 0x00, sizeof(sig));
  sigemptyset(&sig.sa_mask);
  sig.sa_handler = sig_hdlr;
  sigaction(SIGINT, &sig, NULL);
  sigaction(SIGTSTP, &sig, NULL);

  printf("Making the list.\n");

  if (otest(&fd) < 0)
    exit(EXIT_FAILURE);

  if ((fd_wr = open(_TST_FILE_, O_WRONLY, S_IRWXU)) < 0)
    perror("Open failure");

  create(&list);
  add(&list, fd);

  tv.tv_sec = 0;
  tv.tv_usec = 0;
  highfd = fd;

  int j = 0;

  while (alive != 0 && j < 5)
  {
    j++;

    init_tbr(&tbr, list);
    if (reset == 1)
      set_tv(&tv);
    else
      reset = 1;

    #ifdef _DEBUG_
    printf("*** DEBUG *** Time for sleeping: %d.%ds.\n", (int) tv.tv_sec,
           (int) tv.tv_usec);
    #endif //_DEBUG_

    value = select(highfd + 1, &tbr, NULL, NULL, &tv);

    if (value == -1) {
      if (errno == EINTR) {
        // Interrupted by an incoming signal.
        reset = 0;
      }
      
      else {
        perror("Problem with select()");
        exit(EXIT_FAILURE);
      }
    }

    else if (value == 0)
      printf("Nothing to be done, so go back to sleep.\n");

    else {
      printf("Things to do, yay!\n");
      reset = 0;
      handle_fds(tbr, list);
    }
  }

  if (ctest(&fd) < 0)
    printf("Tsk tsk tsk! File was left open?\n");

  else
    printf("File closed.\n");

  if (unlink(_TST_FILE_) < 0)
    perror("Fifo removal");
    
  else
    printf("Fifo removed.\n");

  close(fd_wr);
  printf("Terminating.\n");

  return 0;
}

