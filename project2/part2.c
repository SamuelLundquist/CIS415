/*
* Description: Project 2 Part 2.
*
*	Read the program workload from an input file. Each line in the file contains
*	the name of the program(command) and its arguments. For each program,
*	launch the program to run as a separate process using fork() and execvp().
*	Have each forked child process wait for a SIGUSR1​ signal before calling
*	execvp() by using sigwait(). After all of the child processes have been
*	forked and are now waiting, the parent process must send each child process
*	a ​SIGUSR1​ signal to wake them up. After all of the workload programs have
*	been launched and are now executing, send each child process a ​SIGSTOP​
*	signal to suspend them. After all of the child processes have been
*	suspended, send each child process a ​SIGCONT​ signal to wake them up.
*	Again, once all of the processes are back up and running, wait for each
*	child process to terminate. After all processes have terminated, exit and
*	free any allocated memory.
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
	/*
		A function used by arrayFromString() to malloc a 2D
		array of chars.

		Input:
		- num | number of char pointers to malloc
		- char_size | number of chars in each char pointer to malloc

		Output:
		- new_arr | 2D Char array [num][char_size]
	*/
	char **new_arr = malloc(num * sizeof(char*));
	for (int i = 0; i < num; i++)
	{
		new_arr[i] = malloc(char_size * sizeof(char));
	}
	return new_arr;
}

char **arrayFromString(char* s, char* delim, int index, int char_size)
{
	/*
		A recursive function that uses strtok() to break down a string
		into a 2D char array. The reason for implementing this function
		is to take a command from a line of text in a text file, and put
		it into char **argv format.

		Input:
		- s | A string that we want to break into 2D char array
		- delim | The delimiter used by strtok() to separate tokens
		- index | The index of the current token from the string
		- char_size | The size of the longest token in the string

		Output:
		- arr | A 2D char array of the string passed to the original
				arrayFromString() command. The last char* in the array
				is NULL.
	*/
	char* word;
	/* If first call to strtok do this */
	if(index == 0)
	{
		word = strtok(s, delim);
	}
	/* Otherwise do this */
	else
	{
		word = strtok(NULL, delim);
	}

	/*
	If word == NULL, there is nothing left in the string. At this point we can
	malloc a 2D array and initialize the last value to NULL. Then we return
	this array and the rest of the previous calls to arrayFromString() can
	input their char* 'word' to the proper index in array.
	*/
	if(word == NULL)
	{

		char** arr = malloc2DArr(index + 1, char_size);
		free(arr[index]);
		arr[index] = NULL;
		return arr;
	}

	/*
	Word is not NULL, so we check its length to see if it is longer than any
	previous words. If it is, then we update char_size. We then make a recursive
	call to arrayFromString() to tokenize the rest of the string. We get an
	array from that call and update our current 'index' value in that array to
	char* 'word'. Once that is done, return the 2D array.
	*/
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
	/*
		getCommands() is a recursive function designed to take an input file and
		break it down into a 3D array of commands to be run by the program.
		This is done dynamically so that commands of any size can be entered in
		the file, assuming that command can fit inside a 255 char buffer.

		Input:
		- fp | A file pointer
		- index | The index/line of the current command in the file

		Output:
		- arr | A 3D char array of commands from a file
	*/

	/* Define variables */
	char delim[6] = " \n\r";
	char* buf;
	size_t bsize = 255;
	int line;

	/* Malloc buffer for reading lines from file and handle malloc error */
	buf = malloc(bsize * sizeof(char));
	if(buf == NULL)
	{
		perror("Unable to allocate buffer.\n");
		exit(EXIT_FAILURE);
	}

	/* Get line from file */
	line = getline(&buf, &bsize, fp);

	/*
		line == -1 indicates that there are no more lines in the file. At this
		point, the function mallocs a 3D array of 2D char arrays where the last
		value in the array is equal to NULL. Note that only the top layer of
		the array is malloced. This is because all previous calls to
		getCommands() already malloced 2D arrays for each command, so those
		can be assigned to their specific index in arr after this returns.
	*/
	if(line == -1)
	{
		free(buf);
		char ***arr = malloc((index + 1) * sizeof(char**));
		free(arr[index]);
		arr[index] = NULL;
		return arr;
	}
	/*
		Takes a line from the file and sends it to arrayFromString() along with
		a delimiter to break the line down into char **argv format. Then
		makes a susequent recursive call to getCommands() to break down the
		rest of the file. Once this call returns, the command received
		from arrayFromString can be updated into the current index in arr.

	*/
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
	/*
		Thic command takes 3D arrays made from getCommands() and frees
		them so that there are no memory leaks.

		Input:
		- arr | A 3D char array

		Output:
		- void
	*/
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
	/*
		Signal handler for SIGUSR1. Indicates when SIGUSR1 has been sent to pid.
		In main we have sigwait() set up on each child process after the initial
		fork. Each child process with wait until SIGUSR1 has been raised before
		they begin execution.

		Input:
		- signal | SIGUSR1

		Output:
		- void
	*/
	printf("    Child %d - Recieved signal %d, starting process...\n", getpid(), signal);
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

	/* Find num commands, needed later */
	int num_commands = 0;
	char **z;
	while((z = commands[num_commands]) != NULL)
	{
		int j = 0;
		char *p;
		while((p = z[j]) != NULL)
		{
			printf("%s ", p);
			j++;
		}
		printf("\n");

		/* Increment here to count number of commands in 3D array commands */
		num_commands++;
	}
	/*************************************************************/

	/* Fork processes and run execvp() for each command in commands */
	pid_t pids[num_commands];
	int status;
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

	/* Wait a second for all processes to fork properly before signaling */
	sleep(1);

	/* Send SIGUSR1 to all child processes */
	for(int i = 0; i < num_commands; i++)
	{
		kill(pids[i], SIGUSR1);
	}
	/*************************************************************/

	/* Stop all child processes using SIGSTOP */
	printf("Stopping all processes...\n");
	for(int i = 0; i < num_commands; i++)
	{
		pid_t p = pids[i];
		if(kill(p, SIGSTOP) == 0)
		{
			printf("Successfully stopped process %d\n", p);
		}
	}
	/*************************************************************/

	/* Sleeping for added suspense between stop and continuing processes */
	sleep(3);

	/* Continue all processes using SIGCONT */
	printf("Continuing all processes...\n");
	for(int i = 0; i < num_commands; i++)
	{
		pid_t p = pids[i];
		if(kill(pids[i], SIGCONT) == 0)
		{
			printf("Successfully started process %d\n", p);
		}
	}
	/*************************************************************/

	/* Wait for all forked processes to finish execution */
	for(int i = 0; i < num_commands; i++)
	{
		pid_t pd = pids[i];
		printf("Waiting for child %d\n", pd);
		waitpid(pd, &status, 0);
	}
	printf("Done waiting\n");
	/*************************************************************/

	/* Done with commands, free them */
	freeCommands(commands);

	/* YAY! */
	exit(EXIT_SUCCESS);
}
