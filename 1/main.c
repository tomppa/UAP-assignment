// just general stuff building up for the assignment
#include <stdio.h>

#include <errno.h>
#include <pwd.h>
#include <string.h>
#include <unistd.h>

#include <sys/utsname.h>

#include "stuff.h"
#include "config.h"

int main (int argc, char **argv)
{
  struct passwd *pw = getpwuid(geteuid());
  struct cfg cf;

  process_cfg(&cf);
  
  printf("Processing the options.\n");
  
  if (cf.print) {
    printf("Print was enabled!\n");
    prt_opt(&cf);
  }

  if (cf.greet) {
    if (greet(pw->pw_name, pw->pw_dir) < 0)
      printf("Problems getting all the info for a proper greeting.\n");
  }

  if (cf.log_access) {
    printf("Appending access log.\n");
    log_access(pw->pw_name);
  }

  if (cf.cls) {
    printf("Checking log sizes.\n");
    cls();
  }

  if (cf.ll_cla) {
    printf("Performing low level search for last sighting of this user.\n");
    ll_cla();
  }

  if (cf.hl_cla) {
    printf("Performing high level search for last sighting of this user.\n");
    hl_cla();
  }

  if (cf.os_dtls) {
    printf("Fetching some details about OS being used right now.\n");
    struct utsname os;
    int found = uname(&os);

    if (!found) 
      chk_pf(pw->pw_shell, os.sysname, os.release);
 
    else
      printf("Couldn't get OS details: %s.\n", strerror(errno));
  }

  if (strlen(cf.uopts) > (size_t) 0)
    printf("Following options in configuration were unknown: %s.\n",
           cf.uopts);
  
  return 0;
}
