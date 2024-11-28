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

int print_binary_file(char *path)
{
	/* To be completed  (part B) */
	return 0;
}


int write_binary_file(char *input_file, char *output_file)
{
	/* To be completed  (part C) */
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
	while ((opt = getopt(argc, argv, "hi:p")) != -1)
	{
		switch (opt)
		{
		case 'h':
			fprintf(stderr, "Usage: %s [ -h | -p | -i file ]\n", argv[0]);
			exit(EXIT_SUCCESS);
		case 'i':
			
			options.input_file = argv[2];//optarg
			break;
		case 'p':
			options.action = PRINT_TEXT_ACT;
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

	printf("Input file: %s\n" , options.input_file);
	printf("Action: %d\n", options.action);
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
