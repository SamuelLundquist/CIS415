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

int commandCheck(char* cmd)
{
	char commands[11][6] = { "ls", "pwd", "mkdir", "cd", "cp", "mv", "rm", "cat", "help", "exit" };

	for(int i = 0; i < 10; i++)
	{
		if(strcmp(cmd, commands[i]) == 0)
		{
			return i+1;
		}
	}
	return 0;
}

int commandHandler(char* command)
{
	/*
	--- Command Handler Function ---
	Handles any function given to the
	shell from either commandMode or
	fileMode.

	Input: A char pointer

	Output: Int
	0 = exit
	1 = success
	2 = error
	*/
	const char s[2] = " ";
	char* token;
	char* saveptr2;

	token = strtok_r(command, s, &saveptr2);

	switch(commandCheck(token))
	{
		case 1: //ls
			/* Error Checking */
			token = strtok_r(NULL, s, &saveptr2);
			if (token != NULL)
			{
				if(commandCheck(token))
				{
					printf("Error! Incorrect syntax. No control code found.\n");
					return 2;
				}
				printf("Error! Unsupported parameters for command: ls\n");
				return 2;
			}
			listDir();
			return 1;

		case 2: //pwd
			/* Error Checking */
			token = strtok_r(NULL, s, &saveptr2);
			if (token != NULL)
			{
				if(commandCheck(token))
				{
					printf("Error! Incorrect syntax. No control code found.\n");
					return 2;
				}
				printf("Error! Unsupported parameters for command: pwd\n");
				return 2;
			}
			return 1;

		case 3: //mkdir
			return 1;

		case 4: //cd
			return 1;

		case 5: //cp
			return 1;

		case 6: //mv
			return 1;

		case 7: //rm
			return 1;

		case 8: //cat
			return 1;

		case 9: //Help
			token = strtok_r(NULL, s, &saveptr2);
			if (token != NULL)
			{
				if(commandCheck(token))
				{
					printf("Error! Incorrect syntax. No control code found.\n");
					return 2;
				}
				printf("Error! Unsupported parameters for command: help\n");
				return 2;
			}
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
			return 1;

		case 10: //Exit
			token = strtok_r(NULL, s, &saveptr2);
			if (token != NULL)
			{
				if(commandCheck(token))
				{
					printf("Error! Incorrect syntax. No control code found.\n");
					return 2;
				}
				printf("Error! Unsupported parameters for command: exit\n");
				return 2;
			}
			return 0;

		case 0: //Command unknown
			printf("Error! Unrecognized command: %s\n", token);
			return 2;
	}
	return 1;
}

void commandMode()
{
	/*
	--- Command Mode ---
	The mode used when the user
	inputs the -command flag when
	the pseudo shell is run from
	the command line.

	Uses getline(3) to take user input.
	This input is then separated by ";"
	into tokens. Each token is then processed
	by commandHandler until finished, or
	until an error occurs.

	Will not finish execution until the "exit"
	command has been entered by the user.
	*/

	/* Function Variables */
	char* buf;
	size_t bsize = 32;
	const char sc[4] = ";\n";
	char* command_token;
	char* saveptr;
	int running = 1;

	/* Malloc Memory for Buffer */
	buf = (char*)malloc(bsize * sizeof(char));
	if(buf == NULL)
	{
		printf("Memory for buffer not allocated.\n");
		exit(1);
	}

	/* Main run loop */
	while(running)
	{
		/* Print >>> then get the input string */
		printf(">>> ");
		getline(&buf, &bsize, stdin);

		/* Tokenize Input String */
		command_token = strtok_r(buf, sc, &saveptr);

		/* Runs until all commands have gone through commandHandler() */
		while(command_token != NULL)
		{
			/* commandHandler() returns 2 on error and 0 on exit*/
			running = commandHandler(command_token);
			if(!running || running == 2)
			{
				break;
			}
			command_token = strtok_r(NULL, sc, &saveptr);
		}
	}
	printf("Exited successfully.\n");
	free(buf);
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
	Also so that extra variables are not assigned.
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
