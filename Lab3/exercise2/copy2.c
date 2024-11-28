#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <errno.h>
#include <string.h>

#define BUFFER_SIZE 512

void copy(int fdo, int fdd){
	char buffer[BUFFER_SIZE];
    ssize_t bytes_read, bytes_written;

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

void copy_regular(char *orig, char *dest){
	int fdo = open(orig, O_RDONLY);
    if (fdo == -1) {
        perror("Error opening the source file");
        exit(EXIT_FAILURE);
    }

    int fdd = open(dest, O_CREAT | O_WRONLY | O_TRUNC, 0644);
    if (fdd == -1) {
        perror("Error opening/creating the destination file");
        close(fdo);
        exit(EXIT_FAILURE);
    }

    // Perform the copy operation
    copy(fdo, fdd);

    close(fdo);
    close(fdd);
}

void copy_link(char *orig, char *dest){
	struct stat statbuf;
    
    // Get the size of the symbolic link's path (without null character)
    if (lstat(orig, &statbuf) == -1) {
        perror("Error with lstat");
        exit(EXIT_FAILURE);
    }

    // Allocate memory to store the path + null terminator
    char *link_target = malloc(statbuf.st_size + 1);
    if (link_target == NULL) {
        perror("Memory allocation failed");
        exit(EXIT_FAILURE);
    }

    // Read the path the symbolic link points to
    ssize_t len = readlink(orig, link_target, statbuf.st_size);
    if (len == -1) {
        perror("Error reading the symbolic link");
        free(link_target);
        exit(EXIT_FAILURE);
    }
    link_target[len] = '\0';  // Manually add null character

    // Create a symbolic link at the destination pointing to the same target
    if (symlink(link_target, dest) == -1) {
        perror("Error creating symbolic link");
        free(link_target);
        exit(EXIT_FAILURE);
    }

    free(link_target);  // Free the allocated memory
}

int main(int argc, char *argv[]){
	if (argc != 3) {
        fprintf(stderr, "Usage: %s <source_file> <destination_file>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    struct stat statbuf;

    // Get file status information
    if (lstat(argv[1], &statbuf) == -1) {
        perror("Error with lstat");
        exit(EXIT_FAILURE);
    }

    // Check the file type
    if (S_ISREG(statbuf.st_mode)) {
        // Source is a regular file
        copy_regular(argv[1], argv[2]);
    }
    else if (S_ISLNK(statbuf.st_mode)) {
        // Source is a symbolic link
        copy_link(argv[1], argv[2]);
    }
    else {
        // Any other type of file (e.g. directory), exit with error
        fprintf(stderr, "Error: '%s' is not a regular file or symbolic link.\n", argv[1]);
        exit(EXIT_FAILURE);
    }
	return 0;
}
