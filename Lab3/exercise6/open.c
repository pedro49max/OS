#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>  // For open()
#include <unistd.h> // For close(), read(), write()
#include <string.h> // For strerror()
#include <errno.h>  // For errno

void usage(const char *progname) {
    printf("Usage: %s -f <filename> [-r] [-w]\n", progname);
}

int main(int argc, char *argv[]) {
    int opt;
    char *filename = NULL;
    int flags = 0;
    int fd;
   
    if (argc < 3) {
        usage(argv[0]);
        return 1;
    }

    while ((opt = getopt(argc, argv, "f:rw")) != -1) {
        switch (opt) {
            case 'f':
                filename = optarg;
                break;
            case 'r':
                flags |= O_RDONLY;
                break;
            case 'w':
                flags |= O_WRONLY | O_CREAT | O_TRUNC;
                break;
            default:
                usage(argv[0]);
                return 1;
        }
    }

    if (!filename || !(flags & (O_RDONLY | O_WRONLY))) {
        usage(argv[0]);
        return 1;
    }

    // Try to open the file
    fd = open(filename, flags, 0644);
    if (fd == -1) {
        perror("Error opening file");
        return 1;
    }

    // Try to write if the file was opened with write permissions
    if (flags & O_WRONLY) {
        char *data = "Test write\n";
        if (write(fd, data, strlen(data)) == -1) {
            perror("Error writing to file");
        } else {
            printf("Successfully wrote to file.\n");
        }
    }

    // Try to read from the file if opened with read permissions
    if (flags & O_RDONLY) {
        char buffer[128];
        ssize_t bytes_read = read(fd, buffer, sizeof(buffer) - 1);
        if (bytes_read == -1) {
            perror("Error reading from file");
        } else {
            buffer[bytes_read] = '\0';
            printf("Read from file: %s\n", buffer);
        }
    }

    close(fd);
    return 0;
}

//situations1
// chmod 644 sample.txt    read and write permisions


//situation 2
//chmod 200 sample.txt      emove read permision

//situation3
//chmod -x ./open.x         remove executable permision