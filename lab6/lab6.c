/*-----------------------------------------------------------------------------
File name: Lab6.c
Description: A simple lab showing the use of signals.
Author: Jared Hall
-----------------------------------------------------------------------------*/

/*---------------------------Preprocessor Directives-------------------------*/
#include <sys/wait.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <signal.h>
/*---------------------------------------------------------------------------*/

int print = 1;
int seconds = 10;

/*---------------------------------Functions---------------------------------*/
void alarmHandler(int signal)
{
	seconds--;
	print = 1;
}
/*---------------------------------------------------------------------------*/

/*--------------------------------Program Main-------------------------------*/
int main(void)
{
	//variable declarations
	pid_t pid, w;
	int wstatus, eStatus;

	signal(SIGALRM, alarmHandler);

	//create a child process
	pid = fork();
	if(pid < 0)
	{
		perror("Error! could not create a new process.\n");
		exit(EXIT_FAILURE);
	}
	else if(pid == 0)
	{
		//This code runs in the child process only
		printf("  Child process %d - Starting...\n", getpid());
		while(seconds)
		{
				if(print)
				{
					printf("​ Child process %d - Still alive after for %d seconds\n", getpid(), seconds);
					print = 0;
					alarm(1);
				}
		}
		printf("  Child process %d - Finished.\n", getpid());
		exit(EXIT_SUCCESS);

	}
	else if(pid > 0)
	{

		sleep(1);
		while((w = waitpid(pid, &wstatus, WNOHANG)) == 0)
		{
			printf("[PARENT] process: %d - Sending signals to child...\n", getpid());
			kill(pid, SIGSTOP);
			sleep(3);
			kill(pid, SIGCONT);
			sleep(1);
		}
		printf("[PARENT] process: %d - Finished\n", getpid());
	}

	//exit out of program
	return EXIT_SUCCESS;
}
/*----------------------------------------------------------------------------*/
