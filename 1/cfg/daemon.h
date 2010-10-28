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

extern int cpipes();
extern int opipes();
extern int clck();
extern int olck();
extern int daemonize();


