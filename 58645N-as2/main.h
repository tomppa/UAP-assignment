// Header for main.

#include "lib/arraylist.h"

#ifndef _MAIN_STUFF_
#define _MAIN_STUFF_
#define _BUF_SIZE_ 81
#define _SEC_ 3
#define _USEC_ 151592
#endif

extern int handle_fds(fd_set, struct arli);
extern void init_tbr(fd_set*, struct arli);
extern void set_tv(struct timeval*);

