#define _XOPEN_SOURCE 700 // required for barriers to work
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(void) {
    pid_t child_pid;
    int status;

    // Create child process
    child_pid = fork();

    if (child_pid < 0) {
        // Fork failed
        perror("Fork failed");
        exit(EXIT_FAILURE);
    } else if (child_pid == 0) {
        // Child process
        sleep(1);
        printf("Child process\n");
        exit(EXIT_SUCCESS);
    } else {
        // Parent process
        printf("Parent process\n");
        // Wait for the child process to terminate
        wait(&status);

        if (WIFEXITED(status)) {
            printf("Child process exited with status: %d\n", WEXITSTATUS(status));
        } else {
            printf("Child process exited abnormally\n");
        }
    }

    return 0;
}

