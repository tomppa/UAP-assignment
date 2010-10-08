// The code for processing the configuration file.
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include "config.h"

int process_cfg (cfg *cf)
{
  FILE *f;
  char line[_CFG_LINE_LEN_], *pch, opt[_CFG_LINE_LEN_], tmp[100], *tmp1;
  int i = 0, val, res;

  tmp1 = (char*) malloc (100 * sizeof(char));

  f = fopen(_CFG_FILENAME_, "r");

  if (f == NULL) {
    printf("Couldn't open configuration file %s: %s.\n",
           _CFG_FILENAME_, strerror(errno));

    return -1;
  }

  while (i < 10) {
    tmp1 = fgets(line, _CFG_LINE_LEN_, f);
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
  free(tmp1);

  if (res != 0) {
    printf("Couldn't close configuration file %s: %s.\n",
           _CFG_FILENAME_, strerror(errno));

    return -1;
  }

  return 0;
}

