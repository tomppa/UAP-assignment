// Main class for 2nd assignment.
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <errno.h>
#include <fcntl.h>
#include <pthread.h>
#include <signal.h>
#include <string.h>

#include <sys/select.h>

#include "main.h"

// Uncomment this to enable DEBUG print to stdout.
#define _DEBUG_
#define _TST_FILE_ "test.tst"

int alive = 1, thrd_amt = 0, fd_wr = -1;

static void sig_hdlr(int signo)
{
  switch (signo) {
    case SIGINT:
      printf("SIGINT received, setting shutdown flag.\n");
      alive = 0;
      break;

    case SIGTSTP:
      printf("SIGTSTP received, see how the select loop works.\n");
      write(fd_wr, "hello\0", 6);
      break;

    default:
      fprintf(stderr, "Asked to handle an unknown signal (%d).\n",
              signo);
      break;
  }
}

void *process_data(void *arg)
{
  struct thread_info *my_data;
  pthread_t taskid;
  int thread_num;
  char *message;

  my_data = (struct thread_info *) arg;
  taskid = my_data->thread_id;
  thread_num = my_data->thread_num;
  message = my_data->message;
  
  int ai = my_data->ai;

  printf("Thread #%d getting command '%s', with id %lu and ai %d.\n",
         thread_num, my_data->message, (unsigned long) taskid, ai);

  sleep(5);
  thrd_amt--;

  printf("Thread #%d exiting.\n", thread_num);

  pthread_exit(NULL);
}

int handle_fd(int fd, struct thread_info *tinfo)
{
  char *tmp;

  tmp = (char*) malloc(_BUF_SIZE_ * sizeof(char));

  printf("File descriptor is: %d.\n", fd);

  // Reset errno, so you don't get old errors.
  errno = 0;
  int res = read(fd, tmp, _BUF_SIZE_);

  if (res < 0) {
    perror("Reading the data from FD");
    return -1;
  }

  else {
    int rc;

    thrd_amt++;
    tinfo->thread_num = thrd_amt;
    tinfo->message = tmp;
    tinfo->ai = 5;

    struct thread_info temp = *tinfo;

    #ifdef _DEBUG_
    printf("**DEBUG** Message for thread #%d is %s, ai is %d and id is %lu.\n",
           tinfo->thread_num, tinfo->message, tinfo->ai,
           (unsigned long) tinfo->thread_id);
    #endif

    rc = pthread_create(&tinfo->thread_id, NULL, process_data, (void *) &tinfo);
        
    if (rc < 0) {
      perror("Creating thread");
      thrd_amt--;
      return -1;
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

  printf("Testfile opened.\n");

  return 0;
}

int ctest(int *fd_tst)
{
  if (close(*fd_tst) < 0) {
    perror("Closing the testfile");
    return -1;
  }

  printf("Testfile closed.\n");
  *fd_tst = -1;
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
  fd_set tbr; // FDs to be read after select stops blocking.
  struct arli list; // Arraylist of all FDs given to select.
  struct thread_info *tinfo;
  struct timeval tv;
  struct sigaction sig;
  pthread_attr_t attr;
  int value, highfd, reset = 1, fd = -1;

  printf("Setting up signal handling.\n");

  memset(&sig, 0x00, sizeof(sig));
  sigemptyset(&sig.sa_mask);
  sig.sa_handler = sig_hdlr;
  sigaction(SIGINT, &sig, NULL);
  sigaction(SIGTSTP, &sig, NULL);

  // Opening up a pipe.
  if (otest(&fd) < 0)
    exit(EXIT_FAILURE);

  /* Open up a dummy write end to the fifo, so it can be guaranteed to stay
   * open and in case all other write ends exit, select won't always return
   * true (and thus never block) because of encountering EOF.
   */
  if ((fd_wr = open(_TST_FILE_, O_WRONLY, S_IRWXU)) < 0)
    perror("Dummy write open failure");

  printf("Making the list.\n");

  create(&list);
  add(&list, fd);
  add(&list, fileno(stdin));

  tinfo = calloc(_NUM_THREADS_, sizeof(struct thread_info));

  if (pthread_attr_init(&attr) != 0)
    perror("Attribute initialization for pthreads");

  tv.tv_sec = 0;
  tv.tv_usec = 0;
  highfd = fd;

  int j = 0;

  // Select loop.
  while (alive != 0 && j < 5)
  {
    j++;

    init_tbr(&tbr, list);

    // If loop finished before timeout ended, don't reset timer.
    if (reset == 1)
      set_tv(&tv);
    else
      reset = 1;

    value = select(highfd + 1, &tbr, NULL, NULL, &tv);

    if (value == -1) {
      // Interrupted by an incoming signal.
      if (errno == EINTR)
        reset = 0;
      
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
      int i;
      for (i = 0; i < list.size; i++) {
        char *tst;

        if (FD_ISSET(list.values[i], &tbr)) {
          handle_fd(list.values[i], &tinfo[thrd_amt]);
          tst = "true\0";
        }
        else
          tst = "false\0";

        printf("Bit #%d (file descriptor #%d) is set to %s.\n", i,
               list.values[i], tst);
      }
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
  free(tinfo);
  printf("Main thread terminating.\n");

  pthread_exit(NULL);
}

