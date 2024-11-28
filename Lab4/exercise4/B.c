//B
//For this solution, the parent will alternate with the children’s writing. After each child process writes its sequence, the parent writes “00000” five times. This will create the interleaved pattern in output.txt.


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
    strcpy(buffer, "00000");

    // Create or truncate the file initially
    int fd = open("output.txt", O_CREAT | O_TRUNC | O_WRONLY, S_IRUSR | S_IWUSR);
    if (fd < 0) {
        perror("open");
        exit(EXIT_FAILURE);
    }
    close(fd);

    for (i = 1; i < 10; i++) {
        if (fork() == 0) {
            // In child process
            fd = open("output.txt", O_WRONLY);
            if (fd < 0) {
                perror("open");
                exit(EXIT_FAILURE);
            }

            // Prepare the child's unique string to write
            sprintf(buffer, "%d%d%d%d%d", i, i, i, i, i);

            // Write at the position (10 * (i - 1) + 5)
            lseek(fd, 10 * (i - 1) + 5, SEEK_SET);
            write(fd, buffer, 5);
            close(fd);
            exit(0);
        } else {
            // Parent process writes "00000" after each child
            fd = open("output.txt", O_WRONLY);
            if (fd < 0) {
                perror("open");
                exit(EXIT_FAILURE);
            }

            // Write parent string at position 10 * (i - 1)
            lseek(fd, 10 * (i - 1), SEEK_SET);
            write(fd, "00000", 5);
            close(fd);

            // Wait for the child to finish before starting next loop
            wait(NULL);
        }
    }

    // Print the file contents
    fd = open("output.txt", O_RDONLY);
    if (fd < 0) {
        perror("open");
        exit(EXIT_FAILURE);
    }

    char c;
    printf("File contents are:\n");
    while (read(fd, &c, 1) > 0) {
        printf("%c", c);
    }
    printf("\n");
    close(fd);

    return 0;
}