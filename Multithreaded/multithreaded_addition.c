#include <time.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>

#define NUM_COUNT 10000
#define THREAD_NUMBER 5

// Define a data structure that contains the indices representing the bounds
// of the array to add together.  The sum field contains the sum of the numbers
// within the bounds of the array between 'begin' and 'end'. Placing this field
// inside the struct, which is accessible from the main program, is easier than
// getting the return value of the thread function. 

struct limits {
  int begin;
  int end;
  int sum;
};


// The array containing NUM_COUNT number of integers to add together

int numbers[NUM_COUNT];
int totalsum = 0;

// The thread function that adds the numbers in the array starting with the
// begin field of the args struct (note it's a void* cast to a (struct limits*))
// through the index represented by the end field (inclusive). 

void* add_numbers(void* args) {
  int first = ((struct limits*)args)->begin;
  int last = ((struct limits*)args)->end;
  
  for (int i = first; i <= last; i++) {
    ((struct limits*)args)->sum += numbers[i];
  }

  totalsum += ((struct limits*)args)->sum;

  pthread_exit(NULL);
  
}

int main(int argc, char** argv) {
  struct limits* zero = (struct limits*)malloc(sizeof(struct limits));
  struct limits* one = (struct limits*)malloc(sizeof(struct limits));
  struct limits* two = (struct limits*)malloc(sizeof(struct limits));
  struct limits* three = (struct limits*)malloc(sizeof(struct limits));
  struct limits* four = (struct limits*)malloc(sizeof(struct limits));
  int correct_total = 0;
  int subset = 0;
  int thread_created;
  void* result;
  
  // Seed the pseudorandom number generator with the current system time
  srand(time(0));

  // Populate the numbers array with pseudorandom ints between 1 and 100
  // and tally the correct total so we can check for program correctness
  for (int i = 0; i < NUM_COUNT; i++) {
    numbers[i] = (rand() % 100) + 1;
    correct_total += numbers[i];
  }

  printf("The correct sum is: %d\n", correct_total);

  pthread_t thread_num[THREAD_NUMBER];
 
  subset = (NUM_COUNT / THREAD_NUMBER);

  for (int i = 0; i < THREAD_NUMBER; i++) {
    switch (i) {
      case 0:
        zero->begin = (i * subset);
        zero->end = ((i + 1) * subset) - 1;
        thread_created = pthread_create(&thread_num[i], NULL, add_numbers, (void*)zero);
        if (thread_created != 0) {
          fprintf(stderr, "Error creating thread: %s\n", strerror(errno));
          exit(0);
        }
        break;
      case 1:
        one->begin = (i * subset);
        one->end = ((i + 1) * subset) - 1;
        thread_created = pthread_create(&thread_num[i], NULL, add_numbers, (void*)one);
        if (thread_created != 0) {
          fprintf(stderr, "Error creating thread: %s\n", strerror(errno));
          exit(0);
        }
        break;
      case 2:
        two->begin = (i * subset);
        two->end = ((i + 1) * subset) - 1;
        thread_created = pthread_create(&thread_num[i], NULL, add_numbers, (void*)two);
        if (thread_created != 0) {
          fprintf(stderr, "Error creating thread: %s\n", strerror(errno));
          exit(0);
        }
        break;
      case 3:
        three->begin = (i * subset);
        three->end = ((i + 1) * subset) - 1;
        thread_created = pthread_create(&thread_num[i], NULL, add_numbers, (void*)three);
        if (thread_created != 0) {
          fprintf(stderr, "Error creating thread: %s\n", strerror(errno));
          exit(0);
        }
        break;
      case 4:
        four->begin = (i * subset);
        four->end = ((i + 1) * subset) - 1;
        thread_created = pthread_create(&thread_num[i], NULL, add_numbers, (void*)four);
        if (thread_created != 0) {
          fprintf(stderr, "Error creating thread: %s\n", strerror(errno));
          exit(0);
        }
        break;
      default:
        fprintf(stderr, "Error creating thread: %s\n", strerror(errno));
        exit(0);
    }
  }

  for (int i = 0; i < THREAD_NUMBER; i++) {
    pthread_join(thread_num[i], &result);
  }

  printf("Threaded sum is: %d\n", totalsum);

  return EXIT_SUCCESS;
}
