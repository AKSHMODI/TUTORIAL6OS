#define _XOPEN_SOURCE 700 // required for barriers to work
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>

#define ARRAY_LENGTH 5

// array to store sums
int moving_sum[ARRAY_LENGTH] = {0};

// Structure to pass arguments to the thread
struct ThreadArg {
    int number;
    int index;
};

// Semaphore for synchronization
sem_t semaphore;

// Function to calculate factorial
int factorial(int n) {
    if (n <= 1)
        return 1;
    else
        return n * factorial(n - 1);
}

// Thread function
void *calculate_factorial(void *arg) {
    struct ThreadArg *thread_arg = (struct ThreadArg *)arg;
    int index = thread_arg->index;
    int number = thread_arg->number;
    int fact = factorial(number);

    // If index is not the first element, wait for the previous value in moving_sum
    if (index > 0) {
        while (moving_sum[index - 1] <= 0) {
            sem_wait(&semaphore); // Wait until the previous value is available
        }
        fact += moving_sum[index - 1];
    }
    moving_sum[index] = fact;
    sem_post(&semaphore); // Release the semaphore for the next thread
    pthread_exit(NULL);
}

int main(void) {
    pthread_t threads[ARRAY_LENGTH];
    struct ThreadArg args[ARRAY_LENGTH];

    // Initialize semaphore
    sem_init(&semaphore, 0, 1);

    // Prompt user for five numbers
    printf("Enter five numbers:\n");
    for (int i = 0; i < ARRAY_LENGTH; i++) {
        scanf("%d", &args[i].number);
        args[i].index = i;
    }

    // Create threads
    for (int i = 0; i < ARRAY_LENGTH; i++) {
        pthread_create(&threads[i], NULL, calculate_factorial, (void *)&args[i]);
    }

    // Join threads
    for (int i = 0; i < ARRAY_LENGTH; i++) {
        pthread_join(threads[i], NULL);
    }

    // Print the contents of moving_sum
    printf("Moving sum:\n");
    for (int i = 0; i < ARRAY_LENGTH; i++) {
        printf("%d ", moving_sum[i]);
    }
    printf("\n");

    // Destroy semaphore
    sem_destroy(&semaphore);

    return 0;
}

