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

int main(int argc, char **argv)
{
	/*
		This is a simple test function for the exec command in project 2.
		Given a positive int n as argument, prints "Test" n times.
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
			while(i > 0)
			{
				printf("Test\n");
				i--;
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
