// Signal handler class

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

/*
static void int_handler(int signo)
{
  printf("Received signal %d\n", signo);
  if (signo == SIGINT)
    running = 0;
}
*/

static void rts_handler(int signum, siginfo_t *info, void *context)
{
  printf("signal number: %d\n", info->si_signo);
  printf("process ID:    %d\n", info->si_pid);
  printf("real user ID:  %d\n", info->si_uid);
  printf("value:         %08X\n", info->si_value.sival_int);

  switch(info->si_code) {
    case SI_USER:
      printf("Signal from user.\n");
      break;

    case SI_QUEUE:
      printf("Signal from sigqueue.\n");
      break;

    default: break;
  }
}
