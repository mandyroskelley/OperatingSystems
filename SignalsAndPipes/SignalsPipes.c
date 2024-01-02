#include <signal.h>
 #include <unistd.h>
 #include <errno.h>
 #include <stdlib.h>
 #include <stdio.h>
 #include <string.h>
 #include <sys/wait.h>

#define BUFFER_SIZE 1000
#define READ_END 0
#define WRITE_END 1

void the_handler (int sig_number) {
    fprintf(stdout, "Parent process (pid %d) sending signal SIGUSRI to child (pid %d)\n", getppid(), getpid());
}

int main(int argc, char** argv) {
    int i,
        file[2];
    char write_msg[] = "Hello World",
        read_msg[BUFFER_SIZE];

  if (signal(SIGUSR1, the_handler) == SIG_ERR) {
    fprintf(stderr, "Error: Signal Handler Failed: %s\n", strerror(errno));
    exit(0);
  }

  if(pipe(file) == -1) {
    fprintf(stderr, "Error: Pipe Failed");
    exit(0);
  }  

  pid_t pid = fork();
  signal(SIGUSR1, the_handler);

  if (pid < 0) {
    fprintf(stderr, "Error: Fork Failed: %s\n", strerror(errno));
    exit (0);
  }

  if (pid == 0) {
    i =3;
    while (i>0) {
        fprintf(stdout, "Child process (pid %d) is waiting\n", getpid());
        sleep(1);
        i--;
    } 
    fflush(stdout);
    kill(getpid(), SIGUSR1);
    close(file[WRITE_END]);
    read(file[READ_END], read_msg, BUFFER_SIZE);
    fprintf(stdout, "Signal SIGUSR1 received by child process (pid %d)\n" , getpid());
    fprintf(stdout, "Child process (pid %d) received message \"%s\" via pipe \n", getpid(), read_msg);
    fprintf(stdout, "Child process (pid %d) exiting\n" , getpid());
    
  } 

  else {
    fprintf(stdout, "Parent process (pid %d) sleeping for 3 seconds\n", getpid());
    sleep(3);
    fflush(stdout);
    fprintf(stdout, "Parent process (pid %d) sending message \"Hello World\" via pipe\n", getpid());
    fprintf(stdout, "Parent process (pid %d) waiting on child\n", getpid());
    close(file[READ_END]);
    write(file[WRITE_END], write_msg, strlen(write_msg) +1);
    wait(NULL);
    fprintf(stdout, "Parent process (pid %d) exiting\n", getpid());

  }

  return EXIT_SUCCESS;
}
