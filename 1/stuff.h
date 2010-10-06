// library file for stuff

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <regex.h>
#include <sys/types.h>
#include <pwd.h>
#include <grp.h>
#include <unistd.h>
#include <regex.h>
#include <sys/utsname.h>
#include <sys/stat.h>
#include <time.h>
#include <fcntl.h>
#include <sys/time.h>
#include <dirent.h>

#ifndef BUF_SIZE
#define BUF_SIZE 0x400
#endif

#ifndef LINE_LENGTH
#define LINE_LENGTH 41
#endif

#ifndef ACCESS_LOG
#define ACCESS_LOG "./logs/access.log"
#endif

#ifndef TIME_PTRN
#define TIME_PTRN "%Y-%m-%d %H:%M:%S"
#endif

#ifndef MAXPATH
#define MAXPATH 1024
#endif

#ifndef LOGPATH
#define LOGPATH "./logs/"
#endif

#ifndef GREET
#define GREET
// Simple greeting function.
int greet (char *login, char *home_path)
{
  regex_t re;
  char* pattern = "^.*2$";
  struct stat st;

  if (regcomp(&re, pattern, REG_EXTENDED|REG_NOSUB) != 0) {
    return 1;
  }

  printf("Hello %s! ", login);

  if (regexec(&re, pattern, (size_t) 0, NULL, 0) != 0)
    printf("Is this your evil twin personality?");

  printf("\n");

  if (stat(home_path, &st)) {
    perror(home_path);
    return 1;
  }

//  printf("Details of your home directory (%s) are as follows.\n",
//         home_path);
//  printf("Last accessed:  %s", ctime(&st.st_atime));
//  printf("Last modified:  %s", ctime(&st.st_mtime));
//  printf("Last changed:   %s", ctime(&st.st_ctime));

  return 0;
}
#endif

// Checking whether time2 is later than time1 and if so
// updating time2 to equal time1
void update(char *time1, char *time2)
{
  struct tm tm1, tm2;

  strftime(time1, sizeof(time1), TIME_PTRN, &tm1);
  strftime(time2, sizeof(time2), TIME_PTRN, &tm2);

  if (difftime(mktime(&tm1), mktime(&tm2)))
    time1 = strdup(time2);
}

void check_platform(char *shell, char *os, char *release)
{
}

// Writing a timestamp and user record into access log.
int log_access(char *login) {
  int fd, n;
  char text[40];
  struct tm *tm;
  struct timeval tv;
  char host[11];

  memset(host, 0x00, sizeof(host));
  gethostname(host, sizeof(host) - 1);

  gettimeofday(&tv, NULL);

  if ((tm = gmtime(&tv.tv_sec)) == NULL)
    return 1;

  n = sprintf(text, "%04d-%02d-%02d %02d:%02d:%02d\t%s\t%s\n",
              tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday, tm->tm_hour,
              tm->tm_min, tm->tm_sec, host, login);

  if ((fd = open(ACCESS_LOG, O_RDWR | O_CREAT | O_APPEND)) == -1)
    perror(ACCESS_LOG);

  else {
    if (write(fd, text, n) < n)
      fprintf(stderr, "Less bytes written than expected!");    

    close(fd);
  }

  return 0;
}

// Low level way of reading the access log.
int ll_access()
{
  int fd, n, i, j = 0;
  char buf[BUF_SIZE], *last, *cur, tmp[LINE_LENGTH], *logname;
  regex_t re;
 
  if ((fd = open(ACCESS_LOG, O_RDONLY)) == -1) {
    perror(ACCESS_LOG);
    return 1;
  }

  logname = getenv("LOGNAME");

  while ((n = read(fd, buf, BUF_SIZE)) > 0) {
    while (j < n-1 && i < 700) {
    for (i = j; i < j+LINE_LENGTH; i++) {
      tmp[i-j] = buf[i];

      if (buf[i] == '\n') {
        j = i+1;
        break;
      }
    }

    //printf("tmp: %s\n", tmp);

    if (regcomp(&re, logname, REG_EXTENDED|REG_NOSUB) == 0 &&
        regexec(&re, logname, (size_t) 0, NULL, 0) == 0) {
      cur = strndup(tmp, (size_t) 19);

      printf("cur: %s & last: %s\n", cur, last);

      if (last == NULL)
        last = cur;
      else
        update(last, cur);
    }

    free(cur);
    }
  }

  printf("You last accessed this program: %s\n", last);

  free(last);

  return 0;
}

// High level way of reading the access log.
int hl_access ()
{
  FILE *pFile;
  char line[LINE_LENGTH], *cur, *last;
  int i = 0;

  pFile = fopen(ACCESS_LOG, "r");
  if (pFile == NULL)
    perror("Error opening up access log.");

  else {
    while (i < 20)
    {
      fgets(line, LINE_LENGTH, pFile);
      cur = strndup(line, (size_t) 19);
      printf("cur: %s, last: %s\n", cur, last);
      
      if (last == NULL)
        last = cur;
      else
        update(cur, last);

      if (feof(pFile))
        break;
      i++;
    }

    fclose(pFile);
  }

  return 0;
}

// Checking the combined size of all log files.
int cls ()
{
  DIR *dir;
  struct dirent *de;
  struct stat st;
  off_t size;
  char path[MAXPATH];

  if ((dir = opendir(LOGPATH)) == NULL) {
    perror(path);
    return 1;
  }

  size = 0;

  while ((de = readdir(dir)) != NULL) {
    if (snprintf(path, MAXPATH, "%s%s", LOGPATH, de->d_name) < MAXPATH) {
      if (lstat(path, &st))
        perror(path);

      else if ((st.st_mode & S_IFMT) == S_IFREG)
        size += st.st_size;
    }
  }

  printf("Size of log directory is: %d\n", (int) size);

  return 0;
}

