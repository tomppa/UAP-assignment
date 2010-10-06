// just general stuff building up for the assignment

#include "stuff.h"
#include "config.h"

int main (int argc, char **argv)
{
  struct passwd *pw = getpwuid(geteuid());
  struct utsname os;
  int found = uname(&os);
  cfg cf;

  if (found == 0) 
     check_platform(pw->pw_shell, os.sysname, os.release);
 
  else
    printf("Couldn't get OS details: %s.\n", strerror(errno));

  //ll_access();
  //hl_access();
  log_access(pw->pw_name);
  cls();

  process_cfg(&cf);

  if (cf.greet == 1) {
    if (greet(pw->pw_name, pw->pw_dir) < 0)
      printf("Problems getting all the info for a proper greeting.\n");
  }

  if (strlen(cf.uopts) > (size_t) 0)
    printf("Following options in configuration were unknown: %s.\n",
           cf.uopts);

  return 0;
}
