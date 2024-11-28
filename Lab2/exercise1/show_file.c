#include <stdio.h>
#include <stdlib.h>
#include <err.h>

int main(int argc, char* argv[]) {
	FILE* file=NULL;
	int c,ret;
	unsigned char buffer[1]; //to holde 1 char
	size_t bytes_read, bytes_written;

	if (argc!=2) {
		fprintf(stderr,"Usage: %s <file_name>\n",argv[0]);
		exit(1);
	}

	/* Open file */
	if ((file = fopen(argv[1], "r")) == NULL)
		err(2,"The input file %s could not be opened",argv[1]);

	/* Read file byte by byte */
	while ((bytes_read = fread(buffer, 1, 1,file)) != EOF) {
		/* Print byte to stdout */
		bytes_written=fwrite(buffer,1, 1,  stdout);

		if (bytes_written != 1){
			fclose(file);
			err(3,"putc() failed!!");
		}
	}

	fclose(file);
	return 0;
}
//Question 1: gcc -o show_file show_file.c

//Question2: for copilation gcc -c show_file.c
// for linking the object file show_file.o and geting the executable: 
//          gcc -o show_file show_file.o