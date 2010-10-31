// The code for processing the configuration file.
#include <stdio.h>
#include <stdlib.h>

#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>

#include <sys/mman.h>
#include <sys/stat.h>

#include "config.h"

int process_cfg (struct cfg *cf, int fd)
{
  FILE *f;
  char line[_CFG_LINE_LEN_], *pch, *opt, *tmp, *tmp1;
  int i = 0, val, res;

  // Duplicate the file descriptor, so closing the stream won't kill it.
  f = fdopen(dup(fd), "r");

  if (f == NULL) {
    perror("Couldn't open configuration file");

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
    perror("Couldn't close configuration file");

    return -1;
  }

  printf("Closed the filestream on config file.\n");

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

// Open up config file and map it to memory.
int fmap(int *fd, char **ptr, size_t *mmapsize)
{
  struct stat st;

  *fd = open(_CFG_FILE_, O_RDONLY); 

  printf("Config file opened.\n");

  if (*fd < 0) {
    perror("Opening config file");
    return -1;
  }

  stat(_CFG_FILE_, &st);
  crdtm(&st);

  *mmapsize = st.st_size;
  *ptr = mmap(NULL, *mmapsize, PROT_READ, MAP_PRIVATE, *fd, 0);

  if (ptr == MAP_FAILED) {
    perror("Memory mapping config file");
    return -1;
  }

  printf("Config file (%s) mapped to memory successfully.\n", _CFG_FILE_);

  return 0;
}

// Closing and unmapping the config file from memory.
int fumap(int *fd, char **ptr, size_t *mmapsize)
{
  if (munmap(*ptr, *mmapsize) < 0) {
    perror("Unmapping config file from memory");
    return -1;
  }

  if (close(*fd) < 0) {
    perror("Closing config file");
    return -1;
  }

  printf("Successfully unmapped config file from memory.\n");
  return 0;
}

/* Checks whether file referred by st has been changed since last check, and
   thus whether the file that's mapped into memory is up to date. */
int crdtm(struct stat *st)
{
  /*
  char buf1[80], buf2[80];
  struct tm *ts1, *ts2;

  // TODO: Doesn't work properly yet, so fix it!

  
  ts1 = localtime(&last_read);
  ts2 = localtime(&st->st_ctime);

  strftime(buf1, sizeof(buf1), "%a %Y-%m-%d %H:%M:%S %Z", ts1);
  strftime(buf2, sizeof(buf2), "%a %Y-%m-%d %H:%M:%S %Z", ts2);

  printf("*DEBUG* last_read: %s, st->st_ctime: %s\n", buf2, buf1);

  if (last_read > st->st_ctime) {
    printf("File mapped to memory either doesn't exist or isn't up to date.\n");
    last_read = st->st_ctime;
    return 0;
  }

  printf("File mapped to memory is up to date, no need to remap it.\n");
  
  */
  return -1;
}

// For debugging or gawking at values, when too lazy to view config file itself.
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
