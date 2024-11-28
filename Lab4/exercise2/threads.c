#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

// Structure for thread arguments
typedef struct {
    int user_number;
    char priority;
} thread_arg_t;

// Thread function
void *thread_usuario(void *arg) {
    thread_arg_t *thread_arg = (thread_arg_t *)arg;

    // Copy arguments to local variables
    int user_number = thread_arg->user_number;
    char priority = thread_arg->priority;

    // Free the allocated memory for arguments
    free(thread_arg);

    // Print thread details
    printf("Thread ID: %lu, User Number: %d, Priority: %c\n",
           pthread_self(), user_number, priority);

    return NULL;
}

int main(int argc, char *argv[]) {
    int num_users = 10; // You can modify this to set the number of threads to create
    pthread_t threads[num_users];

    // Create threads with dynamic memory for arguments
    for (int i = 0; i < num_users; i++) {
        // Allocate memory for the thread argument
        thread_arg_t *arg = (thread_arg_t *)malloc(sizeof(thread_arg_t));
        if (arg == NULL) {
            perror("malloc");
            exit(EXIT_FAILURE);
        }

        // Initialize the argument with user number and priority
        arg->user_number = i;
        arg->priority = (i % 2 == 0) ? 'P' : 'N'; // Even = Priority 'P', Odd = Non-Priority 'N'

        // Create the thread and pass the argument
        if (pthread_create(&threads[i], NULL, thread_usuario, arg) != 0) {
            perror("pthread_create");
            free(arg); // Free allocated memory in case of error
            exit(EXIT_FAILURE);
        }
    }

    // Wait for all threads to finish
    for (int i = 0; i < num_users; i++) {
        pthread_join(threads[i], NULL);
    }

    return 0;
}
//If we give the same value to all threads, they will all change the value to each other with no order.
