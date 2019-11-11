/*
* Description: Test file for exec commands
*
* Author: Samuel Lundquist
*
* Date: 11/6/2019
*/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char **argv)
{
	/*
		This is a simple test function for the exec command in project 2.
		Given a positive int n as argument, sleeps for n seconds.
	*/
	int i;

	if(argc > 2)
	{
		printf("Too many arguments passed to test, test takes 1 argument.\n");
		exit(EXIT_FAILURE);
	}
	else if(argc < 2)
	{
		printf("No arguments passed to test, test takes 1 argument.\n");
		exit(EXIT_FAILURE);
	}
	else
	{
		if((i = atoi(argv[1])))
		{
			for(int a = 0; a < i; a++)
			{
				sleep(1);
			}
		}
		else
		{
			printf("Invalid input for test, test take a positive integer\n");
			exit(EXIT_FAILURE);
		}
	}
	exit(EXIT_SUCCESS);
}
