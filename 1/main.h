#ifndef _DMN_SETUP_
#define _DMN_SETUP_
#define _CFG_DIR_ "./cfg/"
#define _DMN_PTH_ "./daemon"
#define _DMN_LCK_FILE_ "./cfg/daemon.lck"
#endif

#ifndef _MN_SETUP_
#define _MN_SETUP_
#define _MN_SLP_LEN_ 3
#endif

extern int chk4lck(int*, char*, int*);

