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

char **malloc2DArr(int len)
{
	char **arr = (char**)malloc(len * sizeof(char*));
	for (int i = 0; i < len; i++)
	{
		arr[i] = (char*)malloc(64 * sizeof(char));
		arr[i] = NULL;
	}
	return arr;
}

void free2DArr(char **arr, int len)
{
	for (int i = 0; i < len; i++)
	{
		free(arr[i]);
	}
	free(arr);
	return;
}

char **arrayCopy(char **arr, int len)
{
	char **new_arr;
	new_arr = malloc(len * sizeof(char*));
	for (int i = 0; i < len; i++)
	{
		new_arr[i] = malloc(255 * sizeof(char));
		char *str = arr[i];
		if(str)
		{
			new_arr[i] = strdup(arr[i]);
		}
	}
	return new_arr;

}

int lineCounter(char* filename)
{
	int numlines = 0;
	char ch;
	FILE* fp = fopen(filename, "r");
	if(fp == NULL)
	{
		return 0;
	}
	while(1)
	{
		if(feof(fp))
		{
			if(ch == '\n')
			{
				break;
			}
			else
			{
				numlines++;
				break;
			}
		}
		ch = fgetc(fp);
		if(ch == '\n')
		{
			numlines++;
		}
	}
	fclose(fp);
	return numlines;
}

int commandHandler(int argc, char **argv)
{
	return 1;
}

int main(int argc, char **argv)
{
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

	//Initialize Variables
	int numcommands;
	char *token;
	int line;
	char *buf;
	size_t bsize = 255;
	const char s[6] = " \n\r";
	int i = 0;
	char *command_name;
	int argsize = 6;

	numcommands = lineCounter(argv[1]);
	if(numcommands == 0)
	{
		perror("[ERROR] fopen()");
		exit(EXIT_FAILURE);
	}

	FILE* read_file = fopen(argv[1], "r");
	if(read_file == NULL)
	{
		perror("[ERROR] fopen()");
		exit(EXIT_FAILURE);
	}

	char ***commands;
	commands = malloc(sizeof(char**) * numcommands);
	int command_index = 0;

	char **args = malloc2DArr(argsize);

	buf = malloc(bsize * sizeof(char));

	line = getline(&buf, &bsize, read_file);
	while(line != -1)
	{
		token = strtok(buf, s);
		while(token != NULL && i < argsize)
		{

			args[i] = token;
			i++;
			token = strtok(NULL, s);
		}
		i = 0;

		if(token != NULL)
		{
			printf("\n[ERROR] Too many arguments given, max number of args is %d.\n", argsize);
			fclose(read_file);
			free(buf);
			free(token);
			free(command_name);
			for(int i = 0; i < numcommands; i++)
			{
				free2DArr(commands[i], argsize);
			}
			free(commands);
			free2DArr(args, argsize);
			exit(EXIT_FAILURE);
		}

		int len = 0;
		for (int i = 0; i < argsize; i++)
		{
			if(args[i] != NULL)
			{
				printf("%s ", args[i]);
				len++;
			}
			else
			{
				break;
			}
		}
		printf(" :%d\n", len);
		commands[command_index] = arrayCopy(args, len);
		for (int i = 0; i < argsize; i++)
		{
			args[i] = NULL;
		}
		command_index++;
		line = getline(&buf, &bsize, read_file);
	}
	//free2DArr(args, argsize);
	fclose(read_file);
	free(buf);
	free(token);

	for(i = 0 ; i < numcommands ; i++)
	{
		if(fork() == 0)
		{
			char **command = commands[i];
			execvp(command[0], command);
			printf("%d: [son] pid %d from [parent] pid %d\n", i, getpid(), getppid());
			exit(0);
		}

	}
	for(i = 0; i < numcommands; i++)
	{
		wait(NULL);
	}
	printf("Done waiting\n\n");

	for(int i = 0; i < numcommands; i++)
	{
		free2DArr(commands[i], argsize);
	}
	free(commands);
	exit(EXIT_SUCCESS);
}
