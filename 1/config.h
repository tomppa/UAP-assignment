// Header class for processing the configuration file.
#ifndef _CFG_HEADER_
#define _CFG_HEADER_

#ifndef _CFG_FILENAME_
#define _CFG_FILENAME_ "main.cfg"
#endif

#ifndef _CFG_LINE_LEN_
#define _CFG_LINE_LEN_ 80
#endif

struct cfg
{
  int print;
  int greet;
  int log_access;
  int cls;
  int ll_cla;
  int hl_cla;
  int os_dtls;
  char uopts[100];
};

extern struct cfg cf;
extern int process_cfg(struct cfg*);
extern void prt_opt(struct cfg*);

#endif /* _CFG_HEADER_ */

