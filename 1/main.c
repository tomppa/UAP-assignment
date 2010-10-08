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

  if (cf.log_access)
    log_access(pw->pw_name);

  if (cf.cls)
    cls();

  if (cf.ll_cla)
    ll_cla();

  if (cf.hl_cla)
    hl_cla();

  if (cf.os_dtls) {
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
