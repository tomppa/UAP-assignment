// Header for main.

#include "lib/arraylist.h"

#ifndef _MAIN_STUFF_
#define _MAIN_STUFF_
#define _BUF_SIZE_ 81
#define _SEC_ 3
#define _USEC_ 151592
#define _NUM_THREADS_ 5
#endif

struct thread_info {
  pthread_t thread_id;
  int thread_num;
  char *message;
  int ai;
};

//extern int handle_fds(fd_set, struct arli, pthread_t threads[]);
extern int handle_fd(int, struct thread_info*);
extern void init_tbr(fd_set*, struct arli);
extern void set_tv(struct timeval*);

