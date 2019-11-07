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
	printf("    Child Process: %d - Recieved signal: %d\n", getpid(), signal);
	sigset_t set;
	sigemptyset(&set);
	sigaddset(&set, SIGUSR1);
	sigprocmask(SIG_BLOCK, &set, NULL);
	int sig;
	sigwait(&set, &sig);
}

int	 main()
{
	//variable definitions
	pid_t  pid, w;
	int wstatus, i;
	i=0;

	struct sigaction action;
	action.sa_handler = handler;
	sigemptyset(&action.sa_mask);

	pid = fork();
	if(pid < 0)
	{
		perror("Error! could not create a new process.\n");
		exit(EXIT_FAILURE);
	}
	else if (pid == 0)
	{
		printf("	Child Process: %i - Starting infinite loop...\n", getpid());
		//sleep(3);
		sigaction(SIGUSR1, &action, NULL);
		while(1)
		{
			printf("	Child Process: %i - Starting infinite loop...\n", getpid());
			sleep(1);
		}
		exit(EXIT_SUCCESS);
	}
	else
	{

	}
	kill(pid, SIGUSR1);
	sleep(4);
	kill(pid, SIGUSR1);
	sleep(2);
	//kill(pid, SIGQUIT);
	printf("	WAITING\n");
	w = waitpid(pid, &wstatus, WUNTRACED | WCONTINUED);

	return EXIT_SUCCESS;
}
/*-----------------------------Program End-----------------------------------*/
