#define _XOPEN_SOURCE 700 // required for barriers to work
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <pthread.h>
#include <semaphore.h>

#define NUMBERS_FILE "numbers.txt"
#define SUM_FILE "sum.txt"
#define THREADS_COUNT 5

int total_sum = 0;
sem_t semaphore;

void *factorial(void *arg) {
    int number = *((int *)arg);
    int fact = 1;
    for (int i = 1; i <= number; ++i) {
        fact *= i;
    }
    sem_wait(&semaphore);
    total_sum += fact;
    sem_post(&semaphore);
    printf("Factorial of %d: %d\n", number, fact);
    printf("Current total sum: %d\n", total_sum);
    pthread_exit(NULL);
}

int main() {
    pid_t child_pid;
    FILE *numbers_file, *sum_file;
    int numbers[THREADS_COUNT];
    pthread_t threads[THREADS_COUNT];
    // Create numbers file and prompt user for input
    numbers_file = fopen(NUMBERS_FILE, "w");
    if (numbers_file == NULL) {
        perror("Error opening numbers file");
        exit(EXIT_FAILURE);
    }
    printf("Enter five numbers:\n");
    for (int i = 0; i < THREADS_COUNT; ++i) {
        scanf("%d", &numbers[i]);
        fprintf(numbers_file, "%d\n", numbers[i]);
    }
    fclose(numbers_file);
    // Initialize semaphore
    sem_init(&semaphore, 0, 1);
    // Fork
    child_pid = fork();
    if (child_pid < 0) {
        // Fork failed
        perror("Fork failed");
        exit(EXIT_FAILURE);
    } else if (child_pid == 0) {
        // Child process
        numbers_file = fopen(NUMBERS_FILE, "r");
        if (numbers_file == NULL) {
            perror("Error opening numbers file");
            exit(EXIT_FAILURE);
        }
        // Read numbers from file and create threads
        for (int i = 0; i < THREADS_COUNT; ++i) {
            fscanf(numbers_file, "%d", &numbers[i]);
            pthread_create(&threads[i], NULL, factorial, (void *)&numbers[i]);
        }
        fclose(numbers_file);
        // Join threads
        for (int i = 0; i < THREADS_COUNT; ++i) {
            pthread_join(threads[i], NULL);
        }
        // Write total sum to file
        sum_file = fopen(SUM_FILE, "w");
        if (sum_file == NULL) {
            perror("Error opening sum file");
            exit(EXIT_FAILURE);
        }
        fprintf(sum_file, "%d\n", total_sum);
        fclose(sum_file);
        exit(EXIT_SUCCESS);
    	} 
    else {
        // Parent process
        wait(NULL); // Wait for child process to terminate
        // Read total sum from file
        sum_file = fopen(SUM_FILE, "r");
        if (sum_file == NULL) {
            perror("Error opening sum file");
            exit(EXIT_FAILURE);
        }
        fscanf(sum_file, "%d", &total_sum);
        fclose(sum_file);
        printf("Total sum: %d\n", total_sum);
        // Clean up files
        remove(NUMBERS_FILE);
        remove(SUM_FILE);
        // Destroy semaphore
        sem_destroy(&semaphore);
    }
    return 0;
}

