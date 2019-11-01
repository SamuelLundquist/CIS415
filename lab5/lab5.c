/*
* Description: A simple lab showing signal processing
*
* Author: Jared Hall
*
* Date: 10/21/2019
*
* Notes:
* 1. to be done in lab
*/

/*-------------------------Preprocessor Directives---------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <dirent.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
/*---------------------------------------------------------------------------*/

/*-----------------------------Program Main----------------------------------*/

void handler(int signal)
{
	printf("    Child Process: %d - Recieved signal: %d", getpid(), signal);
	sigset_t set;
	sigemptyset(&set);
	sigaddset(&set, SIGUSR1);
	sigwait(&set, &signal);
}

int	 main()
{
	//variable definitions
	pid_t  pid, w;
	int wstatus, i;
	i=0;

	struct sigaction action;
	action.sa_handler = handler;
	sigaction(SIGUSR1, &action, NULL);

	pid = fork();
	if(pid < 0)
	{
		perror("Error! could n ot create a new process.\n");
		exit(EXIT_FAILURE);
	}
	else if (pid == 0)
	{
		printf("	Child Process: %i - Starting infinite loop...\n", getpid());
		sleep(3);
		/*
		while(1)
		{
			i++;
			if(i%10000 == 0)
			{
				printf("	Child Process: %i - Running infinite loop...\n", getpid());
				i=0;
			}
		}
		*/
		printf("    Child process: %d - Finished.\n", getpid());
		exit(EXIT_SUCCESS);
	}
	else
	{
		//else this is an existing proc i.e. the parent
		printf("Parent Process: %i, Waiting for child to finish...\n", getpid());
		kill(SIGUSR1, pid);
		w = waitpid(pid, &wstatus, WUNTRACED | WCONTINUED);
		printf("All child processes joined. Exiting.\n");
	}

	return EXIT_SUCCESS;
}
/*-----------------------------Program End-----------------------------------*/
