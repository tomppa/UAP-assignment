// just general stuff building up for the assignment
#include <stdio.h>
#include <stdlib.h>

#include <errno.h>
#include <fcntl.h>
#include <pwd.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>

#include <sys/stat.h>
#include <sys/types.h>
#include <sys/utsname.h>

#include "main.h"
#include "stuff.h"
#include "config.h"

int running = 1; // flag to indicate whether to run freely or terminate

static void int_handler(int signo)
{
  printf("Received signal %d\n", signo);
  if (signo == SIGINT)
    running = 0;
}

/* Function for checking whether it's time to shutdown gracefully
   or continue processing commands as normal. */
int csd ()
{
  if (!running) {
    printf("<%d> SIGINT received, shutting down!\n", getpid());
    return 1;
  }

  else
    return 0;
}

int main (int argc, char **argv)
{
  struct passwd *pw = getpwuid(geteuid());
  struct cfg cf;
  struct sigaction sig;
  pid_t pid = 0;
  int pwrfd, prdfd, crdfd, cwrfd, cret, i = 1;
  char np1[] = "fifo1\0", np2[] = "fifo2\0", *pbuf, *cbuf;

  memset(&sig, 0x00, sizeof(sig));
  sigemptyset(&sig.sa_mask); // empty the mask
  sig.sa_handler = int_handler; // set the handler
  sigaction(SIGINT, &sig, NULL); // use handler for SIGINT

  pbuf = (char *) malloc (_MAX_BUF_SIZE_ * sizeof(char));
  cbuf = (char *) malloc (_MAX_BUF_SIZE_ * sizeof(char));
  bzero(pbuf, _MAX_BUF_SIZE_);
  bzero(cbuf, _MAX_BUF_SIZE_);

  printf("<%d> Reading configuration.\n", getpid());
  process_cfg(&cf);
  sleep(2);

  if (csd())
    return 1;

  printf("<%d> Processing the options.\n", getpid());
 
  if ((mkfifo(np1, 0600) == -1 || mkfifo(np2, 0600) == -1) 
      && (errno != EEXIST)) {
    
    perror("Failed to create a FIFO.");
    return 1;
  }

  if ((pid = fork()) == -1) {
    perror("Failed to fork.");
    return 1;
  }

  // Main process.
  if (pid != 0) {
    if ((pwrfd = open(np1, O_WRONLY)) == -1 ||
        (prdfd = open(np2, O_RDONLY)) == -1) {
      perror("Failed to open FIFO");
      return 1;
    }

    while (i <= cf.opt_amt)
    {
      switch (i)
      {
        case 1: if (cf.print)
                  strcpy(pbuf, "print");
                break;

        case 2: if (cf.greet)
                  strcpy(pbuf, "greet");
                break;

        case 3: if (cf.log_access)
                  strcpy(pbuf, "log_access");
                break;

        case 4: if (cf.cls)
                  strcpy(pbuf, "cls");
                break;

        case 5: if (cf.cla)
                  strcpy(pbuf, "cla");
                break;

        case 6: if (cf.os_dtls)
                  break;

        default: break;
      }

      if (csd())
        return 1;

      if (pbuf[0] != '\0') {

        printf("<%d> Sending a command (%s) to child.\n", getpid(), pbuf);
  
        // Write something to first pipe.
        write(pwrfd, pbuf, strlen(pbuf));
        sleep(0);
    
        // Read reply from second pipe.
        read(prdfd, pbuf, _MAX_BUF_SIZE_);
    
        // Process reply.
        if (strcmp(pbuf, "success"))
          printf("<%d> Command delivered and performed!\n", getpid());
        else if (strcmp(pbuf, "failure"))
          printf("<%d> Delivery had some problems!\n", getpid());
        else
          printf("<%d> Received non-standard reply.\n", getpid());

        bzero(pbuf, _MAX_BUF_SIZE_);
        sleep(1);
      }

      i++;
    }
  
    close(pwrfd);
    close(prdfd);
  }
  
  // Forked child process.
  else {
    if ((crdfd = open(np1, O_RDONLY)) == -1 ||
        (cwrfd = open(np2, O_WRONLY)) == -1) {
      perror("Failed to open FIFO");
      return 1;
    }

    sleep(0);

    while (read(crdfd, cbuf, _MAX_BUF_SIZE_) > 0)
    {

      sleep(0);
    
      // Read command from first pipe.
      printf("<%d> Command '%s' received.\n", getpid(), cbuf);
      
      // Process command.
      if (strcmp(cbuf, "greet") == 0)
        cret = greet(pw->pw_name, pw->pw_dir);

      else if (strcmp(cbuf, "print") == 0) {
        prt_opt(&cf);
        cret = 0;
      }

      else if (strcmp(cbuf, "log_access") == 0)
        cret = log_access(pw->pw_name);

      else if (strcmp(cbuf, "cla") == 0)
        cret = cla();

      else if (strcmp(cbuf, "cls") == 0)
        cret = cls();

      else if (strcmp(cbuf, "os_dtls") == 0) {
        struct utsname os;
        int found = uname(&os);
  
        if (!found) {
          chk_pf(pw->pw_shell, os.sysname, os.release);
          cret = 0;
        }
 
        else {
          printf("Couldn't get OS details: %s.\n", strerror(errno));
          cret = 1;
        }
      }

      // Reset buffer.
      bzero(cbuf, _MAX_BUF_SIZE_);

      // Create reply.
      if (cret)
        strcpy(cbuf, "success");
      else
        strcpy(cbuf, "failure");

      // Write reply to second pipe.
      write(cwrfd, cbuf, strlen(cbuf));

      // Reset buffer again.
      bzero(cbuf, _MAX_BUF_SIZE_);
    }

    close(crdfd);
    close(cwrfd);

    exit(1);
  }

  if (strlen(cf.uopts) > (size_t) 0)
    printf("<%d> Following options in configuration were unknown: %s.\n",
           getpid(), cf.uopts);
  
  return 0;
}
