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

typedef struct Command
{
	char* command;
	char** args;
}CMD;

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

	char **args;
	args = malloc(argsize * sizeof(char*));
	for (int x = 0 ; x < argsize ; x++)
	{
		args[x] = malloc(255 * sizeof(char));
	}

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

	CMD *commands;
	commands = malloc(sizeof(CMD) * numcommands);
	int command_index = 0;

	line = getline(&buf, &bsize, read_file);
	while(line != -1)
	{
		token = strtok(buf, s);
		if(token){
			command_name = token;
		} else {
			printf("Empty line in file.\n");
			break;
		}

		token = strtok(NULL, s);
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
			free2DArr(args, argsize);
			exit(EXIT_FAILURE);
		}
		commands[command_index].command = strdup(command_name);
		commands[command_index].args = arrayCopy(args, argsize);

		for (int x = 0; x < argsize; x++)
		{
			args[x] = NULL;
		}

		command_index++;
		line = getline(&buf, &bsize, read_file);
	}

	for(int a = 0 ; a < numcommands ; a++)
	{
		printf("%s\n", commands[a].command);
		printf("%s\n", commands[a].args[0]);
	}

	fclose(read_file);
	free(buf);
	free(token);
	free(commands);
	free2DArr(args, argsize);
	exit(EXIT_SUCCESS);
}
