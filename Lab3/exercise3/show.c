#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>

#define BUFFER_SIZE 1  // Read one byte at a time

int main(int argc, char *argv[])
{
    int opt;
    int N = 0;         // Default N value (skip 0 bytes)
    int e_flag = 0;    // Flag to check if -e is present

    // Parse command-line arguments using getopt
    while ((opt = getopt(argc, argv, "n:e")) != -1) {
        switch (opt) {
            case 'n':
                N = atoi(optarg);  // Parse the value for -n
                break;
            case 'e':
                e_flag = 1;  // Set the -e flag
                break;
            default:
                fprintf(stderr, "Usage: %s [-n N] [-e] <file>\n", argv[0]);
                exit(EXIT_FAILURE);
        }
    }

    // Ensure a file name is provided after the options
    if (optind >= argc) {
        fprintf(stderr, "Expected file name after options\n");
        exit(EXIT_FAILURE);
    }

    char *filename = argv[optind];

    // Open the file
    int fd = open(filename, O_RDONLY);
    if (fd == -1) {
        perror("Error opening the file");
        exit(EXIT_FAILURE);
    }

    // Get the file size using lseek
    off_t file_size = lseek(fd, 0, SEEK_END);
    if (file_size == -1) {
        perror("Error getting file size");
        close(fd);
        exit(EXIT_FAILURE);
    }

    // Adjust the file pointer based on the presence of the -e flag
    if (e_flag) {
        // Show last N bytes (move file pointer N bytes before the end)
        if (N > file_size) {
            N = file_size;  // If N is greater than the file size, show the entire file
        }
        if (lseek(fd, -N, SEEK_END) == -1) {
            perror("Error seeking in file");
            close(fd);
            exit(EXIT_FAILURE);
        }
    } else {
        // Skip first N bytes (move file pointer N bytes from the beginning)
        if (N > file_size) {
            N = file_size;  // If N is greater than the file size, skip the entire file
        }
        if (lseek(fd, N, SEEK_SET) == -1) {
            perror("Error seeking in file");
            close(fd);
            exit(EXIT_FAILURE);
        }
    }

    // Read and write the file byte by byte until the end of file
    char buffer[BUFFER_SIZE];
    ssize_t bytes_read;

    while ((bytes_read = read(fd, buffer, BUFFER_SIZE)) > 0) {
        if (write(STDOUT_FILENO, buffer, bytes_read) == -1) {
            perror("Error writing to standard output");
            close(fd);
            exit(EXIT_FAILURE);
        }
    }

    if (bytes_read == -1) {
        perror("Error reading from the file");
    }

    // Close the file
    close(fd);

    return 0;
}

