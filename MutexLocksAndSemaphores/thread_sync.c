#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <semaphore.h>

#define CONCURRENT_THREADS 10000
#define DATA_SIZE          5

static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
static sem_t           semaphore;
static int             fd;
static int             bytes_produced_consumed = 0;
static int             ready_to_read = 0;

// This is the function called by each writer thread.  Each thread will take
// the following steps:
//
// 1. Construct a char array containing the thread's ID number. The content must
// be justified to exactly five characters regardless of the number of digits.
// Hint: consider using sprintf.
// 2. Call wait on the semaphore
// 3. Acquire the mutex lock
// 4. Do a pwrite of the char array on the file at the the thread's designated offset
// 5. Increment the value of bytes_produced_consumed by DATA_SIZE number of bytes
// 6. Release the mutex lock
// 7. Call signal on the semaphore
void* writer_thread(void* arg) {
  int  thread_id = *(int*)arg;
  char buffer[DATA_SIZE+1];
  int  writer;

  sprintf(buffer, "%04d " , thread_id);
  
  sem_wait(&semaphore);
  printf("Writer thread %d called semaphore wait\n", thread_id);

  pthread_mutex_lock(&mutex);
  printf("Writer thread %d acquired mutex lock\n", thread_id);

  writer = pwrite(fd, buffer, DATA_SIZE, (thread_id * DATA_SIZE));
  if (writer == -1) {
    fprintf(stderr, "Error writing to file: %s\n", strerror(errno));
    exit(0);
  }

  bytes_produced_consumed += DATA_SIZE;
  pthread_mutex_unlock(&mutex);
  printf("Writer thread %d released mutex lock\n", thread_id);
  ready_to_read = bytes_produced_consumed;

  sem_post(&semaphore);
  printf("Writer thread %d called semaphore signal\n", thread_id);
}

// This is the function called by each reader thread.  Each thread will take
// the following steps:
//
// 1. Call wait on the semaphore
// 2. Acquire the mutex lock
// 4. Do a pread of the char array on the file at the the thread's designated offset
// 5. Decrement the value of bytes_produced_consumed by DATA_SIZE number of bytes
// 5. Release the mutex lock
// 6. Call signal on the semaphore
// 7. Output the result of the pread
void* reader_thread(void* arg) {
  int  thread_id = *(int*)arg;
  char buffer[DATA_SIZE];
  int  reader;

  if (ready_to_read == (CONCURRENT_THREADS * DATA_SIZE)){
  sem_wait(&semaphore);
  printf("Reader thread %d called semaphore wait\n", thread_id);
  
  pthread_mutex_lock(&mutex);
  printf("Reader thread %d acquired mutex lock\n", thread_id);
  reader = pread(fd, buffer, DATA_SIZE, (thread_id * DATA_SIZE));
  if (reader == -1) {
    fprintf(stderr, "Error reading to file: %s\n", strerror(errno));
    exit(0);
  }
  bytes_produced_consumed -= DATA_SIZE;
     
  printf("Reader thread %d released mutex lock\n", thread_id);
  pthread_mutex_unlock(&mutex);

  printf("Reader thread %d called semaphore signal\n", thread_id);
  sem_post(&semaphore);

  printf("Thread %d read: %s\n", thread_id, buffer);
}
}
  


int main() {
  // Initialize semaphore with a value of 5
  sem_init(&semaphore, 0, 5);

  // Create an array large enough to contain 10,000 threads
  pthread_t rthreads[CONCURRENT_THREADS];
  pthread_t wthreads[CONCURRENT_THREADS];

  // Open the file that constitutes the shared buffer for reading and writing
  fd = open("sharedfile.txt", O_RDWR | O_CREAT, (mode_t)0644);
  
  if (fd < 0) {
    fprintf(stderr, "Could not open file sharedfile.txt: %s\n", strerror(errno));
    return EXIT_FAILURE;
  }

  // Create 10,000 writer threads
  for (int i = 0; i < CONCURRENT_THREADS; i++) {
    pthread_create(&wthreads[i], NULL, writer_thread, (void*)&i);
    // The use of pthread_join here isn't great, but it helps avoid certain
    // undefined behaviors on some platforms
    pthread_join(wthreads[i], NULL);
  }

  printf("%d bytes written by writer threads\n", bytes_produced_consumed);
    
  // Create 10,000 reader threads
  for (int i = 0; i < CONCURRENT_THREADS; i++) 
    pthread_create(&rthreads[i], NULL, reader_thread, (void*)&i);
  
  // Wait on all reader threads
  for (int i = 0; i < CONCURRENT_THREADS; i++) 
    pthread_join(rthreads[i], NULL);

  printf("%d bytes remaining after reader threads\n", bytes_produced_consumed);
  
  // Tear down the semaphore and close the file
  sem_destroy(&semaphore);
  close(fd);
  
  pthread_exit(NULL);
}
