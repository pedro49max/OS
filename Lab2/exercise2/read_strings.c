#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <err.h>

/** Loads a string from a file.
 *
 * file: pointer to the FILE descriptor
 *
 * The loadstr() function must allocate memory from the heap to store
 * the contents of the string read from the FILE.
 * Once the string has been properly built in memory, the function returns
 * the starting address of the string (pointer returned by malloc())
 *
 * Returns: !=NULL if success, NULL if error
 */
char *loadstr(FILE *file)
{
	int lenght  =0;
	long start = ftell(file);
	char c;

	while(fread(&c, sizeof(char), 1, file) == 1 && c!= '\0'){// get the leght  of the string to read
		lenght++;
	}

	if(feof(file) || ferror(file)){//chaecking for the end of file
		return NULL;
	}

	fseek(file, start, SEEK_SET);//Now that we have the leght, we go back to read it

	char *str = malloc((lenght + 1) * sizeof(char));//allocating the memory
	if(!str){
		err(1, "cannot allocate the memory");
	}

	fread(str, sizeof(char), lenght + 1, file);//reading the string including \0
	return str;	
}


int main(int argc, char *argv[])
{
	if(argc != 2){//failed with the number of arguments
		fprintf(stderr, "Usage: %s <inputfile> \n", argv[0]);
		return 1;
	}

	FILE *file = fopen(argv[1], "rb");//open binary file
	if(!file){//coudn't open the file
		perror("Failed to open the file");
		return 1;
	}
	
	char *str;
	while((str = loadstr(file) )!= NULL){
		printf("%s\n", str);//printing each string in a new line
		free(str);//freeing the memory allocated in the function loadstr();
	}

	fclose(file);

	return 0;
}
