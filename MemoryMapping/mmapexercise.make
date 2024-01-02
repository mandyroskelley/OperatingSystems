OS=$(shell uname)
ifeq ($(OS), Darwin)
	CFLAGS += -D_XOPEN_SOURCE=700
endif

# Specify the compiler
CC=/usr/bin/gcc

# Build executable from object file
mmapexercise: mmapexercise.o
	$(CC) -o mmapexercise mmapexercise.o

# Build object file from source file
mmapexercise.o: mmapexercise.c
	$(CC) $(CFLAGS) -c mmapexercise.c

# Additional rule to clean up 
.PHONY: clean
clean:
	rm -f mmapexercise mmapexercise.o mmapexercise.c~ mymmapfile.txt
