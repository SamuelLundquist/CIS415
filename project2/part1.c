/*
* Description: Project 2 Part 1.
*
* Author: Samuel Lundquist
*
* Date: 11/1/2019
*/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

char **malloc2DArr(int num, int char_size)
{
	char **new_arr = malloc(num * sizeof(char*));
	for (int i = 0; i < num; i++)
	{
		new_arr[i] = malloc(char_size * sizeof(char));
	}
	return new_arr;
}

char **arrayFromString(char* s, char* delim, int index, int char_size)
{
	char* word;
	if(index == 0)
	{
		word = strtok(s, delim);
	}
	else
	{
		word = strtok(NULL, delim);
	}

	if(word == NULL)
	{

		char** arr = malloc2DArr(index + 1, char_size);
		free(arr[index]);
		arr[index] = NULL;
		return arr;
	}
	else
	{
		int len = strlen(word);
		if(len > char_size)
		{
			char_size = len;
		}
		char** arr = arrayFromString(s, delim, index+1, char_size);
		strcpy(arr[index], word);
		return arr;
	}
}

char ***getCommands(FILE* fp, int index)
{
	char delim[6] = " \n\r";
	char* buf;
	size_t bsize = 255;
	int line;

	buf = malloc(bsize * sizeof(char));
	if(buf == NULL)
	{
		perror("Unable to allocate buffer.\n");
		exit(EXIT_FAILURE);
	}

	line = getline(&buf, &bsize, fp);
	if(line == -1)
	{
		free(buf);
		char ***arr = malloc((index + 1) * sizeof(char**));
		free(arr[index]);
		arr[index] = NULL;
		return arr;
	}
	else
	{
		char **command = arrayFromString(buf, delim, 0, 0);
		char ***arr = getCommands(fp, index+1);
		arr[index] = command;
		free(buf);
		return arr;
	}
}

void freeCommands(char ***arr)
{
	int i = 0;
	int j = 0;

	while(arr[i])
	{
		while(arr[i][j])
		{
			free(arr[i][j]);
			j++;
		}
		j=0;
		free(arr[i]);
		i++;
	}
	free(arr);
	return;
}

int main(int argc, char **argv)
{

	/* Check to make sure correct parameters are passed to main */
	if (argc != 2)
	{
		printf("\n[ERROR] Too many parameters were passed to main.\n"
			"This program takes a single filename as an input.\n\n");
			exit(EXIT_FAILURE);
	}

	if (argv[1] == NULL)
	{
		printf("\n[ERROR] A filename was not given to main.\n"
			"This program takes a single filename as an input.\n\n");
			exit(EXIT_FAILURE);
	}
	/*************************************************************/

	/* Open the file, handle error case */
	FILE* read_file = fopen(argv[1], "r");
	if(read_file == NULL)
	{
		perror("[ERROR] fopen()");
		exit(EXIT_FAILURE);
	}
	/*************************************************************/

	/* Get commands recursively from file and then print them to console */
	char ***commands = getCommands(read_file, 0);
	fclose(read_file);
	int num_commands = 0;
	int j = 0;
	char *p;
	char **q;
	while((q = commands[num_commands]) != NULL)
	{
		while((p = q[j]) != NULL)
		{
			printf("%s ", p);
			j++;
		}
		j = 0;
		printf("\n");
		num_commands++;
	}
	/*************************************************************/

	/* Fork processes and run execvp() for each command in commands */
	for(int i = 0 ; i < num_commands ; i++)
	{
		if(fork() == 0)
		{
			printf("%d: [son] pid %d from [parent] pid %d\n", i, getpid(), getppid());
			execvp(commands[i][0], commands[i]);
			printf("execvp() failed.\n");
			exit(0);
		}

	}
	/*************************************************************/

	/* Wait for all forked processes to finish execution */
	for(int i = 0; i < num_commands; i++)
	{
		printf("waiting\n");
		wait(NULL);
	}
	/*************************************************************/

	printf("Done waiting\n");

	freeCommands(commands);
}
