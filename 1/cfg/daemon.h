// Header for the configuration daemon.

#ifndef _DMN_STUFF_
#define _DMN_STUFF_
#define _DMN_DIR_ "./dmn"
#define _DMN_OUT_ "daemon.out"
#define _DMN_ERR_ "daemon.err"
#define _DMN_FIFO_ "../daemon.fifo"
#endif

#ifndef _PID_SZ_
#define _PID_SZ_ (size_t) ((sizeof(int)*CHAR_BIT - 1) / 3.3) + 3;
#endif

#ifndef _SLEEP_LEN_
#define _SLEEP_LEN_ 5
#endif

#ifndef _LCK_STUFF_
#define _LCK_STUFF_
#define _LCK_FILE_ "../daemon.lck"
#define _LCK_ST_ 0
#define _LCK_LEN_ 10
#endif

extern int cpipes();
extern int opipes();
extern int clck();
extern int olck();
extern int daemonize();

