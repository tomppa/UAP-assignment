// Class for all sorts of different stuff that hasn't found a proper home yet.
#include <stdio.h>
#include <stdlib.h>

#include <dirent.h>
#include <fcntl.h>
#include <regex.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include <sys/stat.h>
#include <sys/types.h>
#include <sys/time.h>

#include "stuff.h"

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

  return 0;
}

// Checking whether time2 is later than time1 and if so
// updating time2 to equal time1
int update(char *time1, char *time2)
{
  struct tm tm1, tm2;

  strptime(time1, _TIME_PTRN_, &tm1);
  strptime(time2, _TIME_PTRN_, &tm2);

  if (difftime(mktime(&tm1), mktime(&tm2))) {
    time1 = strcpy(time1, time2);
    return 1;
  }

  else
    return 0;
}

void chk_pf(char *shell, char *os, char *release)
{
}

// Writing a timestamp and user record into access log.
int log_access(char *login) {
  int fd, n;
  char text[40];
  struct tm *tm;
  struct timeval tv;
  char host[11];
  mode_t mode;

  memset(host, 0x00, sizeof(host));
  gethostname(host, sizeof(host) - 1);

  gettimeofday(&tv, NULL);

  if ((tm = gmtime(&tv.tv_sec)) == NULL)
    return 1;

  n = sprintf(text, "%04d-%02d-%02d %02d:%02d:%02d\t%s\t%s\n",
              tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday, tm->tm_hour,
              tm->tm_min, tm->tm_sec, host, login);

  mode = S_IWUSR | S_IRUSR | S_IRGRP | S_IROTH;

  if ((fd = open(_ACCESS_LOG_, O_RDWR | O_CREAT | O_APPEND, mode)) == -1)
    perror(_ACCESS_LOG_);

  else {
    if (write(fd, text, n) < n)
      fprintf(stderr, "Less bytes written than expected!");    

    close(fd);
  }

  return 0;
}

// Low level way of checking last access from current user.
int ll_cla()
{
  int fd, n, i = 0, j = 0;
  char buf[_BUF_SIZE_], *last, *cur, *line, *logname, *loc;
  char *tmp;
  regex_t re;

  cur  = (char*) malloc (_TIME_LEN_ * sizeof(char));
  last = (char*) malloc (_TIME_LEN_ * sizeof(char));
  line = (char*) malloc (_LINE_LEN_ * sizeof(char));
  tmp  = (char*) malloc (_LINE_LEN_ * sizeof(char));
  
  bzero(cur, _TIME_LEN_);
  bzero(last, _TIME_LEN_);
  bzero(tmp, _LINE_LEN_);
 
  if ((fd = open(_ACCESS_LOG_, O_RDONLY)) == -1) {
    perror(_ACCESS_LOG_);
    return 1;
  }

  logname = getenv("LOGNAME");

  while ((n = read(fd, buf, _BUF_SIZE_)) > 0) {
    while (i < n) {
      for (i = j; i < j+_LINE_LEN_; i++) {
        line[i-j] = buf[i];

        if (buf[i] == '\n') {
          j = i+1;
          break;
        }
      }

      if (regcomp(&re, logname, REG_EXTENDED|REG_NOSUB) == 0 &&
          regexec(&re, line, (size_t) 0, NULL, 0) == 0) {
        cur = strncpy(cur, line, 19);

        if (update(last, cur)) {
          bzero(tmp, _LINE_LEN_);
          tmp = strcpy(tmp, line);
        }
      }
    }
  }

  if (last[0] != '\0') {
    char *start = strchr(tmp, '\t');
    char *stop = strchr(start+1, '\t');
    int host_len = stop-(start+1);
    
    loc = (char*) malloc (host_len * sizeof(char));
    bzero(loc, host_len);
    loc = strncpy(loc, start+1, host_len);
  
    printf("You last accessed this program %s on %s.\n", last, loc);
    free(loc);
  }

  else
    printf("You haven't used this program yet or have been sneaky and "
            "turned access logging off!\n");

  free(tmp);
  free(cur);
  free(last);

  return 0;
}

// High level way of checking last access from current user.
int hl_cla ()
{
  FILE *pFile;
  char line[_LINE_LEN_], *cur, *last, *tmp;
  int i = 0;

  last = (char*) malloc (_LINE_LEN_ * sizeof(char));

  pFile = fopen(_ACCESS_LOG_, "r");
  if (pFile == NULL)
    perror("Error opening up access log.");

  else {
    while (i < 20)
    {
      tmp = fgets(line, _LINE_LEN_, pFile);
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

  free(last);
  return 0;
}

// Checking the combined size of all log files.
int cls ()
{
  DIR *dir;
  struct dirent *de;
  struct stat st;
  off_t size;
  char path[_MAXPATH_];

  if ((dir = opendir(_LOGPATH_)) == NULL) {
    perror(path);
    return 1;
  }

  size = 0;

  while ((de = readdir(dir)) != NULL) {
    if (snprintf(path, _MAXPATH_, "%s%s", _LOGPATH_, de->d_name) < _MAXPATH_) {
      if (lstat(path, &st))
        perror(path);

      else if ((st.st_mode & S_IFMT) == S_IFREG)
        size += st.st_size;
    }
  }

  printf("Size of log directory is: %d\n", (int) size);

  return 0;
}

