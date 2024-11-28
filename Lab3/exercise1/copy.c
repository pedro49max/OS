#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

#define BUFFER_SIZE 512

void copy(int fdo, int fdd){
	char buffer[BUFFER_SIZE];
    ssize_t bytes_read, bytes_written;

    // Read and write the file in chunks of 512 bytes
    while ((bytes_read = read(fdo, buffer, BUFFER_SIZE)) > 0) {
        bytes_written = write(fdd, buffer, bytes_read);
        if (bytes_written != bytes_read) {
            perror("Error writing to the destination file");
            exit(EXIT_FAILURE);
        }
    }

    if (bytes_read == -1) {
        perror("Error reading from the source file");
        exit(EXIT_FAILURE);
    }
}

int main(int argc, char *argv[]){
	if (argc != 3) {
        fprintf(stderr, "Usage: %s <source_file> <destination_file>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // Open the source file in read-only mode
    int fdo = open(argv[1], O_RDONLY);
    if (fdo == -1) {
        perror("Error opening the source file");
        exit(EXIT_FAILURE);
    }

    // Open the destination file in write-only mode, create if it doesn't exist, truncate if it does
    int fdd = open(argv[2], O_CREAT | O_WRONLY | O_TRUNC, 0644);
    if (fdd == -1) {
        perror("Error opening/creating the destination file");
        close(fdo);  // Clean up and close source file
        exit(EXIT_FAILURE);
    }

    // Perform the copy operation
    copy(fdo, fdd);

    // Close both files
    if (close(fdo) == -1) {
        perror("Error closing the source file");
        exit(EXIT_FAILURE);
    }

    if (close(fdd) == -1) {
        perror("Error closing the destination file");
        exit(EXIT_FAILURE);
    }
	return 0;
}
