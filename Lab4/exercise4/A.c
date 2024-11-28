//A
//To fix the race condition without synchronizing processes or imposing an execution order, we need each child process to open its own file descriptor for output.txt. This prevents processes from competing for a single file descriptor’s position.


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>

int main(void) {
    int i;
    char buffer[6];

    // Create or truncate the file and write the initial zeros
    int fd1 = open("output.txt", O_CREAT | O_TRUNC | O_WRONLY, S_IRUSR | S_IWUSR);
    if (fd1 < 0) {
        perror("open");
        exit(EXIT_FAILURE);
    }
    write(fd1, "00000", 5);
    close(fd1);

    // Fork 9 child processes
    for (i = 1; i < 10; i++) {
        if (fork() == 0) {
            // In child process
            int fd = open("output.txt", O_WRONLY);
            if (fd < 0) {
                perror("open");
                exit(EXIT_FAILURE);
            }

            sprintf(buffer, "%d%d%d%d%d", i, i, i, i, i); // Format 5 repetitions of the number

            // Move the file pointer to the appropriate position (5 * i)
            lseek(fd, i * 5, SEEK_SET);
            write(fd, buffer, 5); // Write the buffer to the file
            close(fd);
            exit(0);
        }
    }

    // Wait for all child processes to complete
    while (wait(NULL) > 0);

    // Print the file contents
    fd1 = open("output.txt", O_RDONLY);
    if (fd1 < 0) {
        perror("open");
        exit(EXIT_FAILURE);
    }

    char c;
    printf("File contents are:\n");
    while (read(fd1, &c, 1) > 0) {
        printf("%c", c);
    }
    printf("\n");
    close(fd1);

    return 0;
}