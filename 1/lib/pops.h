#define _TIME_PTRN_ "%Y-%m-%d %H:%M:%S"
#define _TIME_LEN_ 20 // Time format is 'yyyy-mm-dd hh:mm:ss'
#define _LOGPATH_ "./logs/"
#define _ACCESS_LOG_ "./logs/access.log"
#define _MAXPATH_ 1024
#define _LINE_LEN_ 41
#define _BUF_SIZE_ 81

extern int pcom(char**, int);
extern int greet(char*, char*);
extern int update(char*, char*);
extern int cls();
extern int log_access(char*);
extern int cla();

