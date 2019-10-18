/*
* Description: Executing System Calls in C: lfcat()
*
* Author: Samuel Lundquist
*
* Date: 10-17-2019
*
* Notes:
* Using a.out as the "lab3.exe" on line 54.
* If you compile as something else, it would need to be specified there.
*/

/*-------------------------Preprocessor Directives---------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <unistd.h>
/*---------------------------------------------------------------------------*/

/*----------------------------Function Definitions---------------------------*/
void lfcat()
{
	/* Define your variables here */
	char* buf;
	size_t bsize = 0;
	char cdir[256];
	DIR* dir;
	struct dirent* dp;
	FILE* read_file;
	FILE* write_file;

	/* Open the file output.txt for writing */
	write_file = fopen("./output.txt", "w");

	/* Get the current directory*/
	if(getcwd(cdir, sizeof(cdir)) == NULL)
	{
		printf("Error with getcwd()\n");
		exit(1);
	}

	/* Open the dir using opendir() */
	if((dir = opendir(cdir)) == NULL)
	{
		printf("Cannot open directory %s\n", cdir);
		exit(1);
	}

	/* use a while loop to read the dir */
	while((dp = readdir(dir)) != NULL){

		/* Handle cases for files that don't need to be read */
		if(!strncmp(dp->d_name, ".", 1) || !strcmp(dp->d_name, "951539283-lab3.c")
		|| !strcmp(dp->d_name, "a.out") || !strcmp(dp->d_name, "output.txt"))
		{
			//Do nothing
		}

		/* Write file to output.txt */
		else
		{
			char* file_name = dp->d_name; // use it
            char str[80];
			sprintf(str, "File: %s\n", file_name);
			fputs(str, write_file);
			FILE* read_file = fopen(file_name, "r");
			while(getline(&buf, &bsize, read_file) >= 0)
			{
				fputs(buf, write_file);
			}
			fputs("\n--------------------------------------------------------------------------------\n", write_file);
			fclose(read_file);
			file_name = NULL;
			buf = NULL;
		}
	}
	fclose(write_file);
	closedir(dir);
}

/*---------------------------------------------------------------------------*/
/*-----------------------------Program Main----------------------------------*/
int main() {
	/*function vars */
	char *cBuffer;
	size_t bufferSize;
	size_t inputSize;
	char *token;

	/* Allocate memory to the input buffer. */
	cBuffer = (char *)malloc(bufferSize * sizeof(char));
	if( cBuffer == NULL)
	{
		printf("Error! Unable to allocate input buffer. \n");
		exit(1);
	}

	/*main run cycle*/
	do {

	printf( ">>> ");
	inputSize = getline(&cBuffer, &bufferSize, stdin);

	/* tokenize the input string */
	token = strtok(cBuffer, " ");
	while(token != NULL && strcmp(token, "\n")) {

		/*if the command is 'exit then close the program*/
		if(strcmp(token, "exit\n") == 0 || strcmp(token, "exit") == 0) { break; }

		/*Display any commands */
		if(strcmp(token, "lfcat\n") == 0) {lfcat();}
		else {printf("Error: Unrecognized command! \n"); break;}
		token = strtok(NULL, " ");
	}
	if(token != NULL) {if(strcmp(token, "exit\n") == 0 || strcmp(token, "exit") == 0) { break; }}
	} while (1);
	/*Free the allocated memory*/
	free(cBuffer);
	return 0;
}
/*-----------------------------Program End-----------------------------------*/
