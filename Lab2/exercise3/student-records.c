#include <stdio.h>
#include <unistd.h> /* for getopt() */
#include <stdlib.h> /* for EXIT_SUCCESS, EXIT_FAILURE */
#include <string.h>
#include "defs.h"


/* Assume lines in the text file are no larger than 100 chars */
#define MAXLEN_LINE_FILE 100


int print_text_file(char *path){
	FILE *file = fopen(path, "r");//to read

	if(file == NULL){
		fprintf(stderr, "Cannot open the file: %s\n", path);
		return 1;
	}
	int n;//number of students
	char line[MAXLEN_LINE_FILE];

	if(fgets(line, sizeof(line), file )== NULL){
		fprintf(stderr, "Cannot read the file: %s\n", path);
		fclose(file);
		return 1;
	}
	n = atoi(line);//gets the n from the readed line

	for(int i = 0; i < n; i++){
		if(fgets(line, sizeof(line), file )== NULL){
		fprintf(stderr, "Cannot read the record from the file: %s\n", path);
		fclose(file);
		return 1;
		}
		student_t student;
		char *aux;
		char *lineptr = line; //to avoid warnings in strsep();

		aux = strsep(&lineptr, ":");
		student.student_id = atoi(aux);

		aux = strsep(&lineptr, ":");
		strncpy(student.NIF, aux, MAX_CHARS_NIF);

		student.first_name = strsep(&lineptr, ":");
		student.last_name = strsep(&lineptr, ":");

		printf("[Entry #%d]\n", i);//prints [Entry #0]
		printf("\tstudent_id=%d\n", student.student_id);//printing student_id=#
		printf("\tNIF=%s\n",student.NIF);//printing NIF=#
		printf("\tfirst_name=%s\n",student.first_name);//printing first_name=#
		printf("\tlast_name=%s\n" ,student.last_name);//printing last_name=#
	}
	fclose(file);
	
	return 0;
}

char *loadstr(FILE *file){
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
		fprintf(stderr, "cannot allocate the memory");
	}

	fread(str, sizeof(char), lenght + 1, file);//reading the string including \0
	return str;	
}

int print_binary_file(char *path)
{	
	FILE *file = fopen(path, "rb");//open binary file
	if(!file){//coudn't open the file
		fprintf(stderr,"Failed to open the file %s\n", path);
		return 1;
	}
	
	int n;
	fread(&n, sizeof(int), 1, file);

	for (int i  =0; i < n; i++){
		student_t student;
		fread(&student.student_id, sizeof(int), 1, file);
		char *tmp;
		temp = loadstr(file);
        strcpy(student.NIF,temp);
        free(temp);

		student.first_name = loadstr(file);
		student.last_name = loadstr(file);

		if((student.first_name = loadstr(file)) == NULL || (student.last_name = loadstr(file)) == NULL){
			fprintf(stderr, "Error while reading the names");
			fclose(file);
			return -1;
		}

		printf("[Entry #%d]\n", i);//prints [Entry #0]
		printf("\tstudent_id=%d\n", student.student_id);//printing student_id=#
		printf("\tNIF=%s\n",student.NIF);//printing NIF=#
		printf("\tfirst_name=%s\n",student.first_name);//printing first_name=#
		printf("\tlast_name=%s\n" ,student.last_name);//printing last_name=#

		free(student.first_name);
		free(student.last_name);
	}

	fclose(file);
	return 0;
}



int write_binary_file(char *input_file, char *output_file)
{
	FILE *infile = fopen(input_file, "r");//to read
	if(infile == NULL){
		fprintf(stderr, "Cannot open the input file: %s\n", input_file);
		return 1;
	}
	FILE *outfile = fopen(output_file, "wb");//to write binary
	if(outfile == NULL){
		fprintf(stderr, "Cannot open the output file: %s\n", output_file);
		return 1;
	}

	int n;//number of students
	char line[MAXLEN_LINE_FILE];

	if(fgets(line, sizeof(line), infile )== NULL){
		fprintf(stderr, "Cannot read number of students from the file: %s\n", input_file);
		fclose(infile);
		fclose(outfile);
		return 1;
	}
	n = atoi(line);//gets the n from the readed line
	fwrite(&n, sizeof(int), 1, outfile); //writes n in the output file

	for(int i = 0; i < n; i++){
		if(fgets(line, sizeof(line), infile )== NULL){
			fprintf(stderr, "Cannot read the record from the inputfile: %s\n", input_file);
			fclose(infile);
			fclose(outfile);
			return 1;
		}
		student_t student;
		char *aux;
		char *lineptr = line; //to avoid warnings in strsep();

		aux = strsep(&lineptr, ":");
		student.student_id = atoi(aux);

		aux = strsep(&lineptr, ":");
		strncpy(student.NIF, aux, MAX_CHARS_NIF);
		student.NIF[MAX_CHARS_NIF] = '\0';//to make it end in null

		student.first_name = strsep(&lineptr, ":");
		student.first_name[strlen(student.first_name)] = '\0';
		student.last_name = strsep(&lineptr, ":");
		student.last_name[strlen(student.last_name)] = '\0';

		fwrite(&student.student_id, sizeof(int), 1, outfile);//writes studentId
		fwrite(student.NIF, sizeof(char), MAX_CHARS_NIF + 1, outfile);//writes NIF //Problemas con penelope cruz
		fwrite(student.first_name, sizeof(char), strlen(student.first_name) + 1, outfile);//writes first_name
		fwrite(student.last_name, sizeof(char), strlen(student.last_name), outfile);//writes last_name
	}
	fclose(infile);
	fclose(outfile);

	printf("%d student records written succesfully to binary file %s\n", n, output_file);
	return 0;
}

int main(int argc, char *argv[])
{
	int ret_code, opt;
	struct options options;

	/* Initialize default values for options */
	options.input_file = NULL;
	options.output_file = NULL;
	options.action = NONE_ACT;
	ret_code = 0;

	/* Parse command-line options (incomplete code!) */
	while ((opt = getopt(argc, argv, "hi:p:o:b")) != -1)
	{
		switch (opt)
		{
		case 'h':
			fprintf(stderr, "Usage: %s [ -h | -p | -i file | -o <output_file> | -b ]\n", argv[0]);
			exit(EXIT_SUCCESS);
		case 'i':
			options.input_file = argv[2];//optarg
			break;
		case 'p':
			options.action = PRINT_TEXT_ACT;
			break;
		case 'o':
			options.output_file = argv[4];//optarg
			options.action= WRITE_BINARY_ACT;
			break;
		case 'b':
			options.action = PRINT_BINARY_ACT;
			break;
		/**
		 **  To be completed ...
		 **/

		default:
			exit(EXIT_FAILURE);
		}
	}

	if (options.input_file == NULL)
	{
		fprintf(stderr, "Must specify one record file as an argument of -i\n");
		exit(EXIT_FAILURE);
	}

	
	switch (options.action)
	{
	case NONE_ACT:
		fprintf(stderr, "Must indicate one of the following options: -p, -o, -b \n");
		ret_code = EXIT_FAILURE;
		break;
	case PRINT_TEXT_ACT:
		/* Part A */
		ret_code = print_text_file(options.input_file);
		break;
	case WRITE_BINARY_ACT:
		/* Part B */
		ret_code = write_binary_file(options.input_file, options.output_file);
		break;
	case PRINT_BINARY_ACT:
		/* Part C */
		ret_code = print_binary_file(options.input_file);
		break;
	default:
		break;
	}
	exit(ret_code);
}
