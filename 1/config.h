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

#endif /* _CFG_HEADER_ */

int process_cfg (cfg *cf)
{
  FILE *f;
  char line[_CFG_LINE_LEN_], *pch, opt[_CFG_LINE_LEN_], tmp[100];
  int i = 0, val, res;

  f = fopen(_CFG_FILENAME_, "r");

  if (f == NULL) {
    printf("Couldn't open configuration file %s: %s.\n",
           _CFG_FILENAME_, strerror(errno));

    return -1;
  }

  while (i < 10) {
    fgets(line, _CFG_LINE_LEN_, f);
    pch = strchr(line, '=');

    if (pch != NULL) {
       strncpy(opt, line, pch-line);
       val = atoi(pch+1); 

       if (strcmp(opt, "greet") == 0)
         cf->greet = val;

       else {
         if (strlen(tmp) != 0) {
           strcat(tmp, ", ");
           strcat(tmp, opt);
         }

         else {
           strcat(tmp, opt);
         }
         
         strcpy(cf->uopts, tmp);
       }
    }

    if (feof(f))
      break;

    i++;
  }

  res = fclose(f);

  if (res != 0) {
    printf("Couldn't close configuration file %s: %s.\n",
           _CFG_FILENAME_, strerror(errno));

    return -1;
  }

  return 0;
}

