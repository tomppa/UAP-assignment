// just general stuff building up for the assignment

#include "stuff.h"

#ifndef CFG_FILENAME
#define CFG_FILENAME "stuff.cfg"
#endif

int main (int argc, char **argv)
{
  struct passwd *pw;
  struct utsname *os = NULL;
  int found = uname(os);

  pw = getpwuid(geteuid());
  if (found > 0) {
    printf("os: %s, node: %s, release: %s, version: %s, machine: %s\n",
           os->sysname, os->nodename, os->release, os->version,
           os->machine);
  }
  else
    printf("Couldn't get OS details.\n");

  //last_accessed();
  //hl_access();
  log_access(pw->pw_name);
  cls();

  //if (say_hello(pw->pw_name, pw->pw_dir) != 0)
  // printf("Problems getting all the info for a proper greeting.\n");

  check_platform(pw->pw_shell, os->sysname, os->release);
  
  return 0;
}
