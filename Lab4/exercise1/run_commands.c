#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <ctype.h>

#define MAX_COMMANDS 100


pid_t launch_command(char** argv){
    pid_t pid = fork();
    if (pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }
    if (pid == 0) {
        // In child process, execute the command
        execvp(argv[0], argv);
        perror("execvp"); // Exec failed
        exit(EXIT_FAILURE);
    }
    return pid; // Return PID of the child process
}



char **parse_command(const char *cmd, int* argc) {
    // Allocate space for the argv array (initially with space for 10 args)
    size_t argv_size = 10;
    const char *end;
    size_t arg_len; 
    int arg_count = 0;
    const char *start = cmd;
    char **argv = malloc(argv_size * sizeof(char *));

    if (argv == NULL) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    while (*start && isspace(*start)) start++; // Skip leading spaces

    while (*start) {
        // Reallocate more space if needed
        if (arg_count >= argv_size - 1) {  // Reserve space for the NULL at the end
            argv_size *= 2;
            argv = realloc(argv, argv_size * sizeof(char *));
            if (argv == NULL) {
                perror("realloc");
                exit(EXIT_FAILURE);
            }
        }

        // Find the start of the next argument
        end = start;
        while (*end && !isspace(*end)) end++;

        // Allocate space and copy the argument
        arg_len = end - start;
        argv[arg_count] = malloc(arg_len + 1);

        if (argv[arg_count] == NULL) {
            perror("malloc");
            exit(EXIT_FAILURE);
        }
        strncpy(argv[arg_count], start, arg_len);
        argv[arg_count][arg_len] = '\0';  // Null-terminate the argument
        arg_count++;

        // Move to the next argument, skipping spaces
        start = end;
        while (*start && isspace(*start)) start++;
    }

    argv[arg_count] = NULL; // Null-terminate the array

    (*argc)=arg_count; // Return argc

    return argv;
}


int main(int argc, char *argv[]) {
   int opt;
    int background = 0;
    char *command = NULL;
    char *filename = NULL;

    // Parse options
    while ((opt = getopt(argc, argv, "x:s:b")) != -1) {
        switch (opt) {
            case 'x':
                command = optarg;
                break;
            case 's':
                filename = optarg;
                break;
            case 'b':
                background = 1;
                break;
            default:
                fprintf(stderr, "Usage: %s [-x command] [-s file] [-b]\n", argv[0]);
                exit(EXIT_FAILURE);
        }
    }

    // Handle -x option (single command execution)
    if (command) {
        int cmd_argc;
        char **cmd_argv = parse_command(command, &cmd_argc);
        pid_t pid = launch_command(cmd_argv);
        int status;
        waitpid(pid, &status, 0);
        printf("@@ Command terminated (pid: %d, status: %d)\n", pid, WEXITSTATUS(status));
        for (int i = 0; i < cmd_argc; i++) free(cmd_argv[i]);
        free(cmd_argv);
    }

    // Handle -s and -b options (file of commands)
    if (filename) {
        FILE *file = fopen(filename, "r");
        if (!file) {
            perror("fopen");
            exit(EXIT_FAILURE);
        }
        char line[256];
        pid_t pids[MAX_COMMANDS];
        int cmd_num = 0;
        while (fgets(line, sizeof(line), file) && cmd_num < MAX_COMMANDS) {
            line[strcspn(line, "\n")] = 0; // Remove newline
            int cmd_argc;
            char **cmd_argv = parse_command(line, &cmd_argc);
            printf("@@ Running command #%d: %s\n", cmd_num, line);
            pid_t pid = launch_command(cmd_argv);
            if (background) {
                pids[cmd_num] = pid;
            } else {
                int status;
                waitpid(pid, &status, 0);
                printf("@@ Command #%d terminated (pid: %d, status: %d)\n", cmd_num, pid, WEXITSTATUS(status));
            }
            for (int i = 0; i < cmd_argc; i++) free(cmd_argv[i]);
            free(cmd_argv);
            cmd_num++;
        }
        fclose(file);

        // Handle background waiting for all commands
        if (background) {
            int status;
            for (int i = 0; i < cmd_num; i++) {
                waitpid(pids[i], &status, 0);
                printf("@@ Command #%d terminated (pid: %d, status: %d)\n", i, pids[i], WEXITSTATUS(status));
            }
        }
    }
    return 0;
}
//Question 1
//it takes the -l as an invalid command

//Question2:
//execlp() requires inividual arguments ,so execvp is better for parsing commands

//Question3:
//it literally prints hello > a.txt
//Nothing hapens on the "/bin/bash ...." because execvp() canot do that