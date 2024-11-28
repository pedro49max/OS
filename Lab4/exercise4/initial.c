#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>

int main(void)
{
    int fd1,fd2,i,pos;
    char c;
    char buffer[6];

    fd1 = open("output.txt", O_CREAT | O_TRUNC | O_RDWR, S_IRUSR | S_IWUSR);
    write(fd1, "00000", 5);
    for (i=1; i < 10; i++) {
        pos = lseek(fd1, 0, SEEK_CUR);
        if (fork() == 0) {
            /* Child */
            sprintf(buffer, "%d", i*11111);
            lseek(fd1, pos, SEEK_SET);
            write(fd1, buffer, 5);
            close(fd1);
            exit(0);
        } else {
            /* Parent */
            lseek(fd1, 5, SEEK_CUR);
        }
    }

	//wait for all childs to finish
    while (wait(NULL) != -1);

    lseek(fd1, 0, SEEK_SET);
    printf("File contents are:\n");
    while (read(fd1, &c, 1) > 0)
        printf("%c", (char) c);
    printf("\n");
    close(fd1);
    exit(0);
}
/*
A
To fix the race condition without synchronizing processes or imposing an execution order, we need each child process to open its own file descriptor for output.txt. This prevents processes from competing for a single file descriptor’s position.


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
*/

/*
B
For this solution, the parent will alternate with the children’s writing. After each child process writes its sequence, the parent writes “00000” five times. This will create the interleaved pattern in output.txt.


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
*/