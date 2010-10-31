// Header class for processing the configuration file.
#ifndef _CFG_HEADER_
#define _CFG_HEADER_

#ifndef _CFG_FILENAME_
#define _CFG_FILENAME_ "main.cfg"
#endif

#ifndef _CFG_LINE_LEN_
#define _CFG_LINE_LEN_ 80
#endif

#ifndef _CFG_FILE_
// Config file relative to the path of the daemon after it has moved itself.
#define _CFG_FILE_ "./../config.txt"
#endif

struct cfg
{
  int opt_amt;
  int print;
  int greet;
  int log_access;
  int cls;
  int cla;
  int os_dtls;
  char uopts[80];
  char bvals[80];
};

extern struct cfg cf;
extern int process_cfg(struct cfg*, int);
extern void tmpcat(char*, char*);
extern int fmap(int*, char**, size_t*);
extern int fumap(int*, char**, size_t*);
extern int crdtm(struct stat*);
extern void prt_opt(struct cfg*);

#endif /* _CFG_HEADER_ */

