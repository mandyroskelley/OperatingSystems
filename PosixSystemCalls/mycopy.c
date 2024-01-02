#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>

#define BUFFER_SIZE 2000000

int main(int argc, char** argv) {
	int readfd,
		writefd,
		result,
		rcount,
		readable;

	char buffer[BUFFER_SIZE];

	if(argc != 3) {
		printf("mycopy: invalid number of arguments\n");
		printf("Usage: mycopy: <source> <target>");
		exit(0);
	}

	readfd = open(argv[1], O_RDONLY, 0);
	readable = access(argv[1], R_OK);

	if (readfd < 0) {
		fprintf(stderr, "Unable to open %s: %s\n", argv[1], strerror(errno));
		exit(0);
	}

	if (readable == -1) {
		fprintf(stderr, "Unable to write to file %s: %s\n", argv[2], strerror(errno));
		exit(0);
	}

	writefd = creat(argv[2], S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
	
	rcount = read(readfd, buffer, BUFFER_SIZE);
	result = rcount;

	while (result > 0) {
		write(writefd, buffer, result);
		result = read(readfd, buffer, BUFFER_SIZE);
	}

	if (writefd > 0) {
		printf("copied %d" , rcount);
  		printf(" bytes from file %s" , argv[1]);
  		printf(" to %s\n" , argv[2]);
	}

	close(writefd);
	close(readfd);

	return 0;
}
