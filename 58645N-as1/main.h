#ifndef _DMN_SETUP_
#define _DMN_SETUP_
#define _CFG_DIR_ "./cfg/"
#define _DMN_PTH_ "./daemon"
#define _DMN_LCK_FILE_ "./cfg/daemon.lck"
#endif

#ifndef _MN_SETUP_
#define _MN_SETUP_
#define _MN_SLP_LEN_ 3
#define _MN_FIFO_ "cfg/daemon.fifo"
#define _MN_BUF_SIZE_ 81
#endif

extern int chk4lck(char*, int*);
extern int chk4pid(char*, int*);

