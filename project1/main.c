/*
* Description: Project 1 Main File.
*
* Author: Samuel Lundquist
*
* Date: 10/16/2019
*
* Notes:
*
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "command.h"

void commandMode()
{
	/*

	Command Mode

	*/

	/* Function Variables */
	char* buf;
	size_t bsize = 32;
	const char s[2] = " ";
	char* token;
	int running = 1;

	/* Malloc Memory for Buffer */
	buf = (char*)malloc(bsize * sizeof(char));

	/* Main run loop */
	while(running)
	{
		/* Print >>> then get the input string */
		printf(">>> ");
		getline(&buf, &bsize, stdin);

		/* Tokenize Input String */
		token = strtok(buf, s);

		/* Handle Exit Case */
		if(strcmp(token, "exit\n") == 0)
		{
			running = 0;
			break;
		}
		else if(strcmp(token, "help\n") == 0)
		{
			printf("\n --- PSEUDO SHELL HELP --- \n\n"
				" ls | Show names and files in current directory.\n"
				" pwd | Shows the current directory.\n"
				" mkdir <name> | Make directory with name specified in <name>.\n"
				" cd <directory> | Changes directory based on destination in argument.\n"
				" cp <src> <dst> | Copy one file from the path <src> to location <dst>.\n"
				" mv <src> <dst> | Move one file from the path <src> to location <dst>.\n"
				" rm <filename> | Removes the specified file from the current directory.\n"
				" cat <filename> | Display the specified file's content on the console.\n"
				" help | Displays the current window.\n"
				" exit | Closes the pseudo shell.\n\n"
				" -------------------------\n\n"
			);
		}
		else if(strcmp(token, "") == 0)
		{

		}
		else if(strcmp(token, "") == 0)
		{

		}
		else if(strcmp(token, "") == 0)
		{

		}
		else if(strcmp(token, "") == 0)
		{

		}
		else if(strcmp(token, "") == 0)
		{

		}
		else if(strcmp(token, "") == 0)
		{

		}
		else if(strcmp(token, "") == 0)
		{

		}
		else
		{
			printf("Error! Unrecognized command: %s\n", token);
		}
	}
	free(buf);
	printf("Exited Successfully\n");
	return;
}

void fileMode(char* filename)
{
	/*

	File Mode

	*/
	return;
}

int main(int argc, char *argv[])
{
	/*

	Main handles flags, launches proper mode based on given arguments.
	If inproper arguments given, program exits.
	This ensures that memory is not allocated unless needed.

	*/
	if (argv[1] == NULL)
	{
		printf("\n[ERROR] A flag was not given for pseudo-shell.\n"
			"pseudo-shell takes either -command or -f <filename> as a flag.\n\n");
	}
	else if (strcmp(argv[1], "-command") == 0)
	{
		if(argv[2] == NULL)
		{
			commandMode();
		}
		else
		{
			printf("\n[ERROR] Too many arguments passed to -command "
				"flag.\n-command takes no arguments.\n\n");
		}
	}
	else if (strcmp(argv[1], "-f") == 0)
	{
		if(argv[2] != NULL)
		{
			if(argv[3] == NULL)
			{
				fileMode(argv[2]);
			}
			else
			{
				printf("\n[ERROR] Too many arguments passed to -f flag.\n"
					"-f takes a single filename as an argument.\n\n");
			}
		}
		else
		{
			printf("\n[ERROR] An argument was not passed to the -f flag.\n"
				"-f takes a single filename as an argument.\n\n");
		}
	}
	else
	{
		printf("\n[ERROR] An unknown flag was given to pseudo-shell.\n"
			"pseudo-shell takes either -command or -f <filename> as a flag.\n\n");
	}
	return 0;
}
