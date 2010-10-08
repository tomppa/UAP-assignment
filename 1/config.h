// Header class for processing the configuration file.
#ifndef _CFG_HEADER_
#define _CFG_HEADER_

#ifndef _CFG_FILENAME_
#define _CFG_FILENAME_ "main.cfg"
#endif

#ifndef _CFG_LINE_LEN_
#define _CFG_LINE_LEN_ 80
#endif

typedef struct {
  int greet;
  char uopts[100];
} cfg;

extern cfg cf;
extern int process_cfg(cfg*);

#endif /* _CFG_HEADER_ */

