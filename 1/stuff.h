// library file for stuff
#ifndef _BUF_SIZE_
#define _BUF_SIZE_ 0x400
#endif

#ifndef _LINE_LENGTH_
#define _LINE_LENGTH_ 41
#endif

#ifndef _ACCESS_LOG_
#define _ACCESS_LOG_ "./logs/access.log"
#endif

#ifndef _TIME_PTRN_
#define _TIME_PTRN_ "%Y-%m-%d %H:%M:%S"
#endif

#ifndef _MAXPATH_
#define _MAXPATH_ 1024
#endif

#ifndef _LOGPATH_
#define _LOGPATH_ "./logs/"
extern int log_access(char*);
extern int ll_access();
extern int hl_access();
#endif

#ifndef _GREET_
#define _GREET_
extern int greet(char*, char*);
#endif

#ifndef _UPDATE_
#define _UPDATE_
extern void update(char*, char*);
#endif

#ifndef _CHK_LOG_SZ_
#define _CHK_LOG_SZ_
extern int cls();
#endif

#ifndef _CHK_PF_
#define _CHK_PF_
extern void chk_pf(char*, char*, char*);
#endif

