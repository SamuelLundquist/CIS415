#include <stdlib.h>
#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>

int main(int argc, char** argv)
{
	pid_t ch;
	pid_t w;
	int wstatus;

	ch = fork();
	if(ch == -1)
	{
		perror("Error with fork()");
		return 0;
	}

	if(ch == 0)
	{
		printf("Child Process ID is %ld\n", (long) getpid());
		if(argc == 1)
		{
			pause();

		}
		_exit(atoi(argv[1]));
	}

	else
	{
		w = waitpid(ch, &wstatus, WUNTRACED | WCONTINUED);
		if (w == -1)
		{
			perror("waitpid");
			return 0;
		}
		return 1;
	}


	return 0;
}
