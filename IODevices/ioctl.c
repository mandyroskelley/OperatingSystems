#include <sys/ioctl.h>
#include <fcntl.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>

int main(int argc, char *argv[])
{
  int fd;
  struct winsize win;
  struct winsize win2;

  fd = open("/dev/ttys000", O_RDONLY);
  if (fd < 0) {
    fprintf(stderr, "Could not open file /dev/ttys000: %s\n", strerror(errno));
    return EXIT_FAILURE;
  }

  printf("%d\n" , isatty(fd));

  ioctl(fd, TIOCGWINSZ, &win);

  printf("Original Size: %d rows  %d columns\n" , win.ws_row, win.ws_col);
  
  win.ws_row = 20;
  win.ws_col = 76;
  ioctl(fd, TIOCSSIZE, &win);

  ioctl(fd, TIOCGWINSZ, &win2);

  printf("Updated Size: %d rows  %d columns\n" , win2.ws_row, win2.ws_col);

  close(fd);
  return EXIT_SUCCESS;

}
