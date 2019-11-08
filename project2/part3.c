/*
* Description: Project 2 Part 3.
*
* Author: Samuel Lundquist
*
* Date: 11/6/2019
*/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
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

void handler(int signal)
{
	printf("    Child %d - Recieved signal %d, starting process...\n", getpid(), signal);
}

int flag = 0;
void alarmHandler(int signal)
{
	flag = 1;
	alarm(2);
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
	pid_t pids[num_commands];
	pid_t w;
	int status;
	int index = 0;

	int sig;
	sigset_t set;
	sigemptyset(&set);
	sigaddset(&set, SIGUSR1);
	sigprocmask( SIG_BLOCK, &set, NULL );

	signal(SIGUSR1, handler);
	for(int i = 0 ; i < num_commands ; i++)
	{
		pid_t pid;
		if((pid = fork()) == 0)
		{
			printf("    Child %d sucessfully forked\n", getpid());
			sigwait(&set, &sig);
			execvp(commands[i][0], commands[i]);
			printf("    Child %d execvp() failed.\n", getpid());
			exit(EXIT_FAILURE);
		}
		if(pid < 0)
		{
			perror("Error! could not create a new process.\n");
			exit(EXIT_FAILURE);
		}
		else
		{
			pids[i] = pid;
		}
	}
	/*************************************************************/

	sleep(1);
	for(int i = 0; i < num_commands; i++)
	{
		kill(pids[i], SIGUSR1);
	}

	for(int i = 1; i < num_commands; i++)
	{
		pid_t p = pids[i];
		if(kill(p, SIGSTOP) == 0)
		{
			printf("Successfully stopped process %d\n", p);
		}

	}

	signal(SIGALRM, alarmHandler);
	raise(SIGALRM);

	while(1)
	{
		pid_t current = pids[index];
		if(flag)
		{

		//If current process is running, stop it.
		//If no longer running, let user know it has been completed.
		if((w = waitpid(current, &status, WNOHANG)) >= 0)
		{
			if(kill(current, SIGSTOP) == 0)
			{
				printf("Successfully stopped process %d\n", current);
			}
		}
		else
		{
			printf("Successfully completed process %d.\n", current);
		}

		//Finds a new process to run from pids[] that hasn't finished executing.
		for(int i = index + 1; i != index; i++)
		{
			if(i >= num_commands){i = 0;};
			pid_t p = pids[i];
			if((w = waitpid(p, &status, WNOHANG)) >= 0)
			{
				//Continue process
				if(kill(p, SIGCONT) == 0)
				{
					printf("Successfully started process %d\n", p);
				}
				index = i;
				//An unfinished process has been found and started, stop search.
				break;
			}
		}

		//If new process is the current process, make sure it needs to finish
		//If it's already done, then we can end the program.
		if(pids[index] == current)
		{
			if((w = waitpid(current, &status, WNOHANG)) >= 0)
			{
				//Continue process
				if(kill(current, SIGCONT) == 0)
				{
					printf("Successfully started process %d\n", current);
				}
			}
			else
			{
				break;
			}
		}
		flag = 0;
		}
		if((w = waitpid(current, &status, WNOHANG)) < 0)
		{
			alarm(0);
			raise(SIGALRM);
		}
	}

	printf("Done waiting\n");

	freeCommands(commands);

	exit(EXIT_SUCCESS);
}
