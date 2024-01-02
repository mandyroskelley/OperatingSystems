#include <sys/signal.h>
#include <sys/types.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <inttypes.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <limits.h>
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>
#include <signal.h>

#ifdef __CYGWIN__
  #define NUMCHARS  (65537)
#else
  #define NUMCHARS  (12289)
  #include <ucontext.h>
#endif

#define FILESIZE (NUMCHARS * sizeof(char))
#define FILEPATH "mymmapfile.txt"

static int i = 0;
static int fd;
char*  map = 0;

// Code to be executed when a segmentation fault occurs
void segfault_handler(int signum, siginfo_t *info, void *context)
{
  if (signum == SIGSEGV) {
     fprintf(stderr, "Segmentation Fault (SIGSEGV). Memory address = %" PRIuPTR "\n", (uintptr_t)info->si_addr);

     // Replace the following line of code:
     exit(EXIT_FAILURE);
  }
}

// Code to be executed when a bus error occurs
void buserror_handler(int signum, siginfo_t *info, void *context)
{

  if (signum == SIGBUS)
      {
          mprotect((void*)(map + (i * sizeof(char))), 1, PROT_READ | PROT_WRITE);
          fprintf(stderr, "Bus Error (SIGBUS). Memory address = %" PRIuPTR "\n", (uintptr_t)info->si_addr);
	  #ifdef __linux__
       	     fprintf(stderr, "Check your file size\n");
	  #endif
	  //Replace the following line of code:
	  //exit(EXIT_FAILURE);   
    return;	  
         
      }
}

int main(int argc, char *argv[])
{
  struct sigaction sa;
  
  // Open a file (create it if it doesn't exist or truncate to zero bytes if it does)
  fd = open(FILEPATH, O_RDWR | O_CREAT, (mode_t)0600);
  if (fd < 0)
    {
      fprintf(stderr, "%s", strerror(errno));
      exit(EXIT_FAILURE);
    }

  /* Change the action taken in response to a SIGSEGV (segmentation fault) signal. Behavior
     is defined in the function segfault_handler() */ 
  sa.sa_sigaction = segfault_handler;
  sa.sa_flags = SA_SIGINFO;
  if (sigaction(SIGSEGV, &sa, 0) < 0)
    {
      fprintf(stderr, "%s", strerror(errno));
      exit(EXIT_FAILURE);
    }
  
  /* Change the action taken in response to a SIGBUS (bus error) signal. Behavior is defined
     in the function buserror_handler() */ 
  sa.sa_sigaction = buserror_handler;
  sa.sa_flags = SA_SIGINFO;
  if (sigaction(SIGBUS, &sa, 0) < 0)
      {
      fprintf(stderr, "%s", strerror(errno));
      exit(EXIT_FAILURE);
    }

  /* Set the file size to a specific value in bytes by setting the offset to a
     specified length then writing something there */
 if (lseek(fd, FILESIZE-1, SEEK_SET) < 0)
    {
      close(fd);
      fprintf(stderr, "Error calling lseek() to 'stretch' file %s: %s", FILEPATH, strerror(errno));
      exit(EXIT_FAILURE);
    }
  
 /* Need to write *something* at the position of the offset in order to actually
    set the filesize, otherwise the new filesize disappears in a puff of smoke */
  if (write(fd, "", 1) < 0)
    {
      close(fd);
      fprintf(stderr, "Error writing to file %s: %s", FILEPATH, strerror(errno));
      exit(EXIT_FAILURE);
    }

  // Create the memory map from the file we just opened (file descriptor fd)
  map = mmap(0, FILESIZE, PROT_NONE, MAP_SHARED, fd, 0);
  if (map == MAP_FAILED)
    {
      close(fd);
      fprintf(stderr, "%s", strerror(errno));
      exit(EXIT_FAILURE);
    }

  /* What happens if we uncomment the next line? */
  //unlink(FILEPATH);
  
  /* Write FILESIZE number of bytes to the file through the memory map */
 for (i = 0; i < FILESIZE; i++) {
     map[i] = 'a';
     //printf("TEST  %d \n" , i);
  }   
  
  fprintf(stdout, "Wrote %lu bytes to file  \"%s\"\n", FILESIZE, FILEPATH);

  /* We're done writing, so now we un-map the file */
  if (munmap(map, FILESIZE) < 0)
    fprintf(stderr, "%s", strerror(errno));
  
  /*  Un-mapping doesn't close the file, so we still need to do that. */
  if (close(fd) < 0)
    fprintf(stderr, "Error closing file %s\n", FILEPATH);
      
  return 0;
}
