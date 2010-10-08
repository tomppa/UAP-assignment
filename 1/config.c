// The code for processing the configuration file.
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include "config.h"

int process_cfg (struct cfg *cf)
{
  FILE *f;
  char line[_CFG_LINE_LEN_], *pch, *opt, *tmp, *tmp1;
  int i = 0, val, res;

  f = fopen(_CFG_FILENAME_, "r");

  if (f == NULL) {
    printf("Couldn't open configuration file %s: %s.\n",
           _CFG_FILENAME_, strerror(errno));

    return -1;
  }

  tmp = (char*) malloc ((_CFG_LINE_LEN_ + 20) * sizeof(char));

  while (i < 100) {
    tmp1 = fgets(line, _CFG_LINE_LEN_, f);
    
    if (tmp1[0] == '#')
      pch = NULL;
    
    else
      pch = strchr(line, '=');

    if (pch != NULL) {
      opt = (char*) malloc (_CFG_LINE_LEN_ * sizeof(char));
      strncpy(opt, line, pch-line);
      val = atoi(pch+1); 

      if (strcmp(opt, "greet") == 0)
        cf->greet = val;

      else if (strcmp(opt, "print") == 0)
        cf->print = val;

      else if (strcmp(opt, "log_access") == 0)
        cf->log_access = val;

      else if (strcmp(opt, "ll_cla") == 0)
        cf->ll_cla = val;

      else if (strcmp(opt, "hl_cla") == 0)
        cf->hl_cla = val;

      else if (strcmp(opt, "cls") == 0)
        cf->cls = val;

      else if (strcmp(opt, "os_dtls") == 0)
        cf->os_dtls = val;

      else {
        int len = (int) (strlen(tmp) + strlen(", ") + strlen(opt));

        if (strstr(tmp, " and more.") != NULL)
         len = len +1 -1;// do nothing

        else if (strlen(tmp) > (size_t) 0) {
          if (len > (_CFG_LINE_LEN_ + 10))
            strcat(tmp, " and more.");

          else {
            strcat(tmp, ", ");
            strcat(tmp, opt);
          }
        }
        
        else
          strcat(tmp, opt);
         
        strcpy(cf->uopts, tmp);

        free(opt);
      }
    }

    printf("Round #%d, values are: \n", i);
    prt_opt(cf);
   
    if (ferror(f))
      perror("Error! ");

    if (feof(f))
      break;

    i++;
  }

  printf("Configuration file processsed and options saved.!");

  res = fclose(f);

  free(tmp);

  if (res != 0) {
    printf("Couldn't close configuration file %s: %s.\n",
           _CFG_FILENAME_, strerror(errno));

    return -1;
  }

  return 0;
}

void prt_opt(struct cfg *cf)
{
  printf("{ %d, %d, %d, %d, %d, %d, %d, '%s' }\n", cf->print, cf->greet,
         cf->log_access, cf->ll_cla, cf->hl_cla, cf->cls, cf->os_dtls,
         cf->uopts);
/*
  printf("Printing options...\n");
  printf("Show greeting? %s\n", cf->greet ? "True." : "False.");
  printf("Log access? %s\n", cf->log_access ? "True." : "False.");
  printf("Low level check of last access? %s\n",
         cf->ll_cla ? "True." : "False.");
  printf("High level check of last access? %s\n",
         cf->hl_cla ? "True." : "False.");
  printf("Check log sizes? %s\n", cf->cls ? "True." : "False.");
  printf("Get OS details? %s\n", cf->os_dtls ? "True." : "False.");
*/
}
