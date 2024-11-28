#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>

pid_t child_pid; // Global variable to store the child process ID

// Signal handler for SIGALRM
void handle_sigalrm(int sig) {
    printf("SIGALRM received: Sending SIGKILL to child (PID %d)\n", child_pid);
    kill(child_pid, SIGKILL); // Send SIGKILL to the child process
}

// Function to set up signal handlers
void setup_signal_handlers() {
    // Set up SIGALRM handler to call handle_sigalrm
    struct sigaction sa;
    sa.sa_handler = handle_sigalrm;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    if (sigaction(SIGALRM, &sa, NULL) == -1) {
        perror("sigaction");
        exit(EXIT_FAILURE);
    }

    // Ignore SIGINT
    signal(SIGINT, SIG_IGN);
}

int main(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <executable>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // Fork to create child process
    child_pid = fork();
    if (child_pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (child_pid == 0) {
        // In child process: replace with specified executable
        if (execvp(argv[1], &argv[1]) == -1) {
            perror("execvp");
            exit(EXIT_FAILURE);
        }
    } else {
        // In parent process: set up signal handlers and timer
        setup_signal_handlers();
        alarm(5); // Set an alarm to send SIGALRM after 5 seconds

        // Wait for the child to terminate
        int status;
        if (waitpid(child_pid, &status, 0) == -1) {
            perror("waitpid");
            exit(EXIT_FAILURE);
        }

        // Check the termination cause
        if (WIFEXITED(status)) {
            printf("Child terminated normally with exit status %d\n", WEXITSTATUS(status));
        } else if (WIFSIGNALED(status)) {
            printf("Child terminated by signal %d\n", WTERMSIG(status));
        }
    }

    return 0;
}

