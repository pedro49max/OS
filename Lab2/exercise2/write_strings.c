#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <err.h>

int main(int argc, char* argv[])
{
	if(argc < 3){
		fprintf(stderr, "Usage: %s <outputfile> <string1> <string2> ... \n", argv[0]);
		return 1;
	}
	
	//openeing file
	FILE *file = fopen(argv[1], "wb");
	if(!file){
		perror("Failed to open file");
		return 1;
	}

	//writing from argv[2]
	for(int i = 2; i < argc; i++){
		fwrite(argv[i], sizeof(char), strlen(argv[i]) + 1, file);
	}

	fclose(file);
	
	return 0;
}
