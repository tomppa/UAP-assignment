// Header for the configuration daemon.

#ifndef _DMN_DIR_
#define _DMN_DIR_ "./dmn"
#endif

#ifndef _DMN_OUT_
#define _DMN_OUT_ "daemon.out"
#endif

#ifndef _DMN_ERR_
#define _DMN_ERR_ "daemon.err"
#endif

#ifndef _PID_SZ_
#define _PID_SZ_ (size_t) ((sizeof(int)*CHAR_BIT - 1) / 3.3) + 3;
#endif

#ifndef _LCK_FILE_
#define _LCK_FILE_ "../daemon.lck"
#endif

#ifndef _LCK_ST_
#define _LCK_ST_ 0
#endif

#ifndef _LCK_LEN_
#define _LCK_LEN_ 10
#endif

extern int cpipes();
extern int opipes();
extern int clck();
extern int olck();
extern int daemonize();


