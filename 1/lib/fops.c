// Generic file operations.
#include <stdio.h>
#include <stdlib.h>

#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>

// Open up the pipe required for communicating with other processes.
int opipe(int *fd, char *name)
{
  if (mkfifo(name, 0600) == -1) {
    if (errno == EEXIST)
      printf("Warning: FIFO already exists, it could be in use.\n");

    else {
      perror("Failed to create FIFO");
      return -1;
    }
  }
  
  if (errno != EEXIST)
    printf("FIFO created.\n");

  // Open FIFO with nonblocking for asynchronic I/O.
  if ((*fd = open(name, O_RDWR|O_NONBLOCK)) == -1) {
    perror("Failed to open FIFO");
    return -1;
  }


  printf("FIFO opened.\n");

  return 0;
}

// Close down the pipe used to communicate with the rest of the world.
int cpipe(int *fd)
{
  if (close(*fd) == -1) {
    perror("Failed to close down FIFO");
    return -1;
  }

  *fd = -1;

  printf("FIFO closed.\n");

  return 0;
}

// Open the file lock to show this process has terminated.
int olck(int *fd)
{
  struct flock fl;

  memset(&fl, 0, sizeof(fl));

  if (fcntl(*fd, F_GETLK, &fl) < 0) {
    perror("Problem probing lockfile for locks");
    return -1;
  }

  if (fl.l_type == F_UNLCK) {
    fl.l_type = F_UNLCK;

    if (fcntl(*fd, F_SETLK, &fl) < 0) {
      perror("Problem unlocking lockfile");
      return -1;
    }

    printf("Lockfile unlocked.\n");
  }

  else
    printf("Lock encountered by pid %d, so cannot unlock the file.\n",
           fl.l_pid);

  close(*fd);
  *fd = -1;
  printf("Lockfile descriptor closed.\n");

  return 0;
}

// Close the file (lock it down) indicating this daemon is operational.
int clck(int *fd, char *name)
{
  *fd = open(name, O_WRONLY|O_CREAT|O_TRUNC, 0600);

  if (*fd < 0) {
    perror("Problem opening lockfile");
    return -1;
  }

  printf("Lockfile opened/created.\n");

  struct flock fl;

  memset(&fl, 0, sizeof(fl)); // initialize fl

  fl.l_type = F_WRLCK;

  if (fcntl(*fd, F_SETLKW, &fl) < 0) {
    if (errno == ENOLCK) {
      printf("File system doesn't support locking.");
      return -2;
    }

    perror("Problem locking lockfile");
    return -1;
  }

  printf("Lock on lockfile acquired.\n");

  return 0;
}

// Check whether log directory exists, if not create it.
int chk_log(char *path)
{
  struct stat st;

  if (stat(path, &st) != 0) {
    printf("Directory 'logs' not found, creating it ...");

    if (mkdir(path, S_IRWXU | S_IRGRP | S_IROTH) < 0) {
      perror("Directory creation failed");
      return -1;
    }

    else
      fprintf(stdout, " creation successfull.\n");
  }

  return 0;
}

int chk4lck(char *name, int *pid)
{
  struct flock fl;
  int success = 0, fd = 0;
  
  fd = open(name, O_RDONLY);

  printf("Lockfile (%s) opened.\n", name);

  if (fd < 0) {
    perror("Problem opening lockfile");
    return -1;
  }

  memset(&fl, 0, sizeof(fl));

  if (fcntl(fd, F_GETLK, &fl) < 0) {
    if (errno == ENOLCK) {
      printf("Locking not supported by file system.\n");
      return -2;
    }

    perror("Problem probing lockfile for locks");
    return -1;
  }

  if (fl.l_type == F_UNLCK) {
    printf("No lock found.\n");
    success = 0;
  }

  else {
    printf("File locked by process with pid: %d.\n", fl.l_pid);
    *pid = fl.l_pid;
    success = 1;
  }

  close(fd);

  printf("Closed lockfile.\n");

  return success;
}

int chk4pid(char *name, int size, int *pid)
{
  int fd = 0;
  char buf[size];

  fd = open(name, O_RDONLY);

  if (fd < 0) {
    perror("Problem opening lockfile");
    return -1;
  }

  if (read(fd, buf, size) < 0) {
    perror("Problem reading lockfile");
    return -1;
  }

  *pid = atoi(buf);

  close(fd);

  return 0;
}

int wr_pid (int fd, int pid)
{
  char tmp[10];

  if (sprintf(tmp, "%d", pid) < 0) {
    fprintf(stderr, "Failed to convert pid to string.\n");
    return -1;
  }
  
  write(fd, tmp, 10);

  printf("PID written successfully.\n");

  return 0;
}

