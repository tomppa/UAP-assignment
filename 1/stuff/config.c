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

  // Allocate some memory here for the variables that need it.
  opt = (char*) malloc (_CFG_LINE_LEN_ * sizeof(char));
  tmp = (char*) malloc (_CFG_LINE_LEN_ * sizeof(char));
  tmp1 = (char*) malloc (_CFG_LINE_LEN_ * sizeof(char));

  while (i < 100) {
    fgets(line, _CFG_LINE_LEN_, f);

    if (feof(f))
      break;

    // For comment lines.
    if (line[0] == '#')
      pch = NULL;
    
    // Check if the line actually assigns some value to the option.
    else
      pch = strchr(line, '=');

    // Ignore all lines that were comments or malformed configuration.
    if (pch != NULL) {
      bzero(opt, _CFG_LINE_LEN_);
      strncpy(opt, line, pch-line);

      // Get the value assigned to the option.
      val = atoi(pch+1);

      /* Treat unrecognized values as false (0) and save the names of said
         options for reporting them later. */
      if (val != 0 && val != 1) {
        tmpcat(tmp1, opt);
        val = 0;
      }

      if (strcmp(opt, "greet") == 0)
        cf->greet = val;

      else if (strcmp(opt, "print") == 0)
        cf->print = val;

      else if (strcmp(opt, "log_access") == 0)
        cf->log_access = val;

      else if (strcmp(opt, "cla") == 0)
        cf->cla = val;

      else if (strcmp(opt, "cls") == 0)
        cf->cls = val;

      else if (strcmp(opt, "os_dtls") == 0)
        cf->os_dtls = val;

      // Save names of unrecognized options for reporting them later.
      else
        tmpcat(tmp, opt);
    }

    i++;
  }

  strcpy(cf->uopts, tmp);
  strcpy(cf->bvals, tmp1);
  cf->opt_amt = i;

  printf("Configuration file processsed and options saved.\n");

  res = fclose(f);

  free(tmp);
  free(tmp1);
  free(opt);

  if (res != 0) {
    printf("Couldn't close configuration file %s: %s.\n",
           _CFG_FILENAME_, strerror(errno));

    return -1;
  }

  return 0;
}

void tmpcat(char *tmp, char *opt)
{
  int len = (int) (strlen(tmp) + strlen(", ") + strlen(opt));

  if (strstr(tmp, " and more.") != NULL)
    len = len +1 -1; // Do nothing.

  else if (strlen(tmp) > (size_t) 0) {
    if (len > (_CFG_LINE_LEN_ - 10))
      strcat(tmp, " and more.");

    else {
      strcat(tmp, ", ");
      strcat(tmp, opt);
    }
  }
        
  else
    strcat(tmp, opt);
}

void prt_opt(struct cfg *cf)
{
  printf("Printing options...\n");
  printf("Print options? %s\n", cf->print ? "True." : "False.");
  printf("Show greeting? %s\n", cf->greet ? "True." : "False.");
  printf("Log access? %s\n", cf->log_access ? "True." : "False.");
  printf("Check of last access from current user? %s\n",
         cf->cla ? "True." : "False.");
  printf("Check log sizes? %s\n", cf->cls ? "True." : "False.");
  printf("Get OS details? %s\n", cf->os_dtls ? "True." : "False.");
}
