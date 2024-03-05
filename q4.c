#define _XOPEN_SOURCE 700 // required for barriers to work
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define BUFFER_SIZE 5
#define NUMBERS 10

int buffer[BUFFER_SIZE] = {0};
sem_t empty;
sem_t full;
pthread_mutex_t mutex;

void *producer(void *arg) {
    int *numbers = (int *)arg;
    for (int i = 0; i < NUMBERS; ++i) {
        sleep(rand() % 3); // Random delay
        sem_wait(&empty); // Wait if buffer is full
        pthread_mutex_lock(&mutex); // Lock critical section
        for (int j = 0; j < BUFFER_SIZE; ++j) {
            if (buffer[j] == 0) {
                buffer[j] = numbers[i];
                printf("Produced %d\n", numbers[i]);
                break;
            }
        }
        pthread_mutex_unlock(&mutex); // Unlock critical section
        sem_post(&full); // Signal that buffer is no longer empty
    }
    pthread_exit(NULL);
}

void *consumer(void *arg) {
    for (int i = 0; i < NUMBERS; ++i) {
        sleep(rand() % 3); // Random delay
        sem_wait(&full); // Wait if buffer is empty
        pthread_mutex_lock(&mutex); // Lock critical section
        for (int j = 0; j < BUFFER_SIZE; ++j) {
            if (buffer[j] != 0) {
                printf("Consumed %d\n", buffer[j]);
                buffer[j] = 0;
                break;
            }
        }
        pthread_mutex_unlock(&mutex); // Unlock critical section
        sem_post(&empty); // Signal that buffer is no longer full
    }
    pthread_exit(NULL);
}

int main() {
    pthread_t producer_thread, consumer_thread;
    int numbers[NUMBERS];
    printf("Enter %d numbers:\n", NUMBERS);
    for (int i = 0; i < NUMBERS; ++i) {
        scanf("%d", &numbers[i]);
    }
    // Initialize semaphores and mutex
    sem_init(&empty, 0, BUFFER_SIZE);
    sem_init(&full, 0, 0);
    pthread_mutex_init(&mutex, NULL);
    // Create threads
    pthread_create(&producer_thread, NULL, producer, (void *)numbers);
    pthread_create(&consumer_thread, NULL, consumer, NULL);
    // Join threads
    pthread_join(producer_thread, NULL);
    pthread_join(consumer_thread, NULL);
    // Print buffer contents
    printf("Buffer contents:\n");
    for (int i = 0; i < BUFFER_SIZE; ++i) {
        printf("%d ", buffer[i]);
    }
    printf("\n");
    // Destroy semaphores and mutex
    sem_destroy(&empty);
    sem_destroy(&full);
    pthread_mutex_destroy(&mutex);
    return 0;
}

