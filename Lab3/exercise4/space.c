#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <dirent.h>
#include <string.h>
#include <errno.h>

#define BLOCK_SIZE 512
#define KB_SIZE 1024

/* Forward declaration */
int get_size_dir(char *fname, size_t *blocks);

/* Gets in the blocks buffer the size of file fname using lstat. If fname is a
 * directory get_size_dir is called to add the size of its contents.
 */
int get_size(char *fname, size_t *blocks){
	struct stat statbuf;

    // Use lstat to get the file or directory's metadata
    if (lstat(fname, &statbuf) == -1) {
        perror("Error with lstat");
        return -1;
    }

    // If it's a regular file, add its size in blocks
    if (S_ISREG(statbuf.st_mode)) {
        *blocks += statbuf.st_blocks;  // st_blocks gives the number of 512B blocks
    }
    // If it's a directory, traverse it recursively
    else if (S_ISDIR(statbuf.st_mode)) {
        if (get_size_dir(fname, blocks) == -1) {
            return -1;
        }
    }
	return 0;
}


/* Gets the total number of blocks occupied by all the files in a directory. If
 * a contained file is a directory a recursive call to get_size_dir is
 * performed. Entries . and .. are conveniently ignored.
 */
int get_size_dir(char *dname, size_t *blocks){
	DIR *dir;
    struct dirent *entry;
    char path[PATH_MAX];

    // Open the directory
    if ((dir = opendir(dname)) == NULL) {
        perror("Error opening directory");
        return -1;
    }

    // Read the directory entries
    while ((entry = readdir(dir)) != NULL) {
        // Ignore the special entries "." and ".."
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        // Build the full path to the file/directory
        snprintf(path, sizeof(path), "%s/%s", dname, entry->d_name);

        // Get the size of the entry (recursively if it's a directory)
        if (get_size(path, blocks) == -1) {
            closedir(dir);
			return -1;
        }
    }

    closedir(dir);
    return 0;
}

/* Processes all the files in the command line calling get_size on them to
 * obtain the number of 512 B blocks they occupy and prints the total size in
 * kilobytes on the standard output
 */
int main(int argc, char *argv[]){
	if (argc < 2) {
        fprintf(stderr, "Usage: %s <files or directories>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // Process each file or directory passed as argument
    for (int i = 1; i < argc; i++) {
        size_t blocks = 0;

        // Get the size of the current file or directory
        if (get_size(argv[i], &blocks) == -1) {
            fprintf(stderr, "Error processing %s\n", argv[i]);
            continue;
        }

        // Convert blocks to kilobytes and print
        size_t kilobytes = (blocks * BLOCK_SIZE) / KB_SIZE;
        printf("%zuK %s\n", kilobytes, argv[i]);
    }
	return 0;
}
