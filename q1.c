#define _XOPEN_SOURCE 700 // required for barriers to work
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(void) {
    pid_t child1, child2;
    FILE *file1, *file2;

    // Create child process
    child1 = fork();

    if (child1 < 0) {
        // Fork failed
        perror("Fork failed");
        exit(EXIT_FAILURE);
    } else if (child1 == 0) {
        // Child process 1
        sleep(1);
        file1 = fopen("child1.txt", "r");
        if (file1 == NULL) {
            perror("Error opening file child1.txt");
            exit(EXIT_FAILURE);
        }
        char line[100];
        fgets(line, sizeof(line), file1);
        printf("Child 1: %s", line);
        fclose(file1);
        exit(EXIT_SUCCESS);
    } else {
        // Parent process
        child2 = fork();

        if (child2 < 0) {
            // Fork failed
            perror("Fork failed");
            exit(EXIT_FAILURE);
        } else if (child2 == 0) {
            // Child process 2
            sleep(1);
            file2 = fopen("child2.txt", "r");
            if (file2 == NULL) {
                perror("Error opening file child2.txt");
                exit(EXIT_FAILURE);
            }
            char line[100];
            fgets(line, sizeof(line), file2);
            printf("Child 2: %s", line);
            fclose(file2);
            exit(EXIT_SUCCESS);
        } else {
            // Parent process
            file1 = fopen("child1.txt", "w");
            if (file1 == NULL) {
                perror("Error opening file child1.txt");
                exit(EXIT_FAILURE);
            }
            fprintf(file1, "child 1\n");
            fclose(file1);

            file2 = fopen("child2.txt", "w");
            if (file2 == NULL) {
                perror("Error opening file child2.txt");
                exit(EXIT_FAILURE);
            }
            fprintf(file2, "child 2\n");
            fclose(file2);

            // Wait for child processes to finish
            wait(NULL);
            wait(NULL);
        }
    }

    return 0;
}

